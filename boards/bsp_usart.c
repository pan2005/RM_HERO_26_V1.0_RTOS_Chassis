//
// Created by Admin on 2025/11/4.
//

#include "bsp_usart.h"
#include "main.h"
#include <stdarg.h>
#include <stdio.h>

#include "cmsis_os2.h"
#include "cybergear.h"
#include "LKMF9025.h"
#include "remote_control.h"
#include "Supercapacitor.h"
extern UART_HandleTypeDef huart1;
extern DMA_HandleTypeDef hdma_usart1_tx;
extern UART_HandleTypeDef huart6;
extern DMA_HandleTypeDef hdma_usart6_rx;
extern DMA_HandleTypeDef hdma_usart6_tx;
extern const RC_ctrl_t* local_rc_ctrl;

void usart_tx_dma_init(UART_HandleTypeDef* huart)
{

    //enable the DMA transfer for the receiver and tramsmit request
    //Ê¹ÄÜDMA´®¿Ú½ÓÊÕºÍ·¢ËÍ
    if (huart == NULL || huart->hdmatx == NULL || huart->Instance == NULL) {
        return; // 或者返回错误代码
    }
    SET_BIT(huart->Instance->CR3, USART_CR3_DMAR);
    SET_BIT(huart->Instance->CR3, USART_CR3_DMAT);

    //disable DMA
    //Ê§Ð§DMA
    __HAL_DMA_DISABLE(huart->hdmatx);

    while(huart->hdmatx->Instance->CR & DMA_SxCR_EN)
    {
        __HAL_DMA_DISABLE(huart->hdmatx);
    }

    huart->hdmatx->Instance->PAR = (uint32_t)&(huart->Instance->DR);
    huart->hdmatx->Instance->M0AR = (uint32_t)(NULL);
    huart->hdmatx->Instance->NDTR = 0;


}
void usart_tx_dma_enable(UART_HandleTypeDef* huart,uint8_t *data, uint16_t len)
{
    //disable DMA
    //Ê§Ð§DMA
    __HAL_DMA_DISABLE(huart->hdmatx);

    while(huart->hdmatx->Instance->CR & DMA_SxCR_EN)
    {
        __HAL_DMA_DISABLE(huart->hdmatx);
    }

    __HAL_DMA_CLEAR_FLAG(huart->hdmatx, DMA_HISR_TCIF7);

    huart->hdmatx->Instance->M0AR = (uint32_t)(data);
    __HAL_DMA_SET_COUNTER(huart->hdmatx, len);

    __HAL_DMA_ENABLE(huart->hdmatx);
}




void usart_printf(UART_HandleTypeDef* huart,const char *fmt, ...) {
    static uint8_t tx_buffer[256];
    static va_list ap;
    static uint16_t len;

    va_start(ap,fmt);

    len = vsprintf((char *)tx_buffer,fmt,ap);

    va_end(ap);

    usart_tx_dma_enable(huart,tx_buffer,len);
}


void usart6_init(uint8_t *rx1_buf, uint8_t *rx2_buf, uint16_t dma_buf_num)
{

    //enable the DMA transfer for the receiver and tramsmit request
    //Ê¹ÄÜDMA´®¿Ú½ÓÊÕºÍ·¢ËÍ
    SET_BIT(huart6.Instance->CR3, USART_CR3_DMAR);
    SET_BIT(huart6.Instance->CR3, USART_CR3_DMAT);

    //enalbe idle interrupt
    //Ê¹ÄÜ¿ÕÏÐÖÐ¶Ï
    __HAL_UART_ENABLE_IT(&huart6, UART_IT_IDLE);



    //disable DMA
    //Ê§Ð§DMA
    __HAL_DMA_DISABLE(&hdma_usart6_rx);

    while(hdma_usart6_rx.Instance->CR & DMA_SxCR_EN)
    {
        __HAL_DMA_DISABLE(&hdma_usart6_rx);
    }

    __HAL_DMA_CLEAR_FLAG(&hdma_usart6_rx, DMA_LISR_TCIF1);

    hdma_usart6_rx.Instance->PAR = (uint32_t) & (USART6->DR);
    //memory buffer 1
    //ÄÚ´æ»º³åÇø1
    hdma_usart6_rx.Instance->M0AR = (uint32_t)(rx1_buf);
    //memory buffer 2
    //ÄÚ´æ»º³åÇø2
    hdma_usart6_rx.Instance->M1AR = (uint32_t)(rx2_buf);
    //data length
    //Êý¾Ý³¤¶È
    __HAL_DMA_SET_COUNTER(&hdma_usart6_rx, dma_buf_num);

    //enable double memory buffer
    //Ê¹ÄÜË«»º³åÇø
    SET_BIT(hdma_usart6_rx.Instance->CR, DMA_SxCR_DBM);

    //enable DMA
    //Ê¹ÄÜDMA
    __HAL_DMA_ENABLE(&hdma_usart6_rx);


    //disable DMA
    //Ê§Ð§DMA
    __HAL_DMA_DISABLE(&hdma_usart6_tx);

    while(hdma_usart6_tx.Instance->CR & DMA_SxCR_EN)
    {
        __HAL_DMA_DISABLE(&hdma_usart6_tx);
    }

    hdma_usart6_tx.Instance->PAR = (uint32_t) & (USART6->DR);

}






extern  LK_Motor_t YAW_Motor;
extern MI_Motor mi_motor[4];
extern Super_Cap_Return_Pack super_cap_return_pack;
void print_task(void *arument) {

    static uint32_t counter;


   // usart1_tx_dma_init();
    osDelay(100);

    while (1) {
        usart_printf(&huart1,"%d:Tempreture:%d,Yaw:%d\r\n",counter++,super_cap_return_pack.Temperature,YAW_Motor.speed);
        osDelay(1000);
    }

}