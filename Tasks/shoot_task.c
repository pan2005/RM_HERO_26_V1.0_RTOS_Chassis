//
// Created by PanZhaoHui on 25-12-13.
//

#include "shoot_task.h"
#include "chassis_control_task.h"

#include "remote_control.h"

#include "CAN_receive.h"
#include "cmsis_os2.h"
#include "LKMF9025.h"


#include "pid.h"
#include "DJI_Motor.h"
#include "LKMF9025.h"

extern CAN_HandleTypeDef hcan1;

PID_t shoot_pid;
dji_motor_object_t shoot_motor;

void shoot_task() {
    DJI_Motor_Init(&shoot_motor,&hcan1,0x205);
    PID_Init(&shoot_pid,7,0.1,0.1,1000);
    int16_t I = 0;
    int16_t velocity = 0;
    while (1) {
        velocity = local_rc_ctrl->rc.ch[4] * 10;
        I  = PID_Caculate(&shoot_pid,velocity,shoot_motor.measure.speed_rpm);
        DJI_Motor_SendGroup_0x1FF(&hcan1,I,I,I,I);
        osDelay(2);
    }

}
