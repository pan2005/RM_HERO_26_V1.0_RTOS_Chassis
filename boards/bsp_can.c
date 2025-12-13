#include "bsp_can.h"
#include <string.h>

/* 管理所有注册的回调函数 */
static can_rx_hook_t rx_hooks[CAN_MAX_RX_CALLBACKS];
static uint8_t hook_count = 0;

extern CAN_HandleTypeDef hcan1;
extern CAN_HandleTypeDef hcan2;

/**
  * @brief 初始化CAN过滤器等
  */
void BSP_CAN_Init(void)
{
    // 这里保留原有的过滤器配置代码
    // 确保过滤器设置为接收所有ID (Mask模式)，或者根据注册表动态配置
    // 下面是通用的全接收配置示例：
    CAN_FilterTypeDef can_filter_st;
    can_filter_st.FilterActivation = ENABLE;
    can_filter_st.FilterMode = CAN_FILTERMODE_IDMASK;
    can_filter_st.FilterScale = CAN_FILTERSCALE_32BIT;
    can_filter_st.FilterIdHigh = 0x0000;
    can_filter_st.FilterIdLow = 0x0000;
    can_filter_st.FilterMaskIdHigh = 0x0000;
    can_filter_st.FilterMaskIdLow = 0x0000;
    can_filter_st.FilterBank = 0;
    can_filter_st.FilterFIFOAssignment = CAN_RX_FIFO0;

    HAL_CAN_ConfigFilter(&hcan1, &can_filter_st);
    HAL_CAN_Start(&hcan1);
    HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING);

    // 如果有CAN2，同理配置，注意 FilterBank 要从14开始
    can_filter_st.FilterBank = 14;
    HAL_CAN_ConfigFilter(&hcan2, &can_filter_st);
    HAL_CAN_Start(&hcan2);
    HAL_CAN_ActivateNotification(&hcan2, CAN_IT_RX_FIFO0_MSG_PENDING);

    // 清空回调表
    memset(rx_hooks, 0, sizeof(rx_hooks));
    hook_count = 0;
}

/**
  * @brief 注册接收回调函数
  * @param hcan: CAN句柄
  * @param std_id: 要监听的CAN ID
  * @param cb: 回调函数指针
  * @param obj: 设备对象指针 (在回调时会传回，解决全局变量问题)
  * @return 0: 成功, -1: 表满
  */
int8_t BSP_CAN_RegisterRxCallback(CAN_HandleTypeDef *hcan, uint32_t std_id, can_rx_callback_t cb, void *obj)
{
    if (hook_count >= CAN_MAX_RX_CALLBACKS) return -1;

    rx_hooks[hook_count].hcan = hcan;
    rx_hooks[hook_count].std_id = std_id;
    rx_hooks[hook_count].callback = cb;
    rx_hooks[hook_count].device_ptr = obj;
    rx_hooks[hook_count].is_registered = 1;
    hook_count++;

    return 0;
}

/**
  * @brief 通用CAN发送函数
  */
uint8_t BSP_CAN_SendMsg(CAN_HandleTypeDef *hcan, uint32_t std_id, uint8_t *data, uint8_t len)
{
    CAN_TxHeaderTypeDef tx_header;
    uint32_t send_mail_box;

    tx_header.StdId = std_id;
    tx_header.IDE = CAN_ID_STD;
    tx_header.RTR = CAN_RTR_DATA;
    tx_header.DLC = len;

    if (HAL_CAN_AddTxMessage(hcan, &tx_header, data, &send_mail_box) != HAL_OK)
    {

        return 1; // Error
    }
    return 0; // OK
}

/**
  * @brief 统一的 HAL CAN 接收回调 (替代了原来代码中的 switch-case)
  */
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    CAN_RxHeaderTypeDef rx_header;
    uint8_t rx_data[8];

    HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &rx_header, rx_data);

    // 遍历注册表，找到匹配 ID 和 CAN 句柄的回调函数
    for (uint8_t i = 0; i < hook_count; i++)
    {
        if (rx_hooks[i].is_registered &&
            rx_hooks[i].hcan == hcan &&
            rx_hooks[i].std_id == rx_header.StdId)
        {
            if (rx_hooks[i].callback != NULL)
            {
                // 调用注册的解析函数，传入具体的设备指针
                rx_hooks[i].callback(rx_hooks[i].device_ptr, rx_data);
            }
            return; // 找到处理者后立即返回 (如果ID不重复)
        }
    }
}