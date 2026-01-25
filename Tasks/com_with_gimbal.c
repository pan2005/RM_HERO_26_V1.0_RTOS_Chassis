//
// Created by PanZhaoHui on 25-12-15.
//

#include "com_with_gimbal.h"

#include "remote_control.h"
#include "bsp_can.h"
#include "bsp_usart.h"
#include "cmsis_os2.h"
#include "robot_global.h"

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart6;

uint8_t cmd_id;
uint8_t raw_data[64]; // 假设这是串口接收到的原始数据

// 准备一个足够大的联合体或直接传入对应的结构体地址
ChassisInfo_t received_chassis;
GimbalInfo_t gimbal_info;

static void On_Usart1_Data_Received(uint8_t *data, uint16_t len) {
    //HAL_UART_Transmit(&huart1, data, len, 10);
    if (Protocol_Parse(data, len,&cmd_id, &received_chassis)) {
        //HAL_GPIO_TogglePin(LED_B_GPIO_Port,LED_B_Pin);
    }
}

void com_with_gimbal_init(void) {
    usart1_register_callback(On_Usart1_Data_Received);
    usart1_init();
}

void communication_with_gimbal_task(void * argument) {
    char buffer[20];
    static uint8_t tx_buf[64];
    static uint16_t tx_len;

    com_with_gimbal_init();
    HAL_Delay(50);
    while (1) {

        Protocol_Pack_GimbalInfo(&gimbal_info,tx_buf, &tx_len);
        usart_tx_binary(&huart1, tx_buf, tx_len);


        // sprintf(buffer,"YAW:%.3f,%.3f\r\n",received_chassis.yaw_INS,received_chassis.yaw_v_INS);
        // HAL_UART_Transmit(&huart6, buffer, 20,10);



        //robot_ctrl.chassis_current.yaw = received_chassis.yaw_current;

        // if (switch_is_down(local_rc_ctrl->rc.s[1])) {
        //     position = local_rc_ctrl->rc.ch[1] / 6600.0/2.0f;
        //
        // }
        // else {
        //     position = 0;
        // }
        //
        //  my_info.pitch_position = position;
        //  if (switch_is_up(local_rc_ctrl->rc.s[0])) {
        //      my_info.shoot_gear = 2;
        //  }
        // else if (switch_is_mid(local_rc_ctrl->rc.s[0])) {
        //     my_info.shoot_gear = 1;
        // }
        // else if (switch_is_down(local_rc_ctrl->rc.s[0])){
        //     my_info.shoot_gear = 0;
        //
        // }
        //  Protocol_Pack_GimbalInfo(&my_info, tx_buf, &tx_len);
        //  usart_tx_binary(&huart1, tx_buf, tx_len);

        osDelay(20);

    }

}