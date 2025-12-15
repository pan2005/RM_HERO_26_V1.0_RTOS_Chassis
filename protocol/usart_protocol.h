//
// Created by PanZhaoHui on 25-12-15.
//

#ifndef USART_PROTOCOL_H
#define USART_PROTOCOL_H


#include <stdint.h>

// --- 协议配置 ---
#define FRAME_HEADER    0xA5
#define FRAME_TAIL      0x5A
// 包的最小长度 = 头(1)+Cmd(1)+Len(1)+Check(1)+尾(1) = 5
#define PROT_MIN_LEN    5

// --- 功能字定义 ---
#define CMD_ID_CHASSIS_INFO  0x01 // 底盘发给云台的信息

// --- 数据结构定义 ---
// 使用 #pragma pack(1) 确保结构体紧凑，没有字节对齐填充
#pragma pack(1)

// 1. 底盘发送给云台的数据包
typedef struct {
    float pitch_position; // 4字节
    uint8_t shoot_gear;   // 1字节
} ChassisInfo_t;

#pragma pack()

// --- 函数声明 ---
void Protocol_Pack_ChassisInfo(ChassisInfo_t *info, uint8_t *tx_buf, uint16_t *tx_len);
uint8_t Protocol_Parse(uint8_t *rx_buf, uint16_t rx_len, ChassisInfo_t *out_info);


#endif //USART_PROTOCOL_H
