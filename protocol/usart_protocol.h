#ifndef USART_PROTOCOL_H
#define USART_PROTOCOL_H

#include <stdint.h>

// --- 协议配置 ---
#define FRAME_HEADER    0xA5
#define FRAME_TAIL      0x5A
#define PROT_MIN_LEN    5

// --- 功能字定义 ---
#define CMD_ID_GIMBAL_INFO   0x01 // 云台状态 -> 底盘 (原有的)
#define CMD_ID_CHASSIS_CTRL  0x02 // 底盘控制 -> 云台 (新增)

#pragma pack(1)

// 1. 云台发送给底盘的数据包 (反馈状态)
typedef struct {
    float yaw_encoder;  //YAW轴电机编码器转换的角度  total angle

} GimbalInfo_t;

// 2. 底盘发送给云台的数据包 (运动指令 + 姿态)
typedef struct {
    int16_t vx;    // 前进速度
    int16_t vy;    // 横移速度
    int16_t vz;    // 旋转速度 (自旋)
    uint8_t fire;  // 开火标志 (0:不发射, 1:单发, 2:连发等)
    uint8_t mode;   // 是否使能，若为0则电机不出力
    float yaw;     // 底盘相对于云台或大地的偏航角
    float yaw_INS;  //绝对坐标系的偏航角
    float yaw_v_INS;
    uint8_t shoot_gear; // 摩擦轮挡位: 0=关闭, 1=低档, 2=高档
    float mouse_x_v;   // mouse.x变化率(前馈控制用)
    // float yaw_measure;   //用于位置的闭环计算
} ChassisInfo_t;

#pragma pack()

// --- 函数声明 ---
// 封包函数
void Protocol_Pack_GimbalInfo(GimbalInfo_t *info, uint8_t *tx_buf, uint16_t *tx_len);
void Protocol_Pack_ChassisInfo(ChassisInfo_t *info, uint8_t *tx_buf, uint16_t *tx_len);

// 通用解析函数：增加 cmd_id 输出，用于判断解析出了哪种结构体
uint8_t Protocol_Parse(uint8_t *rx_buf, uint16_t rx_len, uint8_t *out_cmd_id, void *out_struct);

#endif //USART_PROTOCOL_H