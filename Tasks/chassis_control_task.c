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
#include "can_motor.h"

extern CAN_HandleTypeDef hcan1;

// 1. 定义电机对象实例
// dji_motor_object_t chassis_m1;
// dji_motor_object_t chassis_m2;
// dji_motor_object_t chassis_m3;
// dji_motor_object_t chassis_m4;


dji_motor_object_t shoot_motor;
PID_t chassis_pid[4];
PID_t YAW_pid;



Can_Motor_t wheel_motor_1;
M3508_Data_t wheel_priv_1;

Can_Motor_t wheel_motor_2;
M3508_Data_t wheel_priv_2;

Can_Motor_t wheel_motor_3;
M3508_Data_t wheel_priv_3;

Can_Motor_t wheel_motor_4;
M3508_Data_t wheel_priv_4;

Can_Motor_t wheel_motor_4;
M3508_Data_t wheel_priv_4;

Can_Motor_t thumbwheel_motor;
M3508_Data_t thumbwheel_priv;

Can_Motor_t yaw_motor;
GM6020_Data_t yaw_priv;
void chassis_control_task() {

    char buffer[20];

    osDelay(1000);

    const char test2[20] = "initing\r\n";


    HAL_UART_Transmit(&huart6, test2, 20,100);

    PID_Init(&wheel_priv_1.speed_pid, 7.0f, 0.1f, 0.0f, 500, 16384);
    Can_Motor_Init(&wheel_motor_1, &hcan1, 0x201, M3508_Decode, M3508_Update, &wheel_priv_1);

    PID_Init(&wheel_priv_2.speed_pid, 7.0f, 0.1f, 0.0f, 500, 16384);
    Can_Motor_Init(&wheel_motor_2, &hcan1, 0x202, M3508_Decode, M3508_Update, &wheel_priv_2);

    PID_Init(&wheel_priv_3.speed_pid, 7.0f, 0.1f, 0.0f, 500, 16384);
    Can_Motor_Init(&wheel_motor_3, &hcan1, 0x203, M3508_Decode, M3508_Update, &wheel_priv_3);

    PID_Init(&wheel_priv_4.speed_pid, 7.0f, 0.1f, 0.0f, 500, 16384);
    Can_Motor_Init(&wheel_motor_4, &hcan1, 0x204, M3508_Decode, M3508_Update, &wheel_priv_4);

    PID_Init(&thumbwheel_priv.speed_pid, 7.0f, 0.1f, 0.0f, 500, 16384);
    Can_Motor_Init(&thumbwheel_motor, &hcan1, 0x205, M3508_Decode, M3508_Update, &thumbwheel_priv);  //

    PID_Init(&yaw_priv.speed_pid, 50.0f, 0.0f, 0.0f, 500, 25000);
    PID_Init(&yaw_priv.pos_pid, 800.0f, 0.0f, 0.0f, 500, 5000);
    Can_Motor_Init(&yaw_motor, &hcan1, 0x206, GM6020_Decode, GM6020_Update, &yaw_priv);

    const char test1[20] = "init succefully";
    HAL_UART_Transmit(&huart6, test1, 20,100);


    // DJI_Motor_Init(&chassis_m1,&hcan1,0x201);
    // DJI_Motor_Init(&chassis_m2,&hcan1,0x202);
    // DJI_Motor_Init(&chassis_m3,&hcan1,0x203);
    // DJI_Motor_Init(&chassis_m4,&hcan1,0x204);
    // DJI_Motor_Init(&shoot_motor,&hcan1,0x205);
    // PID_Init(&shoot_pid,7,0.1,0.1,1000,8000);
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



    osDelay(100);

    while (1) {

        if (switch_is_mid(local_rc_ctrl->rc.s[1])) {
            velocity_turn = local_rc_ctrl->rc.ch[1] * 12;
            velocity_14 = local_rc_ctrl->rc.ch[2] * 10 + local_rc_ctrl->rc.ch[3] * 10;
            velocity_23 = -local_rc_ctrl->rc.ch[2] * 10 + local_rc_ctrl->rc.ch[3] * 10;

            //  = (int16_t)PID_Calculate(&chassis_pid[0],velocity_14 + velocity_turn,chassis_m1.measure.speed_rpm);
            // chassis_m2.input_current = (int16_t)PID_Calculate(&chassis_pid[1],-(velocity_23 - velocity_turn),chassis_m2.measure.speed_rpm);
            // chassis_m3.input_current = (int16_t)PID_Calculate(&chassis_pid[2],velocity_23 + velocity_turn,chassis_m3.measure.speed_rpm);
            // chassis_m4.input_current = (int16_t)PID_Calculate(&chassis_pid[3],-(velocity_14 - velocity_turn),chassis_m4.measure.speed_rpm);
            //
            //
            wheel_priv_1.target_speed = velocity_14 + velocity_turn;
            wheel_priv_2.target_speed = -(velocity_23 - velocity_turn);
            wheel_priv_3.target_speed = velocity_23 + velocity_turn;
            wheel_priv_4.target_speed = -(velocity_14 - velocity_turn);

            wheel_motor_1.update_func(&wheel_motor_1);

            wheel_motor_2.update_func(&wheel_motor_2);
            wheel_motor_3.update_func(&wheel_motor_3);
            wheel_motor_4.update_func(&wheel_motor_4);
            //GM6020_Update(&yaw_motor);
            yaw_motor.update_func(&yaw_motor);
            DJI_Motor_SendGroup_0x200(&hcan1,wheel_motor_1.output_value,wheel_motor_2.output_value,wheel_motor_3.output_value,
            wheel_motor_4.output_value);

            angle += local_rc_ctrl->rc.ch[0] / 6600.0f /5.0f;
            yaw_priv.target_angle = angle;

            //HAL_GPIO_TogglePin(LED_B_GPIO_Port,LED_B_Pin);

           // HAL_UART_Transmit(&huart6,"yes\r\n",5,100);


           // osDelay(2);
           //DJI_Motor_SendGroup_0x1FF(&hcan1,shoot_motor.input_current,YAW_Motor6020.input_current,0,0);
            osDelay(2);


        }
        else if (switch_is_down(local_rc_ctrl->rc.s[1])) {

            velocity_14 = local_rc_ctrl->rc.ch[2] * 10 + local_rc_ctrl->rc.ch[3] * 10;
            velocity_23 = -local_rc_ctrl->rc.ch[2] * 10 + local_rc_ctrl->rc.ch[3] * 10;

            // chassis_m1.input_current = (int16_t)PID_Calculate(&chassis_pid[0],velocity_14,chassis_m1.measure.speed_rpm);
            // chassis_m2.input_current = (int16_t)PID_Calculate(&chassis_pid[1],-(velocity_23),chassis_m2.measure.speed_rpm);
            // chassis_m3.input_current = (int16_t)PID_Calculate(&chassis_pid[2],velocity_23,chassis_m3.measure.speed_rpm);
            // chassis_m4.input_current = (int16_t)PID_Calculate(&chassis_pid[3],-(velocity_14),chassis_m4.measure.speed_rpm);

            wheel_priv_1.target_speed = velocity_14;
            wheel_priv_2.target_speed = -(velocity_23);
            wheel_priv_3.target_speed = velocity_23;
            wheel_priv_4.target_speed = -(velocity_14);

            angle += local_rc_ctrl->rc.ch[0] / 6600.0f /5.0f;
            yaw_priv.target_angle = angle;
           // GM6020_PV_set_target(&YAW_Motor6020,angle);
            //GM6020_PV_update(&YAW_Motor6020);

            //LK_Motor_SpeedControl(&YAW_Motor,local_rc_ctrl->rc.ch[1] * 20);
            // 替代原来的两行代码
            // LK_Motor_SpeedControl(&YAW_Motor, local_rc_ctrl->rc.ch[0] * 20); // 根据需要调整倍数

            wheel_motor_1.update_func(&wheel_motor_1);

            wheel_motor_2.update_func(&wheel_motor_2);
            wheel_motor_3.update_func(&wheel_motor_3);
            wheel_motor_4.update_func(&wheel_motor_4);
            yaw_motor.update_func(&yaw_motor);
            DJI_Motor_SendGroup_0x200(&hcan1,wheel_motor_1.output_value,wheel_motor_2.output_value,wheel_motor_3.output_value,
            wheel_motor_4.output_value);

            osDelay(2);

        }
        thumbwheel_priv.target_speed = local_rc_ctrl->rc.ch[4] * 10;


       // shoot_motor.input_current  = (int16_t)PID_Calculate(&shoot_pid,shoot_motor.target_velocity,shoot_motor.measure.speed_rpm);

        thumbwheel_motor.update_func(&thumbwheel_motor);
        DJI_Motor_SendGroup_0x1FF(&hcan1,thumbwheel_motor.output_value,yaw_motor.output_value,0,0);
        osDelay(2);



       // osDelay(2);

    }

}

void M3508_test_task() {


}