
#define JSON_MSG_MAX 256
#define JSON_RX_MSG_MAX 512
#define JSON_MSG_QUEUE_LEN 10

#ifndef GO2_CONSTANTS_H
#define GO2_CONSTANTS_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * ============================================================
 *  SPORT_CMD (ID → Name mapping)
 * ============================================================
 *
 * This follows the original Python dict:
 * SPORT_CMD = {1001: "Damp", 1002: "BalanceStand", ... }
 *
 * In C we represent it as a struct table.
 */

typedef struct {
    const char* name;
    uint16_t id;
} Go2SportCmdEntry;

static const Go2SportCmdEntry SPORT_CMD[] = {
    {"Damp", 1001},
    {"BalanceStand", 1002},
    {"StopMove", 1003},
    {"StandUp", 1004},
    {"StandDown", 1005},
    {"RecoveryStand", 1006},
    {"Euler", 1007},
    {"Move", 1008},
    {"Sit", 1009},
    {"RiseSit", 1010},
    {"SwitchGait", 1011},
    {"Trigger", 1012},
    {"BodyHeight", 1013},
    {"FootRaiseHeight", 1014},
    {"SpeedLevel", 1015},
    {"Hello", 1016},
    {"Stretch", 1017},
    {"TrajectoryFollow", 1018},
    {"ContinuousGait", 1019},
    {"Content", 1020},
    {"Wallow", 1021},
    {"Dance1", 1022},
    {"Dance2", 1023},
    {"GetBodyHeight", 1024},
    {"GetFootRaiseHeight", 1025},
    {"GetSpeedLevel", 1026},
    {"SwitchJoystick", 1027},
    {"Pose", 1028},
    {"Scrape", 1029},
    {"FrontFlip", 1030},
    {"FrontJump", 1031},
    {"FrontPounce", 1032},
    {"WiggleHips", 1033},
    {"GetState", 1034},
    {"EconomicGait", 1035},
    {"FingerHeart", 1036},
};

#define SPORT_CMD_COUNT (sizeof(SPORT_CMD) / sizeof(SPORT_CMD[0]))

static int find_cmd_id(const char *name) {
    for (size_t i = 0; i < SPORT_CMD_COUNT; i++) {
        if (strcmp(name, SPORT_CMD[i].name) == 0) {
            return SPORT_CMD[i].id;
        }
    }
    return -1;  // Not found
}

/*
 * ============================================================
 *  ROBOT_CMD (Name → ID mapping)
 * ============================================================
 *
 * Mirrors the Python dict:
 * ROBOT_CMD = {"Damp":1001, "BalanceStand":1002, ... }
 *
 */

#define CMD_DAMP                 1001
#define CMD_BALANCE_STAND        1002
#define CMD_STOP_MOVE            1003
#define CMD_STAND_UP             1004
#define CMD_STAND_DOWN           1005
#define CMD_RECOVERY_STAND       1006
#define CMD_EULER                1007
#define CMD_MOVE                 1008
#define CMD_SIT                  1009
#define CMD_RISE_SIT             1010
#define CMD_SWITCH_GAIT          1011
#define CMD_TRIGGER              1012
#define CMD_BODY_HEIGHT          1013
#define CMD_FOOT_RAISE_HEIGHT    1014
#define CMD_SPEED_LEVEL          1015
#define CMD_HELLO                1016
#define CMD_STRETCH              1017
#define CMD_TRAJECTORY_FOLLOW    1018
#define CMD_CONTINUOUS_GAIT      1019
#define CMD_CONTENT              1020
#define CMD_WALLOW               1021
#define CMD_DANCE1               1022
#define CMD_DANCE2               1023
#define CMD_GET_BODY_HEIGHT      1024
#define CMD_GET_FOOT_HEIGHT      1025
#define CMD_GET_SPEED_LEVEL      1026
#define CMD_SWITCH_JOYSTICK      1027
#define CMD_POSE                 1028
#define CMD_SCRAPE               1029
#define CMD_FRONT_FLIP           1030
#define CMD_FRONT_JUMP           1031
#define CMD_FRONT_POUNCE         1032
#define CMD_WIGGLE_HIPS          1033
#define CMD_GET_STATE            1034
#define CMD_ECONOMIC_GAIT        1035
#define CMD_FINGER_HEART         1036


/*
 * ============================================================
 *  DATA_CHANNEL_TYPE (Strings)
 * ============================================================
 */

#define DC_VALIDATION       "validation"
#define DC_SUBSCRIBE        "subscribe"
#define DC_UNSUBSCRIBE      "unsubscribe"
#define DC_MSG              "msg"
#define DC_REQUEST          "request"
#define DC_RESPONSE         "response"
#define DC_VID              "vid"
#define DC_AUD              "aud"
#define DC_ERR              "err"
#define DC_HEARTBEAT        "heartbeat"
#define DC_RTC_INNER_REQ    "rtc_inner_req"
#define DC_RTC_REPORT       "rtc_report"
#define DC_ADD_ERROR        "add_error"
#define DC_RM_ERROR         "rm_error"
#define DC_ERRORS           "errors"


/*
 * ============================================================
 *  RTC_TOPIC_ALL (Full Topic Dictionary)
 * ============================================================
 */

typedef struct {
    const char* key;
    const char* value;
} Go2TopicEntry;

static const Go2TopicEntry RTC_TOPIC_ALL[] = {
    {"LOW_STATE", "rt/lf/lowstate"},
    {"MULTIPLE_STATE", "rt/multiplestate"},
    {"FRONT_PHOTO_REQ", "rt/api/videohub/request"},
    {"ULIDAR_SWITCH", "rt/utlidar/switch"},
    {"ULIDAR", "rt/utlidar/voxel_map"},
    {"ULIDAR_ARRAY", "rt/utlidar/voxel_map_compressed"},
    {"ULIDAR_STATE", "rt/utlidar/lidar_state"},
    {"ROBOTODOM", "rt/utlidar/robot_pose"},
    {"UWB_REQ", "rt/api/uwbswitch/request"},
    {"UWB_STATE", "rt/uwbstate"},
    {"LOW_CMD", "rt/lowcmd"},
    {"WIRELESS_CONTROLLER", "rt/wirelesscontroller"},
    {"SPORT_MOD", "rt/api/sport/request"},
    {"SPORT_MOD_STATE", "rt/sportmodestate"},
    {"LF_SPORT_MOD_STATE", "rt/lf/sportmodestate"},
    {"BASH_REQ", "rt/api/bashrunner/request"},
    {"SELF_TEST", "rt/selftest"},
    {"GRID_MAP", "rt/mapping/grid_map"},
    {"SERVICE_STATE", "rt/servicestate"},
    {"GPT_FEEDBACK", "rt/gptflowfeedback"},
    {"VUI", "rt/api/vui/request"},
    {"OBSTACLES_AVOID", "rt/api/obstacles_avoid/request"},
    {"SLAM_QT_COMMAND", "rt/qt_command"},
    {"SLAM_ADD_NODE", "rt/qt_add_node"},
    {"SLAM_ADD_EDGE", "rt/qt_add_edge"},
    {"SLAM_QT_NOTICE", "rt/qt_notice"},
    {"SLAM_PC_TO_IMAGE_LOCAL", "rt/pctoimage_local"},
    {"SLAM_ODOMETRY", "rt/lio_sam_ros2/mapping/odometry"},
    {"ARM_COMMAND", "rt/arm_Command"},
    {"ARM_FEEDBACK", "rt/arm_Feedback"},
    {"AUDIO_HUB_REQ", "rt/api/audiohub/request"},
    {"AUDIO_HUB_PLAY_STATE", "rt/audiohub/player/state"},
};

#define RTC_TOPIC_ALL_COUNT (sizeof(RTC_TOPIC_ALL) / sizeof(RTC_TOPIC_ALL[0]))


/*
 * ============================================================
 *  RTC_TOPIC (Minimal subset mirror)
 * ============================================================
 */

static const Go2TopicEntry RTC_TOPIC[] = {
    {"LOW_STATE", "rt/lf/lowstate"},
    {"MULTIPLE_STATE", "rt/multiplestate"},
    {"ULIDAR_SWITCH", "rt/utlidar/switch"},
    {"ULIDAR", "rt/utlidar/voxel_map"},
    {"ULIDAR_ARRAY", "rt/utlidar/voxel_map_compressed"},
    {"ULIDAR_STATE", "rt/utlidar/lidar_state"},
    {"ROBOTODOM", "rt/utlidar/robot_pose"},
    {"LOW_CMD", "rt/lowcmd"},
};

#define RTC_TOPIC_COUNT (sizeof(RTC_TOPIC) / sizeof(RTC_TOPIC[0]))


#ifdef __cplusplus
}
#endif

#endif // GO2_CONSTANTS_H
