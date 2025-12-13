//
// Created by Admin on 2025/12/4.
//

#include "chassis_control_task.h"

#include "remote_control.h"

#include "CAN_receive.h"
#include "cmsis_os2.h"
#include "LKMF9025.h"


#include "pid.h"
#include "DJI_Motor.h"

extern CAN_HandleTypeDef hcan1;

// 1. 定义电机对象实例
dji_motor_object_t chassis_m1;
dji_motor_object_t chassis_m2;
dji_motor_object_t chassis_m3;
dji_motor_object_t chassis_m4;
PID_t chassis_pid[4];


void chassis_control_task() {

    DJI_Motor_Init(&chassis_m1,&hcan1,0x201);
    DJI_Motor_Init(&chassis_m2,&hcan1,0x202);
    DJI_Motor_Init(&chassis_m3,&hcan1,0x203);
    DJI_Motor_Init(&chassis_m4,&hcan1,0x204);

    int16_t velocity_forword = 0;
    int16_t velocity_turn = 0;
    int16_t velocity_14 = 0;
    int16_t velocity_23 = 0;


    int16_t I1 = 0;
    int16_t I2 = 0;
    int16_t I3 = 0;
    int16_t I4 = 0;
   for (int i = 0; i < 4; i++) {
       PID_Init(&chassis_pid[i],5,0.0,0.0,1000);
   }

    while (1) {
        if (switch_is_down(local_rc_ctrl->rc.s[1])) {
            velocity_forword = local_rc_ctrl->rc.ch[3] * 5;
            velocity_turn = local_rc_ctrl->rc.ch[2] * 1;

            I1 = PID_Caculate(&chassis_pid[0],velocity_forword + velocity_turn,chassis_m1.measure.speed_rpm);
            I2 = PID_Caculate(&chassis_pid[1],-(velocity_forword - velocity_turn),chassis_m2.measure.speed_rpm);
            I3 = PID_Caculate(&chassis_pid[2],velocity_forword + velocity_turn,chassis_m3.measure.speed_rpm);
            I4 = PID_Caculate(&chassis_pid[3],-(velocity_forword - velocity_turn),chassis_m4.measure.speed_rpm);



            DJI_Motor_SendGroup(&hcan1,I1,I2,I3,I4);
            osDelay(1);
            //LK_Motor_SpeedControl(&YAW_Motor,local_rc_ctrl->rc.ch[0] * 20);

        }
        else if (switch_is_up(local_rc_ctrl->rc.s[1])) {

            velocity_14 = local_rc_ctrl->rc.ch[2] * 10 + local_rc_ctrl->rc.ch[3] * 10;
            velocity_23 = -local_rc_ctrl->rc.ch[2] * 10 + local_rc_ctrl->rc.ch[3] * 10;

            I1 = PID_Caculate(&chassis_pid[0],velocity_14,chassis_m1.measure.speed_rpm);
            I2 = PID_Caculate(&chassis_pid[1],-velocity_23,chassis_m2.measure.speed_rpm);
            I3 = PID_Caculate(&chassis_pid[2],velocity_23,chassis_m3.measure.speed_rpm);
            I4 = PID_Caculate(&chassis_pid[3],-velocity_14,chassis_m4.measure.speed_rpm);



            DJI_Motor_SendGroup(&hcan1,I1,I2,I3,I4);

            osDelay(1);
           // LK_Motor_SpeedControl(&YAW_Motor,local_rc_ctrl->rc.ch[0] * 20);




        }
        else {
            velocity_turn = local_rc_ctrl->rc.ch[0] * 12;
            velocity_14 = local_rc_ctrl->rc.ch[2] * 10 + local_rc_ctrl->rc.ch[3] * 10;
            velocity_23 = -local_rc_ctrl->rc.ch[2] * 10 + local_rc_ctrl->rc.ch[3] * 10;

            I1 = PID_Caculate(&chassis_pid[0],velocity_14 + velocity_turn,chassis_m1.measure.speed_rpm);
            I2 = PID_Caculate(&chassis_pid[1],-(velocity_23 - velocity_turn),chassis_m2.measure.speed_rpm);
            I3 = PID_Caculate(&chassis_pid[2],velocity_23 + velocity_turn,chassis_m3.measure.speed_rpm);
            I4 = PID_Caculate(&chassis_pid[3],-(velocity_14 - velocity_turn),chassis_m4.measure.speed_rpm);

            DJI_Motor_SendGroup(&hcan1,I1,I2,I3,I4);

            osDelay(1);
           // LK_Motor_SpeedControl(&YAW_Motor,local_rc_ctrl->rc.ch[0] * 20);

        }
        osDelay(2);

    }

}

void M3508_test_task() {


}