//
// Created by Admin on 2025/12/4.
//

#include "chassis_control_task.h"

#include <stdio.h>

#include "remote_control.h"

#include "CAN_receive.h"
#include "cmsis_os2.h"
#include "LKMF9025.h"


#include "pid.h"
#include "DJI_Motor.h"

#include "com_with_gimbal.h"
#include "robot_global.h"
#include <math.h>
#include "irr_fliter.h"
#include "bsp_usart.h"
#include "Supercapacitor.h"

extern CAN_HandleTypeDef hcan1;
extern TIM_HandleTypeDef htim6;

dji_motor_object_t shoot_motor;

PID_t Follow;


Can_Motor_t wheel_motor_1;
M3508_Data_t wheel_priv_1;

Can_Motor_t wheel_motor_2;
M3508_Data_t wheel_priv_2;

Can_Motor_t wheel_motor_3;
M3508_Data_t wheel_priv_3;

Can_Motor_t wheel_motor_4;
M3508_Data_t wheel_priv_4;

Can_Motor_t wheel_motor_4;
M3508_Data_t wheel_priv_4;

Can_Motor_t thumbwheel_motor;
M3508_Pos_Data_t thumbwheel_priv;

Can_Motor_t yaw_motor;
GM6020_Data_t yaw_priv;

float friction_comp = 0.0f;
float friction_threshold = 800.0f; // 刚好能让云台动起来的最小电流值（在车上实际测出来！）


float Kf = 3.8f; //YAW轴前馈补偿值
float velocity_turn = 0;

// 拨弹盘状态机变量
static uint8_t fire_prev_state = 0;  // 上一次 fire 状态
static uint8_t fire_rising_detected = 0;  // 上升沿检测标志

// 功率限制状态变量
static uint8_t power_limit_active = 0;  // 功率限制激活标志
static const int16_t VOLTAGE_LOW_THRESHOLD = 550;    // 低电压阈值 (5.5V x100)
static const int16_t VOLTAGE_HIGH_THRESHOLD = 1000;  // 恢复电压阈值 (10.0V x100)
static const float SPEED_SCALE_FACTOR = 0.5f;        // 速度缩放比例 (50%)

static const uint16_t T_LIST[] = {

    3000,2000,1000, 667, 500, 400, 333, 286, 250, 222, 200, 182, 167, 154, 143, 133, 125, 118, 111, 105, 100, // 1-10Hz等
    95, 91, 87, 83, 80, 77, 74, 71, 69, 67, 65, 63, 61, 59, 57, 56, 54, 53, 51, 50, 48, 45, // ...
    42, 38, 36, 33, 31, 29, 28, 26, 25, 20, 17, 14, 13, 11, 10, 9, 8, 5, 4, 3, 2
};
#define T_LIST_LEN (sizeof(T_LIST)/sizeof(uint16_t))
#define LOG_BUF_SIZE 2000
float log_u[LOG_BUF_SIZE];
float log_v[LOG_BUF_SIZE];
uint16_t log_read_ptr = 0;
uint16_t log_write_ptr = 0;


uint8_t sweep_running = 1;   // 扫频标志位
uint16_t current_f_idx = 0;  // 当前频率索引
uint32_t tick_in_period = 0; // 当前周期内的计步
uint16_t cycle_count = 0;    // 当前频率已跑过的周期数

const float SWEEP_AMPLITUDE = 15000.0f; // 激励幅值
const uint16_t CYCLES_PER_F = 5;        // 每个频率点跑 5 个周期（根据需要调整）

// 拨弹盘堵转检测变量
static uint32_t last_stall_check_time = 0;
static const uint32_t STALL_CHECK_INTERVAL = 2000; // 2 秒检测间隔
static const float STALL_ANGLE_THRESHOLD = 0.7854f; // 45 度 = π/4 ≈ 0.7854 弧度

float Get_Sine_Wave(float amplitude, float offset) {
    // 固定的数学常量

    const float PERIOD_S = 2.0f;     // 信号周期 2s
    const float STEP_S = 0.001f;     // 步进周期 1ms (1kHZ)

    // 记录当前的步数 (static 变量在函数调用结束后不会被销毁)
    static uint32_t tick = 0;

    // 计算总点数: 2.0 / 0.001 = 2000
    const uint32_t max_tick = (uint32_t)(PERIOD_S / STEP_S);

    // 计算当前弧度: (tick / max_tick) * 2 * PI
    float phase = (float)tick * (2.0f * PI / (float)max_tick);

    // 计算正弦值
    float result = amplitude * sinf(phase) + offset;


    // 更新步数，到 2000 后归零重新开始
    tick++;
    if (tick >= max_tick) {
        tick = 0;
    }

    return result;
}





void chassis_control_task() {


    osDelay(10);



    PID_Init(&wheel_priv_1.speed_pid, 7.0f, 0.1f, 0.0f,0, 500, 16384);
    Can_Motor_Init(&wheel_motor_1, &hcan1, 0x201, M3508_Decode, M3508_Update, &wheel_priv_1);

    PID_Init(&wheel_priv_2.speed_pid, 7.0f, 0.1f, 0.0f,0, 500, 16384);
    Can_Motor_Init(&wheel_motor_2, &hcan1, 0x202, M3508_Decode, M3508_Update, &wheel_priv_2);

    PID_Init(&wheel_priv_3.speed_pid, 7.0f, 0.1f, 0.0f,0, 500, 16384);
    Can_Motor_Init(&wheel_motor_3, &hcan1, 0x203, M3508_Decode, M3508_Update, &wheel_priv_3);

    PID_Init(&wheel_priv_4.speed_pid, 7.0f, 0.1f, 0.0f,0, 500, 16384);
    Can_Motor_Init(&wheel_motor_4, &hcan1, 0x204, M3508_Decode, M3508_Update, &wheel_priv_4);

    // 拨弹盘电机位置环初始化
    thumbwheel_priv.gear_ratio = 51.0f;  // 减速比 51:1
    thumbwheel_priv.round_count = 0;
    thumbwheel_priv.target_angle = 0;    // 初始目标角度为 0
    PID_Init(&thumbwheel_priv.pos_pid, 4000.0f, 0.0f, 5.0f, 0, 3500, 5000);  // 位置环 PID (需要根据实际调试)
    PID_Init(&thumbwheel_priv.speed_pid, 27.0f, 1.0f, 0.0f, 0, 3500, 16384);  // 速度环 PID
    Can_Motor_Init(&thumbwheel_motor, &hcan1, 0x205, M3508_Pos_Decode, M3508_Pos_Update, &thumbwheel_priv);

    //yaw_priv.whether_extern_data = 1;

    PID_Init(&yaw_priv.speed_pid, 400.0f, 0.14f, 10.00f,0, 3500, 25000);  //450 0.14
    PID_Init(&yaw_priv.pos_pid, 150.0f, 0.1f, 400.0f,0, 3500, 6000);  // 450 0  500
    Can_Motor_Init(&yaw_motor, &hcan1, 0x206, GM6020_Decode, GM6020_Update, &yaw_priv);

    PID_Init(&Follow, 2000.0f, 0.0f, 10.0f,0, 500, 5000);






    float velocity_forword = 0;

    float velocity_14 = 0;
    float velocity_23 = 0;
    float world_error = 0;
    float world_yaw_target_angle = 0;

    osDelay(100);
    HAL_TIM_Base_Start_IT(&htim6);

    last_stall_check_time = osKernelGetTickCount();

    while (1) {


        robot_ctrl.chassis_mode = received_chassis.mode;

        world_yaw_target_angle = received_chassis.yaw;

        // 功率限制状态机更新
        if (super_cap_return_pack.Capacity_Voltage < VOLTAGE_LOW_THRESHOLD) {
            power_limit_active = 1;  // 进入功率限制状态
        } else if (super_cap_return_pack.Capacity_Voltage > VOLTAGE_HIGH_THRESHOLD) {
            power_limit_active = 0;  // 退出功率限制状态
        }

        //yaw_priv.INS_angle = received_chassis.yaw_INS;
        world_error = world_yaw_target_angle - received_chassis.yaw_INS;
        world_error = Radian_Normalize(world_error);

        float theta = yaw_priv.total_angle + 2.5f; // 1.5707963f = PI / 2
        theta = Radian_Normalize(theta);

    // -------------------------------------------------------------
        // 【核心修复】 RM 标准：底盘优选劣弧换向 (双向跟随)
        // -------------------------------------------------------------
        float follow_theta = theta;

        // 绝对不要去反转平移速度！底部的旋转矩阵会自动处理一切！
        float chassis_vx = received_chassis.vx;
        float chassis_vy = received_chassis.vy;

        // 如果云台与底盘的夹角超过 90 度 (PI/2 ≈ 1.57rad)
        // 我们仅仅欺骗底盘的 PID，让它以为车尾就是车头，不再转圈
        if (follow_theta > 1.57f) {
            follow_theta -= PI;
        } else if (follow_theta < -1.57f) {
            follow_theta += PI;
        }

        // 现在的 follow_theta 永远被限制在[-90度, +90度] 之间
        // 180度的奇异点（突变点）被彻底消灭，而且运动方向绝对正确！！！

        if (robot_ctrl.chassis_mode == CHASSIS_FOLLOW) {
            float trans_speed = sqrtf(chassis_vx * chassis_vx + chassis_vy * chassis_vy);

            // 动态权重计算
            float follow_weight = 0.0f;
            float speed_threshold_low = 100.0f;
            float speed_threshold_high = 400.0f;

            if (trans_speed < speed_threshold_low) {
                follow_weight = 0.0f;
            } else if (trans_speed > speed_threshold_high) {
                follow_weight = 1.0f;
            } else {
                follow_weight = (trans_speed - speed_threshold_low) / (speed_threshold_high - speed_threshold_low);
            }

            // 云台角度死区
            float deadband = 0.087f;
            if (fabsf(follow_theta) < deadband) {
                follow_theta = 0.0f;
            } else {
                if (follow_theta > 0) follow_theta -= deadband;
                if (follow_theta < 0) follow_theta += deadband;
            }

            // 计算跟随 PID
            float raw_velocity_turn = PID_Calculate(&Follow, 0, follow_theta);

            // 应用功率限制
            if (power_limit_active) {
                velocity_turn = raw_velocity_turn * follow_weight * 3.0f * SPEED_SCALE_FACTOR + received_chassis.vz * 10.0f * SPEED_SCALE_FACTOR;
            } else {
                velocity_turn = raw_velocity_turn * follow_weight * 3.0f + received_chassis.vz * 10.0f;
            }
        }
        else if (robot_ctrl.chassis_mode == CHASSIS_SPIN){
            // 应用功率限制
            if (power_limit_active) {
                if(super_cap_return_pack.Capacity_Voltage > 550){
                    velocity_turn = Get_Sine_Wave(500, 3000);  // 降低幅值和偏移
                }
                else{
                    velocity_turn = Get_Sine_Wave(250, 1500);  // 进一步降低
                }
            } else {
                if(super_cap_return_pack.Capacity_Voltage > 550){
                    velocity_turn = Get_Sine_Wave(1000, 6000);
                }
                else if(super_cap_return_pack.Capacity_Voltage < 550){
                    velocity_turn = Get_Sine_Wave(500, 3000);
                }
            }
        }
        else {
            velocity_turn = 0;
        }

        // -------------------------------------------------------------
        // 坐标系旋转矩阵变换
        // 这里依然使用原始的 theta 和原始的 chassis_vx/vy，数学会自动接管一切！
        // -------------------------------------------------------------
        float target_vx = chassis_vy * cosf(theta) - chassis_vx * sinf(theta);
        float target_vy = chassis_vy * sinf(theta) + chassis_vx * cosf(theta);

        velocity_14 = target_vy * 10.0f + target_vx * 10.0f;
        velocity_23 = target_vy * 10.0f - target_vx * 10.0f;

        // 应用功率限制到轮子速度
        if (power_limit_active) {
            wheel_priv_1.target_speed = (velocity_14 + velocity_turn) * SPEED_SCALE_FACTOR;
            wheel_priv_2.target_speed = -(velocity_23 - velocity_turn) * SPEED_SCALE_FACTOR;
            wheel_priv_3.target_speed = (velocity_23 + velocity_turn) * SPEED_SCALE_FACTOR;
            wheel_priv_4.target_speed = -(velocity_14 - velocity_turn) * SPEED_SCALE_FACTOR;
        } else {
            wheel_priv_1.target_speed = velocity_14 + velocity_turn;
            wheel_priv_2.target_speed = -(velocity_23 - velocity_turn);
            wheel_priv_3.target_speed = velocity_23 + velocity_turn;
            wheel_priv_4.target_speed = -(velocity_14 - velocity_turn);
        }

            yaw_priv.target_angle = yaw_priv.total_angle + world_error;
        yaw_priv.feedforward_current = Kf * velocity_turn + received_chassis.mouse_x_v * 0.2f ;


            // --- 拨弹盘电机控制逻辑 ---
            // 检测 fire 信号从 0 到 1 的跳变 (正转)
            if (received_chassis.fire == 1 && fire_prev_state == 0) {
                fire_rising_detected = 1;  // 检测到上升沿
            }
            fire_prev_state = received_chassis.fire;  // 保存当前状态

            // 如果检测到上升沿，执行一次拨弹动作
            if (fire_rising_detected) {
                fire_rising_detected = 0;  // 清除标志

                // 判断正转还是反转 (可以根据其他条件，比如鼠标滚轮)
                // 这里假设 fire=1 是正转，你也可以根据实际需求修改
                static float fire_angle_step = -1.046f;  // 每次转动 0.5 弧度

                // 累加目标角度 (相对于当前位置)
                thumbwheel_priv.target_angle += fire_angle_step;

                // 限制最大角度 (可选，防止角度过大溢出)

            }
            else if (received_chassis.fire == 2) {
                thumbwheel_priv.target_angle += 0.001f;
            
            }

            // --- 拨弹盘堵转检测 ---
            uint32_t current_tick = osKernelGetTickCount();
            if (current_tick - last_stall_check_time >= STALL_CHECK_INTERVAL) {
                last_stall_check_time = current_tick;

                // 计算目标角度与当前角度的差值
                float angle_error = thumbwheel_priv.target_angle - thumbwheel_priv.current_angle;
                angle_error = fabsf(angle_error);

                // 如果角度差大于 45 度（π/4 弧度），则认为发生堵转
                if (angle_error > STALL_ANGLE_THRESHOLD) {
                    // 重置目标角度为当前角度，消除累积误差
                    thumbwheel_priv.target_angle = thumbwheel_priv.current_angle;
                }
            }

        osDelay(2);

    }
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{

    if (htim->Instance == TIM2)
    {
        HAL_IncTick();
    }
    if (htim->Instance == TIM6)
    {
        // static uint8_t counter = 0;
        // --- 1. 安全检查 ---
        if (robot_ctrl.chassis_mode == CHASSIS_UNABLE) {
            // 下发全0功率，防止电机乱跑
            DJI_Motor_SendGroup_0x200(&hcan1, 0, 0, 0, 0);
            DJI_Motor_SendGroup_0x1FF(&hcan1, 0, 0, 0, 0);
            return;
        }
        if (wheel_motor_1.update_func == NULL || wheel_motor_2.update_func == NULL) {
            return ;
        }

        // --- 2. 核心控制逻辑 (1ms 一次) ---
        // 这里的电机 update 包含了 PID 计算
        wheel_motor_1.update_func(&wheel_motor_1);
        wheel_motor_2.update_func(&wheel_motor_2);
        wheel_motor_3.update_func(&wheel_motor_3);
        wheel_motor_4.update_func(&wheel_motor_4);

        yaw_motor.update_func(&yaw_motor);          // GM6020 串联 PID 在这里运行
        thumbwheel_motor.update_func(&thumbwheel_motor);

       // yaw_motor.output_value = NotchFilter_Apply(&beltNotchFilter, yaw_motor.output_value);

        // --- 3. 立即下发 CAN ---
        DJI_Motor_SendGroup_0x200(&hcan1,
            wheel_motor_1.output_value, wheel_motor_2.output_value,
            wheel_motor_3.output_value, wheel_motor_4.output_value);



        DJI_Motor_SendGroup_0x1FF(&hcan1,
            thumbwheel_motor.output_value, yaw_motor.output_value, 0, 0);

        // --- 4. 系统辨识采样 (可选) ---
        // 如果要做系统辨识，建议在这里通过 DMA 串口发送 yaw_motor 的数据



        // if (sweep_running) {
        //     uint16_t T = T_LIST[current_f_idx];
        //
        //     // 1. 计算当前时刻的正弦输出
        //     // sin(2 * PI * t / T)
        //     float angle = 2.0f * 3.14159265f * (float)tick_in_period / (float)T;
        //     float u = SWEEP_AMPLITUDE * sinf(angle);
        //
        //     yaw_motor.output_value = (int16_t)u;
        //
        //     // 2. 更新计步
        //     tick_in_period++;
        //
        //     // 3. 检查当前周期是否结束
        //     if (tick_in_period >= T) {
        //         tick_in_period = 0;
        //         cycle_count++;
        //
        //         // 4. 检查当前频率点的周期数是否跑完
        //         if (cycle_count >= CYCLES_PER_F) {
        //             cycle_count = 0;
        //             current_f_idx++; // 切换到下一个频率
        //
        //             // 5. 检查是否扫完全部频率
        //             if (current_f_idx >= T_LIST_LEN) {
        //                 sweep_running = 0;
        //                 yaw_motor.output_value = 0;
        //                 current_f_idx = 0;
        //             }
        //         }
        //     }
        //
        //     if (counter >= 5) {
        //         log_u[log_write_ptr] = u;
        //         log_v[log_write_ptr] = (float)yaw_motor.measure.speed_rpm;
        //         log_write_ptr = (log_write_ptr + 1) % LOG_BUF_SIZE;
        //         counter = 0;
        //
        //     }
        //     counter ++;



        // 执行 CAN 发送
       // DJI_Motor_SendGroup_0x1FF(&hcan1, 0, yaw_motor.output_value, 0,0);




    }
}
