//
// Created by Admin on 2025/12/4.
//

#include "chassis_control_task.h"

#include "remote_control.h"

#include "CAN_receive.h"
#include "cmsis_os2.h"
#include "LKMF9025.h"


#include "pid.h"
PID_t chassis_pid[4];


void chassis_control_task() {
    int16_t velocity_forword = 0;
    int16_t velocity_turn = 0;
    int16_t velocity_14 = 0;
    int16_t velocity_23 = 0;


    int16_t I1 = 0;
    int16_t I2 = 0;
    int16_t I3 = 0;
    int16_t I4 = 0;
   for (int i = 0; i < 4; i++) {
       PID_Init(&chassis_pid[i],7,0.1,0.0,1000);
   }

    while (1) {
        if (switch_is_down(local_rc_ctrl->rc.s[1])) {
            velocity_forword = local_rc_ctrl->rc.ch[3] * 5;
            velocity_turn = local_rc_ctrl->rc.ch[2] * 1;

            I1 = PID_Caculate(&chassis_pid[0],velocity_forword + velocity_turn,motor_chassis[0].speed_rpm);
            I2 = PID_Caculate(&chassis_pid[1],-(velocity_forword - velocity_turn),motor_chassis[1].speed_rpm);
            I3 = PID_Caculate(&chassis_pid[2],velocity_forword + velocity_turn,motor_chassis[2].speed_rpm);
            I4 = PID_Caculate(&chassis_pid[3],-(velocity_forword - velocity_turn),motor_chassis[3].speed_rpm);



            CAN_cmd_chassis(I1,I2,I3,I4);
            osDelay(1);
            //LK_Motor_SpeedControl(&YAW_Motor,local_rc_ctrl->rc.ch[0] * 20);

        }
        else if (switch_is_up(local_rc_ctrl->rc.s[1])) {

            velocity_14 = local_rc_ctrl->rc.ch[2] * 10 + local_rc_ctrl->rc.ch[3] * 10;
            velocity_23 = -local_rc_ctrl->rc.ch[2] * 10 + local_rc_ctrl->rc.ch[3] * 10;

            I1 = PID_Caculate(&chassis_pid[0],velocity_14,motor_chassis[0].speed_rpm);
            I2 = PID_Caculate(&chassis_pid[1],-velocity_23,motor_chassis[1].speed_rpm);
            I3 = PID_Caculate(&chassis_pid[2],velocity_23,motor_chassis[2].speed_rpm);
            I4 = PID_Caculate(&chassis_pid[3],-velocity_14,motor_chassis[3].speed_rpm);



            CAN_cmd_chassis(I1,I2,I3,I4);

            osDelay(1);
           // LK_Motor_SpeedControl(&YAW_Motor,local_rc_ctrl->rc.ch[0] * 20);




        }
        else {
            velocity_turn = local_rc_ctrl->rc.ch[0] * 12;
            velocity_14 = local_rc_ctrl->rc.ch[2] * 10 + local_rc_ctrl->rc.ch[3] * 10;
            velocity_23 = -local_rc_ctrl->rc.ch[2] * 10 + local_rc_ctrl->rc.ch[3] * 10;

            I1 = PID_Caculate(&chassis_pid[0],velocity_14 + velocity_turn,motor_chassis[0].speed_rpm);
            I2 = PID_Caculate(&chassis_pid[1],-(velocity_23 - velocity_turn),motor_chassis[1].speed_rpm);
            I3 = PID_Caculate(&chassis_pid[2],velocity_23 + velocity_turn,motor_chassis[2].speed_rpm);
            I4 = PID_Caculate(&chassis_pid[3],-(velocity_14 - velocity_turn),motor_chassis[3].speed_rpm);

            CAN_cmd_chassis(I1,I2,I3,I4);

            osDelay(1);
           // LK_Motor_SpeedControl(&YAW_Motor,local_rc_ctrl->rc.ch[0] * 20);

        }
        osDelay(2);

    }

}

void M3508_test_task() {


}