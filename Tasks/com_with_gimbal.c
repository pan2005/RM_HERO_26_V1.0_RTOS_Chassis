//
// Created by PanZhaoHui on 25-12-15.
//

#include "com_with_gimbal.h"
#include "usart_protocol.h"
#include "remote_control.h"
#include "bsp_can.h"
#include "bsp_usart.h"

extern UART_HandleTypeDef huart1;


GimbalInfo_t gimbal_info;

static void On_Usart1_Data_Received(uint8_t *data, uint16_t len) {
    HAL_UART_Transmit(&huart1, data, len, 100);
}

void com_with_gimbal_init(void) {
    usart1_register_callback(On_Usart1_Data_Received);
    usart1_init();
}
void communication_with_gimbal_task(void * argument) {
    float position = 0;
    static uint8_t tx_buf[64];
    static uint16_t tx_len;
    com_with_gimbal_init();
    while (1) {
        position += local_rc_ctrl->rc.ch[1] / 660.0;
        Protocol_Pack_GimbalInfo(&gimbal_info, tx_buf, &tx_len)
        usart_tx_binary(&huart1, tx_buf, tx_len);

    }

}