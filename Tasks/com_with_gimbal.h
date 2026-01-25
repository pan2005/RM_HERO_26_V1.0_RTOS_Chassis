//
// Created by PanZhaoHui on 25-12-15.
//

#ifndef COM_WITH_GIMBAL_H
#define COM_WITH_GIMBAL_H
#include "usart_protocol.h"
void communication_with_gimbal_task(void * argument);
extern ChassisInfo_t received_chassis;
extern GimbalInfo_t gimbal_info;
#endif //COM_WITH_GIMBAL_H
