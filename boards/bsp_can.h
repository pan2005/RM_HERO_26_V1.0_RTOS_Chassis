#ifndef __BSP_CAN_H
#define __BSP_CAN_H

#include "stm32f4xx_hal.h" // 根据你的芯片型号修改，如 stm32f1xx_hal.h

/* 定义 CAN 接收回调函数类型 */
/* device_ptr: 指向具体的设备对象（如 motor_struct）， data: 8字节CAN数据 */
typedef void (*can_rx_callback_t)(void *device_ptr, uint8_t *data);

/* CAN 接收回调注册结构体 */
typedef struct
{
    CAN_HandleTypeDef *hcan;      // 所属CAN句柄
    uint32_t std_id;              // 监听的CAN ID
    can_rx_callback_t callback;   // 收到数据后的回调函数
    void *device_ptr;             // 传递给回调函数的设备指针（上下文）
    uint8_t is_registered;        // 标记是否已使用
} can_rx_hook_t;

/* 最大支持的注册数量，根据项目需求调整 */
#define CAN_MAX_RX_CALLBACKS 16

/* 函数声明 */
void BSP_CAN_Init(void);
int8_t BSP_CAN_RegisterRxCallback(CAN_HandleTypeDef *hcan, uint32_t std_id, can_rx_callback_t cb, void *obj);
uint8_t BSP_CAN_SendMsg(CAN_HandleTypeDef *hcan, uint32_t std_id, uint8_t *data, uint8_t len);


static void parse_lk_motor_data(CAN_RxHeaderTypeDef *header, uint8_t *data);
static void parse_cybergear_data(CAN_RxHeaderTypeDef *header, uint8_t *data);
static void parse_Sup_cap_data(CAN_RxHeaderTypeDef *header, uint8_t *data);

#endif //BSP_CAN_H
