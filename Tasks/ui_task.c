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

void UI_Task(void *argument) {
    uint32_t string_resend_elapsed_ms = UI_STRING_RESEND_PERIOD_MS;

    // 1. 等待裁判系统稳定
    osDelay(4000);

    // 2. 初始化静态界面 (ui_g.c 里的函数)
    ui_init_g();

    while(1) {
        // 3. 实时更新电控数据到 UI 结构体
        // 例如：把超级电容百分比传给那个 CAP 元件
     //   strcpy(ui_g_Ungroup_AUTO->string, "AUTO");
        ui_g_Ungroup_CAP_energy->number = (int32_t)super_cap_return_pack.Capacity_Voltage;
        ui_g_Ungroup_Automode->number = (int32_t)robot_ctrl.chassis_mode;
        ui_g_Ungroup_TH_current_number->number = thumbwheel_motor.measure.given_current;
        ui_g_Ungroup_FireMode->number = (int32_t)robot_ctrl.shoot_gear;

        // 4. 检查是否有数据变化并执行串口发送
        ui_update_g();

        // 5. 降低频率，减轻串口压力
        osDelay(UI_UPDATE_PERIOD_MS);
        string_resend_elapsed_ms += UI_UPDATE_PERIOD_MS;
    }
}
