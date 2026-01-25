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
#include "com_with_gimbal.h"
#include "robot_global.h"
#include <math.h>

extern CAN_HandleTypeDef hcan1;

// 1. 定义电机对象实例
// dji_motor_object_t chassis_m1;
// dji_motor_object_t chassis_m2;
// dji_motor_object_t chassis_m3;
// dji_motor_object_t chassis_m4;


dji_motor_object_t shoot_motor;

PID_t Follow;


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

//    const char test2[20] = "initing\r\n";


  //  HAL_UART_Transmit(&huart6, test2, 20,100);

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

    yaw_priv.whether_extern_data = 1;

    PID_Init(&yaw_priv.speed_pid, 40.0f, 0.0f, 5.0f, 500, 25000);
    PID_Init(&yaw_priv.pos_pid, 700.0f, 0.0f, 100.0f, 500, 5000);
    Can_Motor_Init(&yaw_motor, &hcan1, 0x206, GM6020_Decode, GM6020_Update, &yaw_priv);


    PID_Init(&Follow, 2000.0f, 0.0f, 10.0f, 500, 5000);




    float velocity_forword = 0;
    float velocity_turn = 0;
    float velocity_14 = 0;
    float velocity_23 = 0;
    float angle = 0;






    osDelay(100);

    while (1) {
        robot_ctrl.chassis_mode = received_chassis.enable;


        yaw_priv.INS_angle = received_chassis.yaw_INS;

        float theta = yaw_priv.total_angle + 1.5707963f; // 1.5707963f = PI / 2
        theta = Radian_Normalize(theta);

        if (robot_ctrl.chassis_mode == CHASSIS_FOLLOW) {
            velocity_turn = PID_Calculate(&Follow, 0, theta);

        }
        else if (robot_ctrl.chassis_mode == CHASSIS_ENABLE){
            velocity_turn = received_chassis.vz  * 10 ;

        }
        else {
            velocity_turn = 0;
        }

        // 2. 坐标系旋转矩阵变换
        // 将操作手的 vx(左右), vy(前后) 映射到底盘坐标系的 target_vx, target_vy



        // 旋转矩阵：
        // target_vx = vx * cos(theta) - vy * sin(theta)
        // target_vy = vx * sin(theta) + vy * cos(theta)
        float target_vx = received_chassis.vy * cosf(theta) - received_chassis.vx * sinf(theta);
        float target_vy = received_chassis.vy * sinf(theta) + received_chassis.vx * cosf(theta);

        velocity_14 = target_vy * 10.0f + target_vx * 10.0f;
        velocity_23 = target_vy * 10.0f - target_vx * 10.0f;

            wheel_priv_1.target_speed = velocity_14 + velocity_turn;
            wheel_priv_2.target_speed = -(velocity_23 - velocity_turn);  //因为安装位置的问题
            wheel_priv_3.target_speed = velocity_23 + velocity_turn;
            wheel_priv_4.target_speed = -(velocity_14 - velocity_turn);


            yaw_priv.target_angle = received_chassis.yaw;





            if (received_chassis.fire == 1) {
                thumbwheel_priv.target_speed = 3000;
            }


        if (robot_ctrl.chassis_mode == CHASSIS_UNABLE) {
            wheel_motor_1.output_value = 0;
            wheel_motor_2.output_value = 0;
            wheel_motor_4.output_value = 0;
            wheel_motor_3.output_value = 0;
            yaw_motor.output_value = 0;
            thumbwheel_motor.output_value = 0;
            //yaw_priv.target_angle = yaw_priv.total_angle;
            gimbal_info.yaw_encoder = yaw_priv.total_angle;
        }
        else {
            wheel_motor_1.update_func(&wheel_motor_1);
            wheel_motor_2.update_func(&wheel_motor_2);
            wheel_motor_3.update_func(&wheel_motor_3);
            wheel_motor_4.update_func(&wheel_motor_4);
            yaw_motor.update_func(&yaw_motor);
            thumbwheel_motor.update_func(&thumbwheel_motor);

        }


            DJI_Motor_SendGroup_0x200(&hcan1,wheel_motor_1.output_value,wheel_motor_2.output_value,wheel_motor_3.output_value,
            wheel_motor_4.output_value);
            DJI_Motor_SendGroup_0x1FF(&hcan1,thumbwheel_motor.output_value,yaw_motor.output_value,0,0);
            osDelay(2);
            //  sprintf(buffer,"yaw:%f",yaw_priv.total_angle);
            // HAL_UART_Transmit(&huart6, buffer, 20,10);

    }

}
