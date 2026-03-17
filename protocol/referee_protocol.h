//
// Created by PanZhaoHui on 26-3-17.
//

#ifndef REFEREE_PROTOCOL_H
#define REFEREE_PROTOCOL_H

#include "stdint.h"

#if defined(__GNUC__)
#define REFEREE_PACKED __attribute__((packed))
#else
#define REFEREE_PACKED
#endif

// 裁判系统数据包头结构体 (固定 5 字节)
typedef REFEREE_PACKED struct {
    uint8_t SOF;          // 0xA5
    uint16_t data_length; // 数据段长度
    uint8_t seq;          // 包序号
    uint8_t crc8;         // 帧头 CRC8
} frame_header_t;

// 0x0201 机器人性能体系数据 (13 字节)
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
} robot_status_t;

// 0x0202 实时功率热量数据 (14 字节，注意你的截图里前三个是 reserved)
typedef REFEREE_PACKED struct {
    uint16_t reserved1;
    uint16_t reserved2;
    float reserved3;
    uint16_t buffer_energy;             // 缓冲能量
    uint16_t shooter_17mm_barrel_heat;  // 17mm枪管热量 (步兵用)
    uint16_t shooter_42mm_barrel_heat;  // 42mm枪管热量 (英雄用)
} power_heat_data_t;

// 外部声明，供其他文件调用
extern robot_status_t robot_status;
extern power_heat_data_t power_heat_data;
extern int ui_self_id; // 从你的 ui_interface.c 中 extern 过来

void Referee_Data_Solve(uint8_t *frame, uint16_t len);

#endif //REFEREE_PROTOCOL_H
