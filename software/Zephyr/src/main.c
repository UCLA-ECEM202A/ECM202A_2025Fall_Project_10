#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>

#include <string.h>
#include "threads.h"
#include "constants.h"

LOG_MODULE_REGISTER(main_app, LOG_LEVEL_INF);

#define JSON_MSG_MAX 256
#define JSON_RX_MSG_MAX 512
#define JSON_MSG_QUEUE_LEN 10

/* Queue for JSON strings received from UART */
K_MSGQ_DEFINE(json_rx_queue, JSON_RX_MSG_MAX, JSON_MSG_QUEUE_LEN, 4);

/* Queue for commands created by the user */
K_MSGQ_DEFINE(json_tx_queue, JSON_MSG_MAX, JSON_MSG_QUEUE_LEN, 4);

/* UART devices */

/* espressif serial monitor - uart device 0 */
// const struct device *uart0 = DEVICE_DT_GET(DT_NODELABEL(uart0));

/* raspberry pi connection - uart device 1 */
const struct device *uart1 = DEVICE_DT_GET(DT_NODELABEL(uart1));

/* Thread stacks */
K_THREAD_STACK_DEFINE(uart_rx_stack, 4096);
K_THREAD_STACK_DEFINE(process_messages_stack, 4096);
// K_THREAD_STACK_DEFINE(user_input_stack, 4096);

/* Thread control blocks */
static struct k_thread uart_rx_tcb;
static struct k_thread process_messages_tcb;
// static struct k_thread user_input_tcb;

int main(void)
{
    LOG_INF("Zephyr app starting...");

    /* Start UART RX/TX thread */
    k_thread_create(&uart_rx_tcb, uart_rx_stack, K_THREAD_STACK_SIZEOF(uart_rx_stack),
                    uart_rx_tx_thread, NULL, NULL, NULL,
                    5, 0, K_NO_WAIT);

    /* Start JSON processing thread */
    k_thread_create(&process_messages_tcb, process_messages_stack, K_THREAD_STACK_SIZEOF(process_messages_stack),
                    process_messages_thread, NULL, NULL, NULL,
                    7, 0, K_NO_WAIT);

    return 0;
}