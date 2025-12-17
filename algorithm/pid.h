//
// Created by PanZhaoHui on 25-12-7.
//

#ifndef PID_H
#define PID_H

#include "main.h"
#define LIMIT 15000

typedef struct PID {
    float Kp;
    float Ki;
    float Kd;



    float intergral_limit;
    float output_limit;

    int16_t output;
    int16_t last_error;
    int16_t intergral;

}PID_t;

void PID_Init(PID_t *pid,float Kp,float Ki,float Kd,float intergral_limit,int16_t output_Limit);
int16_t PID_Caculate(PID_t *pid, int16_t target, int16_t get);
#endif //PID_H
