//
// Created by Admin on 2025/12/4.
//

#include "chassis_control_task.h"

#include <stdio.h>

#include "remote_control.h"

#include "CAN_receive.h"
#include "cmsis_os2.h"
#include "LKMF9025.h"


#include "pid.h"
#include "DJI_Motor.h"
#include "LKMF9025.h"
#include "usart.h"

extern CAN_HandleTypeDef hcan1;

// 1. 定义电机对象实例
dji_motor_object_t chassis_m1;
dji_motor_object_t chassis_m2;
dji_motor_object_t chassis_m3;
dji_motor_object_t chassis_m4;
dji_motor_object_t YAW_Motor6020;
PID_t shoot_pid;

dji_motor_object_t shoot_motor;
PID_t chassis_pid[4];
PID_t YAW_pid;


void chassis_control_task() {

    char buffer[20];

    DJI_Motor_Init(&chassis_m1,&hcan1,0x201);
    DJI_Motor_Init(&chassis_m2,&hcan1,0x202);
    DJI_Motor_Init(&chassis_m3,&hcan1,0x203);
    DJI_Motor_Init(&chassis_m4,&hcan1,0x204);
    DJI_Motor_Init(&shoot_motor,&hcan1,0x205);
    PID_Init(&shoot_pid,7,0.1,0.1,1000,8000);
   // GM6020_PV_init(&YAW_Motor6020, 0x206, &hcan1, 6,
       //             800.0,     /* P_Kp */
      //              0.0,      /* P_Ki */
       //             50.0,    /* V_Kp */
       //             0.0,      /* V_Ki */
       //             0.0,      /* V_Kd */
       //             25000.0 );  /* Out_Max */


    //LK_Motor_Init(&YAW_Motor,&hcan1,1);

    float velocity_forword = 0;
    float velocity_turn = 0;
    float velocity_14 = 0;
    float velocity_23 = 0;
    float angle = 0;


    int16_t I1 = 0;
    int16_t I2 = 0;
    int16_t I3 = 0;
    int16_t I4 = 0;
   for (int i = 0; i < 4; i++) {
       PID_Init(&chassis_pid[i],7,1.0,0.0,1000,16000);
   }
   PID_Init(&YAW_pid,10,1,0.1,1000,25000);


    osDelay(100);

    while (1) {

        if (switch_is_mid(local_rc_ctrl->rc.s[1])) {
            velocity_turn = local_rc_ctrl->rc.ch[1] * 12;
            velocity_14 = local_rc_ctrl->rc.ch[2] * 10 + local_rc_ctrl->rc.ch[3] * 10;
            velocity_23 = -local_rc_ctrl->rc.ch[2] * 10 + local_rc_ctrl->rc.ch[3] * 10;

            chassis_m1.input_current = (int16_t)PID_Caculate(&chassis_pid[0],velocity_14 + velocity_turn,chassis_m1.measure.speed_rpm);
            chassis_m2.input_current = (int16_t)PID_Caculate(&chassis_pid[1],-(velocity_23 - velocity_turn),chassis_m2.measure.speed_rpm);
            chassis_m3.input_current = (int16_t)PID_Caculate(&chassis_pid[2],velocity_23 + velocity_turn,chassis_m3.measure.speed_rpm);
            chassis_m4.input_current = (int16_t)PID_Caculate(&chassis_pid[3],-(velocity_14 - velocity_turn),chassis_m4.measure.speed_rpm);


            YAW_Motor6020.input_current = (int16_t)PID_Caculate(&YAW_pid,local_rc_ctrl->rc.ch[0] * 10,YAW_Motor6020.measure.speed_rpm);
           // angle += local_rc_ctrl->rc.ch[0] / 6600.0f /5.0f;
           // GM6020_PV_set_target(&YAW_Motor6020,angle);
            //GM6020_PV_update(&YAW_Motor6020);


            //LK_Motor_SpeedControl(&YAW_Motor,local_rc_ctrl->rc.ch[1] * 20);
            // 替代原来的两行代码
           // LK_Motor_SpeedControl(&YAW_Motor, local_rc_ctrl->rc.ch[0] * 20); // 根据需要调整倍数


           DJI_Motor_SendGroup_0x200(&hcan1,chassis_m1.input_current,I2,chassis_m3.input_current,
            chassis_m4.input_current);
           // osDelay(2);
           //DJI_Motor_SendGroup_0x1FF(&hcan1,shoot_motor.input_current,YAW_Motor6020.input_current,0,0);
            osDelay(2);


        }
        else if (switch_is_down(local_rc_ctrl->rc.s[1])) {

            velocity_14 = local_rc_ctrl->rc.ch[2] * 10 + local_rc_ctrl->rc.ch[3] * 10;
            velocity_23 = -local_rc_ctrl->rc.ch[2] * 10 + local_rc_ctrl->rc.ch[3] * 10;

            chassis_m1.input_current = (int16_t)PID_Caculate(&chassis_pid[0],velocity_14 + velocity_turn,chassis_m1.measure.speed_rpm);
            chassis_m2.input_current = (int16_t)PID_Caculate(&chassis_pid[1],-(velocity_23 - velocity_turn),chassis_m2.measure.speed_rpm);
            chassis_m3.input_current = (int16_t)PID_Caculate(&chassis_pid[2],velocity_23 + velocity_turn,chassis_m3.measure.speed_rpm);
            chassis_m4.input_current = (int16_t)PID_Caculate(&chassis_pid[3],-(velocity_14 - velocity_turn),chassis_m4.measure.speed_rpm);

           // angle += local_rc_ctrl->rc.ch[0] / 6600.0f /5.0f;
           // GM6020_PV_set_target(&YAW_Motor6020,angle);
            //GM6020_PV_update(&YAW_Motor6020);

            //LK_Motor_SpeedControl(&YAW_Motor,local_rc_ctrl->rc.ch[1] * 20);
            // 替代原来的两行代码
            // LK_Motor_SpeedControl(&YAW_Motor, local_rc_ctrl->rc.ch[0] * 20); // 根据需要调整倍数

         YAW_Motor6020.input_current = (int16_t)PID_Caculate(&YAW_pid,local_rc_ctrl->rc.ch[0] * 10,YAW_Motor6020.measure.speed_rpm);
           DJI_Motor_SendGroup_0x200(&hcan1,chassis_m1.input_current,chassis_m2.input_current,chassis_m3.input_current,
            chassis_m4.input_current);
           // osDelay(2);
           // DJI_Motor_SendGroup_0x1FF(&hcan1,shoot_motor.input_current,YAW_Motor6020.input_current,0,0);
            osDelay(2);

        }

        shoot_motor.target_velocity = local_rc_ctrl->rc.ch[4] * 10;
        shoot_motor.input_current  = (int16_t)PID_Caculate(&shoot_pid,shoot_motor.target_velocity,shoot_motor.measure.speed_rpm);
        DJI_Motor_SendGroup_0x1FF(&hcan1,shoot_motor.input_current,YAW_Motor6020.input_current,0,0);
        osDelay(2);
      //  sprintf(buffer,"%d,%d,%d\r\n",YAW_Motor6020.measure.speed_rpm,I1,local_rc_ctrl->rc.ch[0]);
       // HAL_UART_Transmit(&huart6,(uint8_t *)buffer,20,100);





           // LK_Motor_SpeedControl(&YAW_Motor,local_rc_ctrl->rc.ch[0] * 20);


        osDelay(2);

    }

}

void M3508_test_task() {


}