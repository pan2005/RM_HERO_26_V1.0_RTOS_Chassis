#include "dji_motor.h"

/**
  * @brief 具体的解码逻辑 (私有函数)
  * @param device: 在注册时传入的 dji_motor_object_t 指针
  * @param data: CAN 原始数据
  */
static void _dji_motor_decode_callback(void *device, uint8_t *data)
{
    dji_motor_object_t *motor = (dji_motor_object_t *)device;
    dji_motor_measure_t *ptr = &(motor->measure);

    ptr->last_ecd = ptr->ecd;
    ptr->ecd = (uint16_t)((data[0] << 8) | data[1]);
    ptr->speed_rpm = (int16_t)((data[2] << 8) | data[3]);
    ptr->given_current = (int16_t)((data[4] << 8) | data[5]);
    ptr->temperate = data[6];
}

/**
  * @brief 初始化一个电机对象并注册到CAN驱动
  */
void DJI_Motor_Init(dji_motor_object_t *motor, CAN_HandleTypeDef *hcan, uint32_t rx_id)
{
    if (motor == NULL) return;

    motor->hcan = hcan;
    motor->rx_id = rx_id;

    // 清零数据
    motor->measure.ecd = 0;
    motor->measure.speed_rpm = 0;
    motor->measure.given_current = 0;
    motor->measure.temperate = 0;

    // 向 BSP 层注册：当收到 rx_id 时，调用 _dji_motor_decode_callback，并将 motor 指针传回去
    BSP_CAN_RegisterRxCallback(hcan, rx_id,CAN_ID_STD, _dji_motor_decode_callback, motor);
}

/**
  * @brief 发送底盘控制指令 (0x200 ID 对应 0x201-0x204)
  */
void DJI_Motor_SendGroup_0x200(CAN_HandleTypeDef *hcan, int16_t c1, int16_t c2, int16_t c3, int16_t c4)
{
    uint8_t data[8];

    data[0] = c1 >> 8;
    data[1] = c1;
    data[2] = c2 >> 8;
    data[3] = c2;
    data[4] = c3 >> 8;
    data[5] = c3;
    data[6] = c4 >> 8;
    data[7] = c4;

    BSP_CAN_SendMsg(hcan, 0x200,CAN_ID_STD, data, 8);
}

void DJI_Motor_SendGroup_0x1FF(CAN_HandleTypeDef *hcan, int16_t c1, int16_t c2, int16_t c3, int16_t c4) {
    uint8_t data[8];

    data[0] = c1 >> 8;
    data[1] = c1;
    data[2] = c2 >> 8;
    data[3] = c2;
    data[4] = c3 >> 8;
    data[5] = c3;
    data[6] = c4 >> 8;
    data[7] = c4;
    BSP_CAN_SendMsg(hcan, 0x1FF,CAN_ID_STD, data, 8);
}