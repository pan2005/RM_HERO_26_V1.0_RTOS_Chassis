//
// Created by Admin on 2025/11/9.
//

#ifndef __LKMF9025_H
#define __LKMF9025_H
#include "main.h"
#include "daemon.h"
#define LK9025_Single_StdId 0x140
#define Torque_Control_Mode 0xA1
#define Speed_Control_Mode 0xA2
#define Multi_Loop_Control_Mode1 0xA3
#define Multi_Loop_Control_Mode2 0xA4
#define Single_Loop_Control_Mode1 0xA5
#define Single_Loop_Control_Mode2 0xA6
#define Increment_Control_Mode1 0xA7
#define Increment_Control_Mode2 0xA8

typedef struct {
    uint8_t id;
    int16_t speed;
    int16_t  iq;
    uint8_t tempreture;
    uint16_t encoder;
    DaemonInstance *daemon;

}LK_Motor_t;
extern  LK_Motor_t YAW_Motor;
void LK_Motor_SpeedControl(const LK_Motor_t * LK_Motor, int32_t speed);

#endif