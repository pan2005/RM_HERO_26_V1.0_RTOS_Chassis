#include "can_motor.h"
#include "robot_global.h"
#include "math.h"
#include <string.h>

float Radian_Normalize(float angle) {
    float res = fmodf(angle, TWO_PI);
    if (res > PI) res -= TWO_PI;
    if (res < -PI) res += TWO_PI;
    return res;
}
void Can_Motor_Init(Can_Motor_t *motor, CAN_HandleTypeDef *hcan, uint32_t rx_id,
                    Motor_Decode_Func decode, Motor_Update_Func update, void *priv) {
    if (!motor || !priv) return;

    motor->hcan = hcan;
    motor->rx_id = rx_id;
    motor->decode_func = decode;
    motor->update_func = update;
    motor->priv_data = priv;

    // 假设你的 BSP 层支持这种注册方式
    BSP_CAN_RegisterRxCallback(hcan, rx_id, CAN_ID_STD, decode, motor);
}

// ---------------- M3508 逻辑 ----------------
void M3508_Decode(void *device, uint8_t *data) {
    Can_Motor_t *self = (Can_Motor_t *)device;
    self->measure.last_ecd = self->measure.ecd;
    self->measure.ecd = (uint16_t)(data[0] << 8 | data[1]);
    self->measure.speed_rpm = (int16_t)(data[2] << 8 | data[3]);
    self->measure.given_current = (int16_t)((data[4] << 8) | data[5]);
    self->measure.temperate = data[6];
}

void M3508_Update(Can_Motor_t *self) {
    M3508_Data_t *d = (M3508_Data_t *)self->priv_data;
    self->output_value = (int16_t)PID_Calculate(&d->speed_pid, d->target_speed, (float)self->measure.speed_rpm);
}

// ---------------- GM6020 逻辑 ----------------
void GM6020_Decode(void * device, uint8_t *data) {
    Can_Motor_t *self = (Can_Motor_t *)device;
    self->measure.last_ecd = self->measure.ecd;
    self->measure.ecd = (uint16_t)(data[0] << 8 | data[1]);
    self->measure.speed_rpm = (int16_t)(data[2] << 8 | data[3]);

    GM6020_Data_t *d = (GM6020_Data_t *)self->priv_data;
    // 过零检测
    int16_t diff = self->measure.ecd - self->measure.last_ecd;
    if (diff < -4096)      d->round_count++;
    else if (diff > 4096)  d->round_count--;

    d->total_angle = (d->round_count * 8192 + self->measure.ecd) * (2.0f * 3.1415926f / 8192.0f);
}

void GM6020_Update(Can_Motor_t *self) {
    GM6020_Data_t *d = (GM6020_Data_t *)self->priv_data;

    if (d->whether_extern_data == 1) {

        float error = d->target_angle - d->INS_angle;
        float short_error = Radian_Normalize(error);

        // 2. 构造虚拟目标值，骗过 PID 函数
        float virtual_target = d->INS_angle + short_error;
        float v_target = PID_Calculate(&d->pos_pid, virtual_target, d->INS_angle);
        self->output_value = (int16_t)PID_Calculate(&d->speed_pid, v_target, (float)self->measure.speed_rpm);

    }
    else {
        float v_target = PID_Calculate(&d->pos_pid, d->target_angle, d->total_angle);
        self->output_value = (int16_t)PID_Calculate(&d->speed_pid, v_target, (float)self->measure.speed_rpm);

    }

    // float error = d->target_angle - d->total_angle;
    // float short_error = Radian_Normalize(error);
    //
    // // 2. 构造虚拟目标值，骗过 PID 函数
    // float virtual_target = d->total_angle + short_error;

}

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

void DJI_Motor_SendGroup_0x2FF(CAN_HandleTypeDef *hcan, int16_t c1, int16_t c2, int16_t c3) {
    uint8_t data[8];

    data[0] = c1 >> 8;
    data[1] = c1;
    data[2] = c2 >> 8;
    data[3] = c2;
    data[4] = c3 >> 8;
    data[5] = c3;
    data[6] = 0;
    data[7] = 0;
    BSP_CAN_SendMsg(hcan, 0x2FF,CAN_ID_STD, data, 8);
}