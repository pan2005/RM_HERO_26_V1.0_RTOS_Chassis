//
// Created by Admin on 2025/11/4.
//

#ifndef __BSP_USART_H
#define __BSP_USART_H
#include "main.h"
extern void usart6_init(uint8_t *rx1_buf, uint8_t *rx2_buf, uint16_t dma_buf_num);

extern void usart_tx_dma_init(UART_HandleTypeDef* huart);
extern void usart_tx_dma_enable(UART_HandleTypeDef* huart,uint8_t *data, uint16_t len);
void usart_printf(UART_HandleTypeDef* huart,const char *fmt, ...);
void print_task(void *arument);
#endif