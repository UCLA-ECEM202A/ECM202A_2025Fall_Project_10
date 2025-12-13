#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <string.h>
#include "constants.h"

extern struct k_msgq json_rx_queue;
extern struct k_msgq json_tx_queue;
extern const struct device *uart1;

LOG_MODULE_REGISTER(uart_io, LOG_LEVEL_INF);

#define START_BYTE 0xAA
#define TYPE_CMD   1

static uint8_t rx_buf[JSON_RX_MSG_MAX];
static int rx_pos = 0;
static int expected_len = -1;
static uint8_t state = 0;
static uint8_t type = 0;


static char tx_buf[JSON_MSG_MAX];

/* ---------------- CRC ---------------- */
uint8_t calc_crc(uint8_t *data, int len)
{
    uint8_t crc = 0;
    for (int i = 0; i < len; i++)
        crc ^= data[i];
    return crc;
}

/* ---------------- TX Helper ---------------- */
static void uart_send_packet(const char *json_str)
{
    if (!json_str || !uart1) {
        LOG_ERR("TX skipped: bad pointer");
        return;
    }

    size_t len = strlen(json_str);
    if (len == 0) return;
    if (len > 255) len = 255;

    uint8_t header[3] = {START_BYTE, (uint8_t)len, TYPE_CMD};

    uint8_t crc = calc_crc(&header[1], 2);
    crc ^= calc_crc((uint8_t *)json_str, len);

    /* HEADER */
    uart_poll_out(uart1, header[0]);
    uart_poll_out(uart1, header[1]);
    uart_poll_out(uart1, header[2]);

    /* PAYLOAD */
    for (int i = 0; i < len; i++) {
        uart_poll_out(uart1, json_str[i]);
    }
    /* CRC */
    uart_poll_out(uart1, crc);

    LOG_INF("UART TX: %s", json_str);
}

/* ---------------- Unified UART Thread ---------------- */
void uart_rx_tx_thread(void *a, void *b, void *c)
{
    LOG_INF("UART IO thread started");

    uint8_t byte;

    while (!device_is_ready(uart1)) {
        LOG_WRN("Waiting for UART1...");
        k_msleep(20);
    }

    LOG_INF("UART1 READY: %p", uart1);

    while (1) {

        /*************** RX HANDLING ***************/
        while (uart_poll_in(uart1, &byte) == 0) {

            switch (state) {

            case 0: // waiting for start
                if (byte == START_BYTE) {
                    state = 1;
                }
                break;

            case 1: // length high
                expected_len = byte << 8;
                state = 2;
                break;

            case 2: // length low
                expected_len |= byte;
                state = 3;
                break;

            case 3: // type
                type = byte;
                rx_pos = 0;
                state = 4;
                break;

            case 4: // payload
                rx_buf[rx_pos++] = byte;

                if (rx_pos == expected_len) {
                    state = 5;
                }
                break;

            case 5: // crc
                uint8_t crc = byte;

                uint8_t calc = 0;
                calc ^= (expected_len >> 8) & 0xFF;
                calc ^= expected_len & 0xFF;
                calc ^= type;

                for (int i = 0; i < expected_len; i++) {
                    calc ^= rx_buf[i];
                }

                if (calc == crc) {
                    // Valid packet -> push JSON
                    char json_msg[JSON_RX_MSG_MAX] = {0};
                    LOG_INF("Received message: %s", rx_buf);
                    memcpy(json_msg, rx_buf, MIN(expected_len, JSON_RX_MSG_MAX - 1));
                    k_msgq_put(&json_rx_queue, json_msg, K_NO_WAIT);
                    
                } else {
                    LOG_ERR("CRC mismatch");
                }

                state = 0;
                break;
            }
        }

        /*************** TX HANDLING ***************/
        if (k_msgq_get(&json_tx_queue, &tx_buf, K_NO_WAIT) == 0) {
            uart_send_packet(tx_buf);
        }

        k_msleep(1);
    }
}
