#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/shell/shell.h> // register commands for 
#include <string.h>
#include <stdlib.h>     
#include "constants.h"
#include "command_generator.h"

LOG_MODULE_REGISTER(cmd_go2_shell, LOG_LEVEL_INF);

extern struct k_msgq json_tx_queue;
static char json_buffer[512];

static int cmd_go2_standup(const struct shell *shell, size_t argc, char **argv)
{
    ARG_UNUSED(argc);
    ARG_UNUSED(argv);

    int len = gen_command_json(
        CMD_STAND_UP,   // (1004) from your C #defines
        NULL,           // no parameters
        NULL,           // default SPORT_MODE_TOPIC
        0,              // auto ID
        json_buffer,
        sizeof(json_buffer)
    );
    LOG_INF("UART TX: %s", json_buffer);
    if (len > 0) {
        k_msgq_put(&json_tx_queue, json_buffer, K_NO_WAIT);
        shell_print(shell, "Sent StandUp command");
    } else {
        shell_error(shell, "Failed to build JSON");
    }

    return 0;
}

static int cmd_go2_generic(const struct shell *shell, size_t argc, char **argv)
{



    char json_buffer[512];
    int len = gen_command_json(
        CMD_STAND_DOWN,   // (1004) from your C #defines
        NULL,           // no parameters
        NULL,           // default SPORT_MODE_TOPIC
        0,              // auto ID
        json_buffer,
        sizeof(json_buffer)
    );
    LOG_INF("UART TX: %s", json_buffer);

    if (len > 0) {
        k_msgq_put(&json_tx_queue, json_buffer, K_NO_WAIT);
        shell_print(shell, "Sent StandDown command");
    } else {
        shell_error(shell, "Failed to build JSON");
    }

    return 0;
}


static int cmd_go2_standdown(const struct shell *shell, size_t argc, char **argv)
{
    ARG_UNUSED(argc);
    ARG_UNUSED(argv);

    char json_buffer[512];
    int len = gen_command_json(
        CMD_STAND_DOWN,   // (1004) from your C #defines
        NULL,           // no parameters
        NULL,           // default SPORT_MODE_TOPIC
        0,              // auto ID
        json_buffer,
        sizeof(json_buffer)
    );
    LOG_INF("UART TX: %s", json_buffer);

    if (len > 0) {
        k_msgq_put(&json_tx_queue, json_buffer, K_NO_WAIT);
        shell_print(shell, "Sent StandDown command");
    } else {
        shell_error(shell, "Failed to build JSON");
    }

    return 0;
}

static int cmd_go2_move(const struct shell *shell, size_t argc, char **argv)
{
    if (argc != 4) {
        shell_error(shell, "Usage: go2 move <x> <y> <yaw>. Scaled down by 1/10.");
        return -EINVAL;
    }

uint8_t x_tenth = strtol(argv[1], NULL, 10);
uint8_t y_tenth = strtol(argv[2], NULL, 10);
uint8_t z_tenth = strtol(argv[3], NULL, 10);

/* Debug print — still integer, no floats */
shell_print(shell, "Move cmd → x=0.%d y=0.%d z=0.%d",
            x_tenth, y_tenth, z_tenth);

/* Convert to JSON without floats */
char param_json[128];
/* 
"parameter": "{\"x\": 0.2, \"y\": 0.2, \"z\": 0.2}"}}*/
snprintk(param_json, sizeof(param_json),
         "{\\\"x\\\": 0.%d, \\\"y\\\": 0.%d, \\\"z\\\": 0.%d}",
         x_tenth, y_tenth, z_tenth);

    /* Build complete Unitree command JSON */
    char json_buffer[256];
    int len = gen_command_json(
        CMD_MOVE,           // API ID
        param_json,         // parameters
        SPORT_MODE_TOPIC,   // topic
        0,                  // auto-generate command ID
        json_buffer,
        sizeof(json_buffer)
    );

    if (len > 0) {
        k_msgq_put(&json_tx_queue, json_buffer, K_NO_WAIT);
        shell_print(shell, "Sent move command");
    } else {
        shell_error(shell, "Failed to build JSON");
        return EIO;
    }

    return 0;
}

static int cmd_go2_send(const struct shell *shell, size_t argc, char **argv)
{
    if (argc < 2) {
        shell_error(shell, "Usage: go2 send <CommandName> [param_json]");
        return -EINVAL;
    }

    const char *cmd_name = argv[1];
    int api_id = find_cmd_id(cmd_name);

    if (api_id < 0) {
        shell_error(shell, "Unknown command: %s", cmd_name);
        return -EINVAL;
    }

    const char *param_json = NULL;

    if (argc >= 3) {
        param_json = argv[2];   // raw JSON string from shell
    }

    char json_buffer[512];
    int len = gen_command_json(
        api_id,
        param_json,
        NULL,      // default SPORT_MODE_TOPIC
        0,         // autogenerate ID
        json_buffer,
        sizeof(json_buffer)
    );

    if (len <= 0) {
        shell_error(shell, "Failed to build JSON");
        return -EINVAL;
    }

    LOG_INF("UART TX: %s", json_buffer);
    k_msgq_put(&json_tx_queue, json_buffer, K_NO_WAIT);

    shell_print(shell, "Sent command: %s (%d)", cmd_name, api_id);
    return 0;
}


SHELL_STATIC_SUBCMD_SET_CREATE(sub_go2,
    SHELL_CMD(standup, NULL, "Send StandUp command", cmd_go2_standup),
    SHELL_CMD(standdown, NULL, "Send StandDown command", cmd_go2_standdown),
    SHELL_CMD(move, NULL, "Send movement (fractional decimal): go2 move <x> <y> <yaw>", cmd_go2_move),
    SHELL_CMD(send, NULL, "Send general command", cmd_go2_send),
    SHELL_SUBCMD_SET_END
);
SHELL_CMD_REGISTER(go2, &sub_go2, "Unitree Go2 control commands", NULL);
