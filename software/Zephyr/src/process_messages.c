#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/data/json.h>
#include "constants.h"


extern struct k_msgq json_rx_queue;

LOG_MODULE_REGISTER(json_worker, LOG_LEVEL_INF);

struct position_t {
    double x;
    double y;
    double z;
};

struct orientation_t {
    double x;
    double y;
    double z;
    double w;
};

struct pose_t {
    struct position_t position;
    struct orientation_t orientation;
};

struct data_t {
    struct pose_t pose;
};

struct robot_msg_t {
    const char topic[64];
    struct data_t data;
};

static const struct json_obj_descr position_desc[] = {
    JSON_OBJ_DESCR_PRIM(struct position_t, x, JSON_TOK_NUMBER),
    JSON_OBJ_DESCR_PRIM(struct position_t, y, JSON_TOK_NUMBER),
    JSON_OBJ_DESCR_PRIM(struct position_t, z, JSON_TOK_NUMBER),
};


static const struct json_obj_descr orientation_desc[] = {
    JSON_OBJ_DESCR_PRIM(struct orientation_t, x, JSON_TOK_NUMBER),
    JSON_OBJ_DESCR_PRIM(struct orientation_t, y, JSON_TOK_NUMBER),
    JSON_OBJ_DESCR_PRIM(struct orientation_t, z, JSON_TOK_NUMBER),
    JSON_OBJ_DESCR_PRIM(struct orientation_t, w, JSON_TOK_NUMBER),
};


static const struct json_obj_descr pose_desc[] = {
    JSON_OBJ_DESCR_OBJECT(struct pose_t, position, position_desc),
    JSON_OBJ_DESCR_OBJECT(struct pose_t, orientation, orientation_desc),
};


static const struct json_obj_descr data_desc[] = {
    JSON_OBJ_DESCR_OBJECT(struct data_t, pose, pose_desc),
};

static const struct json_obj_descr robot_msg_desc[] = {
    JSON_OBJ_DESCR_PRIM(struct robot_msg_t, topic, JSON_TOK_STRING),
    JSON_OBJ_DESCR_OBJECT(struct robot_msg_t, data, data_desc),
};


void process_messages_thread(void *a, void *b, void *c)
{
    LOG_INF("Process messages thread started");

    char json_str[JSON_RX_MSG_MAX];

    while (1) {
        /* Wait forever for a JSON packet */
        k_msgq_get(&json_rx_queue, json_str, K_FOREVER);

        LOG_INF("Processing JSON: %s", json_str);

        // if (strstr(json_str, "robot_pose") == NULL)
        //     continue;

        // struct robot_msg_t msg = {0};

        // int ret = json_obj_parse(json_str, strlen(json_str),
        //                          robot_msg_desc,
        //                          ARRAY_SIZE(robot_msg_desc),
        //                          &msg);

        // if (ret < 0) {
        //     LOG_ERR("JSON parse error: %d", ret);
        //     continue;
        // }

        // LOG_INF("Topic: %s", msg.topic);

        // LOG_INF("Position: x=%d y=%d z=%d",
        //         (int)(msg.data.pose.position.x * 100000),
        //         (int)(msg.data.pose.position.y * 100000),
        //         (int)(msg.data.pose.position.z * 100000));

        // LOG_INF("Orientation: x=%d y=%d z=%d w=%d",
        //         (int)(msg.data.pose.orientation.x * 100000),
        //         (int)(msg.data.pose.orientation.y * 100000),
        //         (int)(msg.data.pose.orientation.z * 100000),
        //         (int)(msg.data.pose.orientation.w * 100000));

    }
}
