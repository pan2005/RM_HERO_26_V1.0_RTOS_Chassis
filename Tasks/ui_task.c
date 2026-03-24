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

#define CAP_VOLTAGE_MIN 6.0f
#define CAP_VOLTAGE_MAX 27.5f
#define CAP_VOLTAGE_YELLOW 15.0f
#define CAP_VOLTAGE_GREEN 20.0f
#define CAP_ARC_START_ANGLE 225
#define CAP_ARC_TOTAL_ANGLE 90
#define CAP_ARC_FILL_WIDTH 12
#define CHASSIS_POWER_ARC_TOP_ANGLE 45
#define CHASSIS_POWER_ARC_BOTTOM_ANGLE 135
#define CHASSIS_POWER_ARC_TOTAL_ANGLE (CHASSIS_POWER_ARC_BOTTOM_ANGLE - CHASSIS_POWER_ARC_TOP_ANGLE)
#define SUPER_CAP_VOLTAGE_SCALE 100.0f
#define SUPER_CAP_POWER_SCALE 10.0f
#define CHASSIS_POWER_UI_MAX 300.0f
#define UI_STRING_RESEND_PERIOD_MS 2000U
#define UI_UPDATE_PERIOD_MS 200U

static float clamp_float(float value, float min_value, float max_value)
{
    if (value < min_value) {
        return min_value;
    }

    if (value > max_value) {
        return max_value;
    }

    return value;
}

static float cap_ratio_from_voltage(float voltage)
{
    float clamped_voltage = clamp_float(voltage, CAP_VOLTAGE_MIN, CAP_VOLTAGE_MAX);
    const float min_sq = CAP_VOLTAGE_MIN * CAP_VOLTAGE_MIN;
    const float max_sq = CAP_VOLTAGE_MAX * CAP_VOLTAGE_MAX;

    return clamp_float((clamped_voltage * clamped_voltage - min_sq) / (max_sq - min_sq), 0.0f, 1.0f);
}

static int cap_arc_end_angle_from_voltage(float voltage)
{
    if (voltage <= CAP_VOLTAGE_MIN) {
        return CAP_ARC_START_ANGLE;
    }

    int end_angle = CAP_ARC_START_ANGLE + (int)(cap_ratio_from_voltage(voltage) * CAP_ARC_TOTAL_ANGLE + 0.5f);

    if ((voltage > CAP_VOLTAGE_MIN) && (end_angle == CAP_ARC_START_ANGLE)) {
        end_angle = CAP_ARC_START_ANGLE + 2;
    }

    return end_angle;
}

static int chassis_power_arc_start_angle_from_power(float chassis_power)
{
    float ratio;
    int start_angle;

    if (chassis_power <= 0.0f) {
        return CHASSIS_POWER_ARC_BOTTOM_ANGLE;
    }

    ratio = clamp_float(chassis_power / CHASSIS_POWER_UI_MAX, 0.0f, 1.0f);
    start_angle = CHASSIS_POWER_ARC_BOTTOM_ANGLE - (int)(ratio * CHASSIS_POWER_ARC_TOTAL_ANGLE + 0.5f);

    if (start_angle >= CHASSIS_POWER_ARC_BOTTOM_ANGLE) {
        start_angle = CHASSIS_POWER_ARC_BOTTOM_ANGLE - 2;
    }

    if (start_angle < CHASSIS_POWER_ARC_TOP_ANGLE) {
        start_angle = CHASSIS_POWER_ARC_TOP_ANGLE;
    }

    return start_angle;
}

static uint32_t cap_color_from_voltage(float voltage)
{
    if (voltage > CAP_VOLTAGE_GREEN) {
        return 2U;
    }

    if (voltage >= CAP_VOLTAGE_YELLOW) {
        return 1U;
    }

    return 3U;
}

static float ui_cap_voltage_from_feedback(void)
{
    return super_cap_return_pack.Capacity_Voltage / SUPER_CAP_VOLTAGE_SCALE;
}

static float ui_chassis_power_from_feedback(void)
{
    int32_t chassis_power_x10 = super_cap_return_pack.Chassis_output;

    if (chassis_power_x10 < 0) {
        chassis_power_x10 = -chassis_power_x10;
    }

    return chassis_power_x10 / SUPER_CAP_POWER_SCALE;
}

void UI_Task(void *argument) {
    uint32_t string_resend_elapsed_ms = UI_STRING_RESEND_PERIOD_MS;

    // 1. 等待裁判系统稳定
    osDelay(4000);

    // 2. 初始化静态界面 (ui_g.c 里的函数)
    ui_init_g();

    while(1) {
        // 3. 实时更新电控数据到 UI 结构体
        // 参考哨兵工程的数据框架，电容电量和底盘功率优先使用同一条电容反馈链路。
        float cap_voltage = ui_cap_voltage_from_feedback();
        float chassis_power = ui_chassis_power_from_feedback();
        uint32_t cap_color = cap_color_from_voltage(cap_voltage);
        int32_t aim_enabled = (robot_ctrl.gimbal_mode == GIMBAL_AUTO) ? 11 : 0;
        int32_t friction_enabled = (robot_ctrl.shoot_gear != 0U) ? 1 : 0;
        int32_t cap_voltage_display = (int32_t)(cap_voltage * 100.0f + 0.5f);
        int32_t trigger_current_display = thumbwheel_motor.measure.given_current;

        if (trigger_current_display < 0) {
            trigger_current_display = -trigger_current_display;
        }

        ui_g_Ungroup_Automode->number = aim_enabled;
        ui_g_Ungroup_FireMode->number = friction_enabled;
        ui_g_Ungroup_TRIG_current_number->number = trigger_current_display;
        ui_g_Ungroup_CAP_voltage_number->number = cap_voltage_display;
        ui_g_Ungroup_PWR_power_number->number = (int32_t)(chassis_power + 0.5f);
        ui_g_Ungroup_CAP_voltage_number->color = cap_color;
        ui_g_Ungroup_CAP_arc_fill->color = cap_color;
        if (cap_voltage <= CAP_VOLTAGE_MIN) {
            ui_g_Ungroup_CAP_arc_fill->width = 0;
            ui_g_Ungroup_CAP_arc_fill->end_angle = CAP_ARC_START_ANGLE;
        } else {
            ui_g_Ungroup_CAP_arc_fill->width = CAP_ARC_FILL_WIDTH;
            ui_g_Ungroup_CAP_arc_fill->end_angle = cap_arc_end_angle_from_voltage(cap_voltage);
        }
        ui_g_Ungroup_CHASSIS_bar_fill->color = 3U;
        ui_g_Ungroup_CHASSIS_bar_fill->start_angle = chassis_power_arc_start_angle_from_power(chassis_power);
        ui_g_Ungroup_CHASSIS_bar_fill->end_angle = CHASSIS_POWER_ARC_BOTTOM_ANGLE;
        ui_g_dirty_figure[0] = ui_g_Ungroup_CAP_voltage_number_max_send_count;
        ui_g_dirty_figure[1] = ui_g_Ungroup_PWR_power_number_max_send_count;
        ui_g_dirty_figure[3] = ui_g_Ungroup_CAP_arc_fill_max_send_count;
        ui_g_dirty_figure[5] = ui_g_Ungroup_CHASSIS_bar_fill_max_send_count;
        ui_g_dirty_figure[7] = ui_g_Ungroup_FireMode_max_send_count;
        ui_g_dirty_figure[8] = ui_g_Ungroup_TRIG_current_number_max_send_count;

        if (string_resend_elapsed_ms >= UI_STRING_RESEND_PERIOD_MS) {
            ui_g_dirty_string[0] = ui_g_Ungroup_CAP_max_send_count;
            ui_g_dirty_string[1] = ui_g_Ungroup_Power_max_send_count;
            ui_g_dirty_string[2] = ui_g_Ungroup_AUTO_max_send_count;
            ui_g_dirty_string[3] = ui_g_Ungroup_fire_max_send_count;
            ui_g_dirty_string[4] = ui_g_Ungroup_TRIG_max_send_count;
            string_resend_elapsed_ms = 0U;
        }

        // 4. 检查是否有数据变化并执行串口发送
        ui_update_g();

        // 5. 降低频率，减轻串口压力
        osDelay(UI_UPDATE_PERIOD_MS);
        string_resend_elapsed_ms += UI_UPDATE_PERIOD_MS;
    }
}
