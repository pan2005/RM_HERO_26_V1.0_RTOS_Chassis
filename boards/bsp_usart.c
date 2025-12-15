#include "bsp_usart.h"
#include <stdarg.h>
#include <string.h>

extern UART_HandleTypeDef huart1;
extern DMA_HandleTypeDef hdma_usart1_tx;
extern DMA_HandleTypeDef hdma_usart1_rx; // 确保在main.c中有定义Rx DMA

// 接收相关变量
static uint8_t rx_buffer[USART1_RX_BUF_SIZE];
static usart_rx_callback_t rx_callback = NULL;

// =============================================
//               发送部分 (TX)
// =============================================

// 底层DMA配置函数 (保留你的逻辑)
static void usart_dma_config_and_start(UART_HandleTypeDef* huart, uint8_t *data, uint16_t len)
{
    if (huart == NULL || huart->hdmatx == NULL) return;

    // 1. 禁用DMA
    __HAL_DMA_DISABLE(huart->hdmatx);
    while(huart->hdmatx->Instance->CR & DMA_SxCR_EN) {
        __HAL_DMA_DISABLE(huart->hdmatx);
    }

    // 2. 清除标志位 (TCIF7 是 F4 USART1_TX Stream7 的标志，如果换芯片需修改)
    __HAL_DMA_CLEAR_FLAG(huart->hdmatx, DMA_HISR_TCIF7);

    // 3. 配置地址和长度
    huart->hdmatx->Instance->M0AR = (uint32_t)(data);
    __HAL_DMA_SET_COUNTER(huart->hdmatx, len);

    // 4. 使能DMA
    __HAL_DMA_ENABLE(huart->hdmatx);
}

// 打印函数 (文本)
void usart_printf(UART_HandleTypeDef* huart, const char *fmt, ...) {
    static uint8_t tx_buf_text[256];
    static va_list ap;
    static uint16_t len;

    va_start(ap, fmt);
    len = vsnprintf((char *)tx_buf_text, 256, fmt, ap);
    va_end(ap);

    usart_dma_config_and_start(huart, tx_buf_text, len);
}

// 二进制发送函数 (用于协议)
void usart_tx_binary(UART_HandleTypeDef* huart, uint8_t *data, uint16_t len) {
    // 直接调用底层配置，这里不使用静态buffer，
    // 因为调用者(协议层)通常会提供buffer，或者在此处加临界区保护
    usart_dma_config_and_start(huart, data, len);
}

// =============================================
//               接收部分 (RX)
// =============================================

void usart1_init(void) {
    // 确保开启了 IDLE 和 DMA 标志
    SET_BIT(huart1.Instance->CR3, USART_CR3_DMAR);
    SET_BIT(huart1.Instance->CR3, USART_CR3_DMAT);

    // 启动接收 (IDLE中断模式)
    HAL_UARTEx_ReceiveToIdle_DMA(&huart1, rx_buffer, USART1_RX_BUF_SIZE);

    // 关闭半传输中断
    __HAL_DMA_DISABLE_IT(huart1.hdmarx, DMA_IT_HT);
}

void usart1_register_callback(usart_rx_callback_t callback) {
    rx_callback = callback;
}

// HAL库回调函数：当DMA满或IDLE中断触发时自动调用
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size) {
    if (huart->Instance == USART1) {
        // 1. 回调给应用层
        if (rx_callback != NULL && Size > 0) {
            rx_callback(rx_buffer, Size);
        }

        // 2. 重启接收 (指针复位到0)
        HAL_UARTEx_ReceiveToIdle_DMA(&huart1, rx_buffer, USART1_RX_BUF_SIZE);
        __HAL_DMA_DISABLE_IT(huart1.hdmarx, DMA_IT_HT);
    }
}

// 错误处理：防止溢出导致串口死锁
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART1) {
        HAL_UARTEx_ReceiveToIdle_DMA(&huart1, rx_buffer, USART1_RX_BUF_SIZE);
    }
}