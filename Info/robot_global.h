//
// Created by Admin on 2026/1/12.
//

#ifndef ROBOT_GLOBAL_H
#define ROBOT_GLOBAL_H

#include "main.h"
#include "stdint.h"
#include "remote_control.h"


/* --- 模式枚举定义 --- */

typedef enum {

    GIMBAL_REMOTE = 0,       // 遥控器手动模式（基于IMU控制）
    GIMBAL_AUTO,         // 视觉自瞄模式
} gimbal_mode_e;

typedef enum {
    CHASSIS_UNABLE = 0,   // 失能状态
    CHASSIS_FOLLOW,      // 跟随模式,
    CHASSIS_SPIN,
} chassis_mode_e;

typedef enum {
    SHOOT_STOP = 0,      // 停止发射
    SHOOT_READY,         // 摩擦轮起旋
} shoot_mode_e;

typedef enum {
    TUCHUAN_CONTROL = 0,

    REMOTE_CONTROL,

}control_mode_e;

/* --- 裁判系统数据结构 --- */
typedef struct {
    uint8_t robot_id;
    uint8_t robot_level;
    uint16_t current_HP;
    uint16_t maximum_HP;
    uint16_t shooter_barrel_cooling_value;
    uint16_t shooter_barrel_heat_limit;
    uint16_t chassis_power_limit;
    uint8_t power_management_gimbal_output : 1;
    uint8_t power_management_chassis_output : 1;
    uint8_t power_management_shooter_output : 1;
} referee_robot_status_t;

typedef struct {
    uint16_t buffer_energy;
    uint16_t shooter_17mm_barrel_heat;
    uint16_t shooter_42mm_barrel_heat;
} referee_power_heat_t;

/* --- 核心控制结构体 --- */

typedef struct {
    // 1. 系统当前运行模式
    gimbal_mode_e  gimbal_mode;
    chassis_mode_e chassis_mode;
    shoot_mode_e   shoot_mode;
    control_mode_e control_mode;
    uint8_t shoot_gear;
    uint8_t fire;
    uint8_t imu_calibrated_flag;
    uint8_t pitch_motor_enabled;

    // 2. 云台姿态反馈数据 (由 Sensor Task 更新)
    struct {
       float yaw;        // 当前航向角 (度)
       float pitch;      // 当前俯仰角 (度)
       float roll;       // 当前横滚角 (度)
        float yaw_v;      // 航向角速度 (度/s)
        float pitch_v;    // 俯仰角速度 (度/s)
    } gimbal_current;

    // 3. 底盘运动状态 (由 Chassis Task 更新)
    struct {
        float yaw_speed;      // 云台相对于底盘的机械夹角 (由编码器转化)
        int16_t vx;
        int16_t vy;
        int16_t d_vz;
    } chassis_current;

    struct {
        float yaw;        //
        float pitch;      //
        float yaw_v;      //
        float pitch_v;    //
        int16_t vx;
        int16_t vy;
        int16_t vz; //底盘转向速度

    }target;

    // 4. 系统监控与异常处理
    struct {
        uint8_t  sensor_ready;   // 传感器校准完成标志
        uint8_t  remote_online;  // 遥控器在线标志
        uint8_t  vision_online;  // 视觉系统在线标志
    } monitor;

    // 5. 裁判系统数据
    struct {
        referee_robot_status_t robot_status;
        referee_power_heat_t power_heat;
    } referee;

} robot_ctrl_info_t;

/* --- 全局变量声明 --- */
extern robot_ctrl_info_t robot_ctrl;
extern uint8_t pitch_enable_flag;

/* --- 核心工具函数 --- */
void Robot_Global_Init(control_mode_e control_mode_p);
void Robot_Global_Update();
void Robot_Global_target_Update(void);

#endif