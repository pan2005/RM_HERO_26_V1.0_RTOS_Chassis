#ifndef CAN_MOTOR_H
#define CAN_MOTOR_H

#include "main.h"
#include "pid.h"
#include "bsp_can.h"
#define PI 3.14159265358979f
#define TWO_PI 6.28318530717958f
struct Can_Motor_t;

// 函数指针定义
typedef void (*Motor_Decode_Func)(void *device, uint8_t *data);
typedef void (*Motor_Update_Func)(struct Can_Motor_t *self);

// 1. 通用反馈数据
typedef struct {
    uint16_t ecd;
    int16_t  speed_rpm;
    int16_t  given_current;
    uint8_t  temperate;
    uint16_t last_ecd;
} Motor_Measure_t;

// 2. M3508 特有数据结构 (暴露给应用层)
typedef struct {
    PID_t speed_pid;
    float target_speed;
} M3508_Data_t;

// 3. GM6020 特有数据结构 (暴露给应用层)
typedef struct {
    PID_t pos_pid;
    PID_t speed_pid;
    float target_angle;

    // 状态量
    int32_t round_count;
    uint8_t whether_extern_data; //是否使用外部传感器数据来做闭环
    float   total_angle;
    float   INS_angle;  //留一个传感器的接口
} GM6020_Data_t;

// 4. 通用对象
typedef struct Can_Motor_t {
    CAN_HandleTypeDef *hcan;
    uint32_t rx_id;
    int16_t  output_value;
    Motor_Measure_t measure;

    Motor_Decode_Func decode_func;
    Motor_Update_Func update_func;

    void *priv_data; // 指向 M3508_Data_t 或 GM6020_Data_t
} Can_Motor_t;

// 5. 函数原型声明
void Can_Motor_Init(Can_Motor_t *motor, CAN_HandleTypeDef *hcan, uint32_t rx_id,
                    Motor_Decode_Func decode, Motor_Update_Func update, void *priv);

// 具体的算法逻辑函数声明（供初始化时绑定）
void M3508_Decode(void * device, uint8_t *data);
void M3508_Update(Can_Motor_t *self);

void GM6020_Decode(void *device, uint8_t *data);
void GM6020_Update(Can_Motor_t *self);




void DJI_Motor_SendGroup_0x200(CAN_HandleTypeDef *hcan, int16_t c1, int16_t c2, int16_t c3, int16_t c4);
void DJI_Motor_SendGroup_0x1FF(CAN_HandleTypeDef *hcan, int16_t c1, int16_t c2, int16_t c3, int16_t c4);
void DJI_Motor_SendGroup_0x2FF(CAN_HandleTypeDef *hcan, int16_t c1, int16_t c2, int16_t c3);

float Radian_Normalize(float angle);
#endif