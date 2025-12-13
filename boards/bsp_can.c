//
// Created by PanZhaoHui on 25-11-1.
//

#include "bsp_can.h"

#include "cybergear.h"
#include "stdio.h"
#include "string.h"
#include "LKMF9025.h"
#include "daemon.h"
#include "bsp_buzzer.h"
#include "Supercapacitor.h"
#include "CAN_receive.h"


xCAN_InfoDef  xCAN1 = {0};      // 定义一个数据结构体，用于管理收、发的数据和信息
extern Super_Cap_Return_Pack super_cap_return_pack ;
extern motor_measure_t motor_chassis[7];

/**
************************************************************************
* @brief:      	can_bsp_init(void)
* @param:       void
* @retval:     	void
* @details:    	CAN 使能
************************************************************************
**/
void can_bsp_init(void)
{
	CAN1_FilterInit();
	HAL_CAN_Start(&hcan1);                               //开启CAN
	HAL_CAN_ActivateNotification(&hcan1, 0);
}





/******************************************************************************
 * 函  数： CAN1_SendData
 * 功  能： CAN发送数据函数
 * 参  数： uint8_t* msgData   需发送数据的地址
 *          uint8_t  len       发送的字节数; 最大值：8
 * 返回值： 发送状态; 成功-0、错误-1、忙错误-2、超时-3
 ******************************************************************************/
uint8_t CAN1_SendData(uint8_t *msgData, uint8_t len)
{
    // 定义两个变量
    static uint32_t TxMailbox = 0;                                                 // 用于记录发送成功时所用的邮箱编号：0~2; 被发送函数HAL_CAN_AddTxMessage()赋值
    static uint32_t txStatus  = 0;                                                 // 用于记录发送状态; 成功-0、错误-1、忙错误-2、超时-3; 同上，被发送函数HAL_CAN_AddTxMessage()赋值
    // 限制字节数
    if(len>8)                                                                      // 判断字节是否超过8字节
        len=8;                                                                     // 如果超过8字节，只发送前8个字节
    // 配置帧信息
    xCAN1.TxHeader.ExtId = CAN_TX_ID;                                              // 帧ID, 在bsp_CAN.h中定义
    xCAN1.TxHeader.IDE   = CAN_ID_EXT;                                             // 帧格式; 标准帧: CAN_ID_STD，注意修改上行为StdID; 扩展帧: CAN_ID_EXT, 注意修改上行为ExtID
    xCAN1.TxHeader.RTR   = CAN_RTR_DATA;                                           // 数据帧;
    xCAN1.TxHeader.DLC   = len;                                                    // 数据字节数，
    xCAN1.TxHeader.TransmitGlobalTime = DISABLE;                                   // 使能时间戳添加到最后两个字节：Data[6]、Data[7]
    // 等待发送邮箱空闲
    while (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 0);                          // 每次发送前，要等待至有发送邮箱空闲。共有3个发送邮箱。如果返回值为0，即没有发送邮箱空闲，继续等
    // 发送
    txStatus = HAL_CAN_AddTxMessage(&hcan1, &xCAN1.TxHeader, msgData, &TxMailbox); // 发送
    // 返回发送状态
    return txStatus;                                                               // 返回发送状态; 成功-0、错误-1、忙错误-2、超时-3
}





/******************************************************************************
 * 函  数： CAN1_FilterInit
 * 功  能： CAN1筛选器初始化
 * 备  注： 为方便调试，本函数配置为：接收所有数据帧，包括标准帧、扩展帧
 * 参  数： 无
 * 返回值： 无
 ******************************************************************************/
void CAN1_FilterInit(void)
{
      /* USER CODE BEGIN 2 */
	//CAN过滤器初始化
	CAN_FilterTypeDef can_filter_st;
	/*	can_filter_st.FilterActivation = ;
		can_filter_st.FilterBank = ;
		can_filter_st.FilterFIFOAssignment = ;
		can_filter_st.FilterIdHigh = ;
		can_filter_st.FilterIdLow = ;
		can_filter_st.FilterMaskIdHigh = ;
		can_filter_st.FilterMaskIdLow = ;
		can_filter_st.FilterMode = ;
		can_filter_st.FilterScale = ;
		can_filter_st.SlaveStartFilterBank = ;

    HAL_CAN_ConfigFilter(&hcan1, &can_filter_st);
	*/



    can_filter_st.FilterActivation = ENABLE;
    can_filter_st.FilterMode = CAN_FILTERMODE_IDMASK;
    can_filter_st.FilterScale = CAN_FILTERSCALE_32BIT;
    can_filter_st.FilterIdHigh = 0x0000;
    can_filter_st.FilterIdLow = 0x0000;
    can_filter_st.FilterMaskIdHigh = 0x0000;
    can_filter_st.FilterMaskIdLow = 0x0000;
    can_filter_st.FilterBank = 0;
    can_filter_st.FilterFIFOAssignment = CAN_RX_FIFO0;
    can_filter_st.SlaveStartFilterBank = 14;
    HAL_CAN_ConfigFilter(&hcan1, &can_filter_st);
    HAL_CAN_Start(&hcan1);
    HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING);

    // HAL_TIM_Base_Start_IT(&htim2);//定时中断初始化
  /* USER CODE END 2 */

}



/*****************************回调函数 负责接回传信息 可转移至别处*****************************/
/**
  * @brief          hal库CAN回调函数,接收电机数据
  * @param[in]      hcan:CAN句柄指针
  * @retval         none
  */
      //接收数据

extern LK_Motor_t YAW_Motor;
extern MI_Motor mi_motor[4];


void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	CAN_RxHeaderTypeDef rxMsg;//发送接收结构体
	uint8_t rx_data[8];
	HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &rxMsg, rx_data);//接收数据
	if (rxMsg.IDE == CAN_ID_STD) {
		if (rxMsg.StdId == 0x301) {
			parse_Sup_cap_data(&rxMsg, rx_data);
		}
		// 标准帧 - LKMF9025电机

		else if (rxMsg.StdId == 0x140) {
			parse_lk_motor_data(&rxMsg, rx_data);
		}
		else {
			switch (rxMsg.StdId)
			{
			    case CAN_3508_M1_ID:
			    case CAN_3508_M2_ID:
			    case CAN_3508_M3_ID:
			    case CAN_3508_M4_ID:
			    case CAN_YAW_MOTOR_ID:
			    case CAN_PIT_MOTOR_ID:
			    case CAN_TRIGGER_MOTOR_ID:
			    {
			        static uint8_t i = 0;
			        //get motor id
			        i = rxMsg.StdId - CAN_3508_M1_ID;
			        get_motor_measure(&motor_chassis[i], rx_data);
			        break;
			    }

			    default:
			    {
			        break;
			    }
			}


		}

	} else if (rxMsg.IDE == CAN_ID_EXT) {
		// 扩展帧 - Cybergear电机
		parse_cybergear_data(&rxMsg, rx_data);
	}
}
static void parse_lk_motor_data(CAN_RxHeaderTypeDef *header, uint8_t *data) {
	uint8_t motor_id = header->StdId - LK9025_Single_StdId;
	DaemonReload(YAW_Motor.daemon);
	buzzer_off();
	switch (data[0]) {
		case Speed_Control_Mode:
			YAW_Motor.tempreture = data[1];
			YAW_Motor.iq = ( data[3] << 8) | data[2];
			YAW_Motor.speed = ( data[5] << 8) | data[4];
			YAW_Motor.encoder = ( data[7] << 8) | data[6];

			break;
		case Multi_Loop_Control_Mode1:
			break;
		default:
			break;
	}
}
static void parse_cybergear_data(CAN_RxHeaderTypeDef *header, uint8_t *data) {
	uint8_t motor_id = Get_Motor_ID(header->ExtId);
	switch (motor_id) {
		case 0x7F:

			Motor_Data_Handler(&mi_motor[1],data,header->ExtId);
			break;
		default:
			break;
	}
}
static void parse_Sup_cap_data(CAN_RxHeaderTypeDef *header, uint8_t *data) {
	super_cap_return_pack.Cell_Power = (data[0] << 8) | data[1];
	super_cap_return_pack.Cap_Power = (data[2] << 8) | data[3];
	super_cap_return_pack.Capacity_Voltage = (data[4] << 8) | data[5];
	super_cap_return_pack.Temperature = data[6];
	super_cap_return_pack.Status.value = data[7];

}