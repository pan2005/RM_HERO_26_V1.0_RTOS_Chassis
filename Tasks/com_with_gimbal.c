//
// Created by PanZhaoHui on 25-12-15.
//

#include "com_with_gimbal.h"

#include "remote_control.h"
#include "bsp_can.h"
#include "bsp_usart.h"
#include "cmsis_os2.h"
#include "robot_global.h"
#include "usart_protocol.h"

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart6;

uint8_t cmd_id;
uint8_t raw_data[64]; // 假设这是串口接收到的原始数据

// 准备一个足够大的联合体或直接传入对应的结构体地址
ChassisInfo_t received_chassis;
GimbalInfo_t gimbal_info;

static void On_Usart1_Data_Received(uint8_t *data, uint16_t len) {
    if (Protocol_Parse(data, len, &cmd_id, &received_chassis)) {
       HAL_GPIO_TogglePin(LED_R_GPIO_Port, LED_R_Pin);
       if (cmd_id == CMD_ID_CHASSIS_CTRL) {
           robot_ctrl.shoot_gear = received_chassis.shoot_gear;
       }
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
        gimbal_info.robot_id = robot_ctrl.referee.robot_status.robot_id;

        Protocol_Pack_GimbalInfo(&gimbal_info,tx_buf, &tx_len);
        usart_tx_binary(&huart1, tx_buf, tx_len);

        if (received_chassis.mode == 1) {
           // HAL_GPIO_WritePin(LED_G_GPIO_Port,LED_G_Pin,0);
        }

        osDelay(20);

    }

}