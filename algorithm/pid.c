//
// Created by Admin on 2025/12/9.
//



#include "pid.h"


void PID_Init(PID_t *pid,float Kp,float Ki,float Kd,float intergral_limit,float output_Limit) {
    pid->Kp = Kp;
    pid->Ki = Ki;
    pid->Kd = Kd;
    pid->intergral_limit = intergral_limit;

    pid->output_limit = LIMIT;

    pid->last_error = 0;

}
float limit_function(float limit,  float intergral) {
    if (intergral > limit) {
        intergral = limit;
    }
    else if (intergral < -limit) {
        intergral = -limit;
    }

    return intergral;

}
float PID_Caculate(PID_t *pid, float target, float get) {
    float error = target - get;
    pid->output = 0;
    pid->intergral += error;
    pid->output = pid->Kp * error +limit_function(pid->intergral_limit ,pid->Ki * pid->intergral)  + pid->Kd * (error - pid->last_error);
    pid->output = limit_function(pid->output_limit,pid->output);
    pid->last_error = error;


    return  pid->output;
}