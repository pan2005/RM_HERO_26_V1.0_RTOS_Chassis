#include "referee_protocol.h"
#include "robot_global.h"
#include "bsp_buzzer.h"
extern robot_ctrl_info_t robot_ctrl;
extern int ui_self_id;

/**
 * @brief 裁判系统数据包拆包与解析 (工业级手动解包，免疫对齐问题)
 */
void Referee_Data_Solve(uint8_t *frame, uint16_t len) {
    uint16_t read_index = 0;

    while (read_index < len) {

        // 1. 寻找帧头 0xA5
        if (frame[read_index] != 0xA5) {
            read_index++;
            continue;
        }

        // 2. 检查长度是否够包头(5字节)
        if (len - read_index < 5) break;

        // 3. 提取数据段长度 (小端)
        uint16_t data_length = (frame[read_index + 2] << 8) | frame[read_index + 1];
        uint16_t frame_length = 9 + data_length;

        // 4. 检查剩余数据是否够一整个包
        if (len - read_index < frame_length) break;

        // 5. 提取 CMD_ID 和数据段首地址
        uint16_t cmd_id = (frame[read_index + 6] << 8) | frame[read_index + 5];
        uint8_t *data = &frame[read_index + 7];

        // ==========================================================
        // 6. 【核心修复】：手动移位解析，彻底抛弃 memcpy 和 sizeof
        // ==========================================================

        if (cmd_id == 0x0201) {
           // buzzer_on(50,15000);
            // 机器人状态数据解析
            robot_ctrl.referee.robot_status.robot_id    = data[0];
            robot_ctrl.referee.robot_status.robot_level = data[1];

            // 16位整数提取 (小端拼接：高字节左移8位 | 低字节)
            robot_ctrl.referee.robot_status.current_HP                   = (data[3] << 8) | data[2];
            robot_ctrl.referee.robot_status.maximum_HP                   = (data[5] << 8) | data[4];
            robot_ctrl.referee.robot_status.shooter_barrel_cooling_value = (data[7] << 8) | data[6];
            robot_ctrl.referee.robot_status.shooter_barrel_heat_limit    = (data[9] << 8) | data[8];
            robot_ctrl.referee.robot_status.chassis_power_limit          = (data[11] << 8) | data[10];

            // 位域手动掩码提取 (安全可靠)
            uint8_t power_mgr = data[12];
            robot_ctrl.referee.robot_status.power_management_gimbal_output  = (power_mgr & 0x01) ? 1 : 0;
            robot_ctrl.referee.robot_status.power_management_chassis_output = (power_mgr & 0x02) ? 1 : 0;
            robot_ctrl.referee.robot_status.power_management_shooter_output = (power_mgr & 0x04) ? 1 : 0;

            // 更新 UI ID
            ui_self_id = robot_ctrl.referee.robot_status.robot_id;
        }
        else if (cmd_id == 0x0202 && data_length == 14) {
            // 功率与热量数据解析 (直接跳过前 8 个没用的 reserved 字节)
            // 读取第 8,9 字节为缓冲能量
            robot_ctrl.referee.power_heat.buffer_energy            = (data[9] << 8) | data[8];
            // 读取第 10,11 字节为 17mm 热量
            robot_ctrl.referee.power_heat.shooter_17mm_barrel_heat = (data[11] << 8) | data[10];
            // 读取第 12,13 字节为 42mm 热量 (英雄用)
            robot_ctrl.referee.power_heat.shooter_42mm_barrel_heat = (data[13] << 8) | data[12];
        }

        // 7. 游标移动到下一个包
        read_index += frame_length;
    }
}
