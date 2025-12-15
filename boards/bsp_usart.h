#ifndef BSP_USART_H
#define BSP_USART_H

#include "main.h"
#include <stdio.h>

// 接收缓冲区大小
#define USART1_RX_BUF_SIZE 128

// 定义接收回调类型
typedef void (*usart_rx_callback_t)(uint8_t *data, uint16_t len);

// --- 初始化与设置 ---
void usart1_init(void);
void usart1_register_callback(usart_rx_callback_t callback);

// --- 发送函数 ---
// 1. 文本打印 (你原来的功能)
void usart_printf(UART_HandleTypeDef* huart, const char *fmt, ...);
// 2. 二进制发送 (用于发送协议包)
void usart_tx_binary(UART_HandleTypeDef* huart, uint8_t *data, uint16_t len);

#endif // BSP_USART_H