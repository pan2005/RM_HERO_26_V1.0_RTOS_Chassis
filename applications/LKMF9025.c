//
// Created by Admin on 2025/11/9.
//

#include "LKMF9025.h"

#include "can.h"
#include "cmsis_os2.h"
#include "remote_control.h"
#include "bsp_usart.h"

#include "daemon.h"
#include "bsp_buzzer.h"
CAN_RxHeaderTypeDef LK_rxMsg;//发送接收结构体
CAN_TxHeaderTypeDef LK_txMsg;//发送配置结构体
uint8_t LK_rx_data[8];       //接收数据


uint32_t LK_send_mail_box = {0};//NONE
LK_Motor_t YAW_Motor;

extern const RC_ctrl_t * local_rc_ctrl;

void LK_Motor_MultiLoopControl_1(const LK_Motor_t * LK_Motor, int32_t angle) {
    uint8_t LK_txdata[8] = {0};
    LK_txMsg.IDE = CAN_ID_STD;
    LK_txMsg.StdId = LK9025_Single_StdId + LK_Motor->id;
    LK_txMsg.RTR = CAN_RTR_DATA;
    LK_txMsg.DLC = 8;
    LK_txdata[0] = Multi_Loop_Control_Mode1;
    LK_txdata[4] = (angle >> 0) & 0xFF;   // 最低字节
    LK_txdata[5] = (angle >> 8) & 0xFF;   // 次低字节
    LK_txdata[6] = (angle >> 16) & 0xFF;  // 次高字节
    LK_txdata[7] = (angle >> 24) & 0xFF;  // 最高字节
    HAL_CAN_AddTxMessage(&hcan1,&LK_txMsg,LK_txdata,&LK_send_mail_box);
    HAL_Delay(1);
}
void LK_Motor_SpeedControl(const LK_Motor_t * LK_Motor, int32_t speed) {
    uint8_t LK_txdata[8] = {0};
    LK_txMsg.IDE = CAN_ID_STD;
    LK_txMsg.StdId = LK9025_Single_StdId + LK_Motor->id;
    LK_txMsg.ExtId = 0x00;
    LK_txMsg.RTR = CAN_RTR_DATA;
    LK_txMsg.DLC = 8;
    LK_txdata[0] = Speed_Control_Mode;
    LK_txdata[4] = (speed >> 0) & 0xFF;
    LK_txdata[5] = (speed >> 8) & 0xFF;
    LK_txdata[6] = (speed >> 16) & 0xFF;
    LK_txdata[7] = (speed >> 24) & 0xFF;

    HAL_CAN_AddTxMessage(&hcan1,&LK_txMsg,LK_txdata,&LK_send_mail_box);
    HAL_Delay(1);
}
static void LKMotorLostCallback(void * id) {
    LK_Motor_t * motor = (LK_Motor_t *)id;
 //   usart_printf("LKMotor Lost\r\n");
    buzzer_on(1,15000);


   DaemonReload(motor->daemon);
    //osDelay(100);

}
void LK_Motor_Init(LK_Motor_t * LK_Motor,uint8_t id){
    LK_Motor->id = id;
    // Daemon_Init_Config_s daemon_config = {
    //     .callback = LKMotorLostCallback,
    //     .owner_id = (void *)LK_Motor,
    //     .init_count = 500,
    //     .reload_count = 100, // 50ms
    // };
    // LK_Motor->daemon = DaemonRegister(&daemon_config);
}

/****************************************************************************************************************************/
/****************************TASK***************************/
void LK_Motor_testtask(void* argument) {
    LK_Motor_Init(&YAW_Motor,1);
    HAL_Delay(200);

    while (1) {
        LK_Motor_SpeedControl(&YAW_Motor,local_rc_ctrl->rc.ch[1] * 20);
        osDelay(100);
    }

}