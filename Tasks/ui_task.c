//
// Created by PanZhaoHui on 26-3-16.
//
#include "cmsis_os2.h"
#include "ui_task.h"

#include "com_with_gimbal.h"
#include "robot_global.h"
#include "chassis_control_task.h"
#include <string.h>
#include "ui_g.h"
#include "Supercapacitor.h"

#define CAP_VOLTAGE_MIN 5.0f
#define CAP_VOLTAGE_MAX 27.5f
#define CAP_BAR_START_X 180
#define CAP_BAR_MAX_LEN 180

static int cap_bar_end_x_from_voltage(float voltage)
{
    float clamped_voltage = voltage;
    const float min_sq = CAP_VOLTAGE_MIN * CAP_VOLTAGE_MIN;
    const float max_sq = CAP_VOLTAGE_MAX * CAP_VOLTAGE_MAX;
    float ratio;

    if (clamped_voltage < CAP_VOLTAGE_MIN) {
        clamped_voltage = CAP_VOLTAGE_MIN;
    }
    if (clamped_voltage > CAP_VOLTAGE_MAX) {
        clamped_voltage = CAP_VOLTAGE_MAX;
    }

    ratio = (clamped_voltage * clamped_voltage - min_sq) / (max_sq - min_sq);
    if (ratio < 0.0f) {
        ratio = 0.0f;
    }
    if (ratio > 1.0f) {
        ratio = 1.0f;
    }

    return CAP_BAR_START_X + (int)(ratio * CAP_BAR_MAX_LEN);
}

void UI_Task(void *argument) {
    // 1. 等待裁判系统稳定
    osDelay(4000);

    // 2. 初始化静态界面 (ui_g.c 里的函数)
    ui_init_g();

    while(1) {
        // 3. 实时更新电控数据到 UI 结构体
        // 例如：把超级电容百分比传给那个 CAP 元件
        float cap_voltage = super_cap_return_pack.Capacity_Voltage / 100.0f;

         ui_g_Ungroup_CAP_energy->number = (int32_t)cap_voltage;
         ui_g_Ungroup_Automode->number = (int32_t)robot_ctrl.chassis_mode;
         ui_g_Ungroup_TH_current_number->number = thumbwheel_motor.measure.given_current;
         ui_g_Ungroup_FireMode->number = (int32_t)robot_ctrl.shoot_gear;
         ui_g_Ungroup_CAP_bar_fill->end_x = cap_bar_end_x_from_voltage(cap_voltage);

        // 4. 检查是否有数据变化并执行串口发送
        ui_update_g();

        // 5. 降低频率，减轻串口压力
        osDelay(100);
    }
}
