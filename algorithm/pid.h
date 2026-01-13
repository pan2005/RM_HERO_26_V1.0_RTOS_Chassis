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

    float output;
    float last_error;
    float intergral;

}PID_t;

void PID_Init(PID_t *pid,float Kp,float Ki,float Kd,float intergral_limit,float output_Limit);
float PID_Caculate(PID_t *pid, float target, float get);
#endif //PID_H
