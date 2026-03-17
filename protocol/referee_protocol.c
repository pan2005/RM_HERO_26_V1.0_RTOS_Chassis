//
// Created by PanZhaoHui on 26-3-17.
//

#include "referee_protocol.h"

#include "string.h"

// 定义全局变量，存储最新状态
robot_status_t robot_status;
power_heat_data_t power_heat_data;

// 从 ui_interface.c 引入 UI 自身 ID 变量
extern int ui_self_id;

/**
 * @brief 裁判系统数据解析核心函数
 * @param frame 指向收到的完整数据帧头 (0xA5 开头) 的指针
 * @param len 收到的数据长度
 */
void Referee_Data_Solve(uint8_t *frame, uint16_t len) {
    // 1. 强制类型转换，读取帧头
    frame_header_t *p_header = (frame_header_t *)frame;

    // 2. 校验包头 0xA5
    if (p_header->SOF != 0xA5) {
        return;
    }

    // 3. 提取命令码 CMD_ID (帧头5字节之后紧接着的2个字节)
    uint16_t cmd_id = (frame[6] << 8) | frame[5];

    // 4. 数据段起始地址指针 (帧头5字节 + CMD_ID 2字节 = 偏移量 7)
    uint8_t *data_ptr = frame + 7;

    // 5. 根据 CMD_ID 拷贝数据到对应结构体
    switch (cmd_id) {
        case 0x0201: // 机器人状态数据
            memcpy(&robot_status, data_ptr, sizeof(robot_status_t));
            ui_self_id = robot_status.robot_id;
            break;

        case 0x0202: // 功率与热量数据
            memcpy(&power_heat_data, data_ptr, sizeof(power_heat_data_t));
            break;

        default:
            break;
    }
}