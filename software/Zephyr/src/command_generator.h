#include <zephyr/kernel.h>
#include <zephyr/random/random.h>
#include <zephyr/sys/printk.h>
#include <string.h>
#include "constants.h"

// topic constants for command type
#define SPORT_MODE_TOPIC "rt/api/sport/request"
#define OBSTACLE_AVOIDANCE_TOPIC "rt/api/obstacles_avoid/request"

typedef struct {
    uint32_t id;          // Unique command ID
    uint32_t api_id;      // ROBOT_CMD value
    const char* topic;    // WebRTC topic string
    const char* parameter; // JSON or string payload
} Go2Command;

/* generates the unique command ID based on timestamp and random number */
int generate_id() {
    uint64_t ms = k_uptime_get();  
    uint32_t timestamp_part = (uint32_t)(ms % 2147483648UL);
    uint32_t random_part = sys_rand32_get() % 1000;
    return timestamp_part + random_part;
}

// Converts a Go2Command struct into a JSON string
int build_go2_json(const Go2Command* cmd, char* out_json, size_t out_size)
{
    if (!cmd || !out_json) 
        return -1;

    return snprintk(out_json, out_size,
        "{"
          "\"type\":\"msg\","
          "\"topic\":\"%s\","
          "\"data\":{"
             "\"header\":{"
                 "\"identity\":{"
                     "\"id\":%u,"
                     "\"api_id\":%u"
                 "}"
             "},"
             "\"parameter\":\"%s\""
          "}"
        "}",
        cmd->topic,
        cmd->id,
        cmd->api_id,
        cmd->parameter
    );
}

int gen_command_json(
    uint32_t cmd,              // ROBOT_CMD value
    const char* parameters,    // can be NULL
    const char* topic,         
    uint32_t command_id,       // optional ID or 0 for auto
    char* out_json,            // JSON output buffer
    size_t out_size
) {
    Go2Command c;

    c.id = (command_id == 0) ? generate_id() : command_id;

    c.api_id = cmd;

    c.topic = (topic != NULL) ? topic : SPORT_MODE_TOPIC;

    static char param_buf[32];
    if (parameters != NULL) {
        c.parameter = parameters;
    } else {
        // Convert cmd int to string
        snprintk(param_buf, sizeof(param_buf), "%u", cmd);
        c.parameter = param_buf;
    }

    return build_go2_json(&c, out_json, out_size);
}


