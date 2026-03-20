//
// Created by PanZhaoHui on 26-3-17.
//

#include "referee_protocol.h"
#include "string.h"
#include "robot_global.h"

extern robot_ctrl_info_t robot_ctrl;

typedef REFEREE_PACKED struct {
    uint8_t robot_id;
    uint8_t robot_level;
    uint16_t current_HP;
    uint16_t maximum_HP;
    uint16_t shooter_barrel_cooling_value;
    uint16_t shooter_barrel_heat_limit;
    uint16_t chassis_power_limit;
    uint8_t power_management_gimbal_output : 1;
    uint8_t power_management_chassis_output : 1;
    uint8_t power_management_shooter_output : 1;
} referee_robot_status_raw_t;

typedef REFEREE_PACKED struct {
    uint16_t reserved1;
    uint16_t reserved2;
    float reserved3;
    uint16_t buffer_energy;
    uint16_t shooter_17mm_barrel_heat;
    uint16_t shooter_42mm_barrel_heat;
} referee_power_heat_raw_t;



extern int ui_self_id;

void Referee_Data_Solve(uint8_t *frame, uint16_t len) {
    if (frame[0] != 0xA5) {
        return;
    }

    uint16_t cmd_id = (frame[6] << 8) | frame[5];
    uint8_t *data_ptr = frame + 7;

    switch (cmd_id) {
        case 0x0201:
            memcpy(&robot_ctrl.referee.robot_status, data_ptr, sizeof(referee_robot_status_raw_t));
            ui_self_id = robot_ctrl.referee.robot_status.robot_id;
            break;

        case 0x0202:
            memcpy(&robot_ctrl.referee.power_heat, data_ptr + 6, sizeof(referee_power_heat_raw_t));
            break;

        default:
            break;
    }
}