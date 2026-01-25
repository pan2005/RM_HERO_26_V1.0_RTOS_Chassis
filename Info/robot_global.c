
#include "robot_global.h"
#include "bsp_can.h"
#include <string.h>
#include "bsp_delay.h"

robot_ctrl_info_t robot_ctrl;

/**
 * @brief 全局控制变量初始化
 * @note  在系统上电时调用，确保所有模式初始为安全状态
 */
void Robot_Global_Init(control_mode_e control_mode_p) {
    // 结构体整体清零 (将所有浮点数置0，指针置空)
    //  memset(&robot_ctrl, 0, sizeof(robot_ctrl_info_t));
    BSP_CAN_Init();

    delay_init();

    // 初始模式设置
    robot_ctrl.gimbal_mode  = GIMBAL_RELAX;
    robot_ctrl.chassis_mode = CHASSIS_UNABLE;
    robot_ctrl.shoot_mode   = SHOOT_STOP;
    robot_ctrl.control_mode = control_mode_p;

    // 关联遥控器句柄 (需要确保 RC_get_handle 返回的是包含遥控器数据的静态指针)

    // 状态标志显式初始化
    robot_ctrl.monitor.sensor_ready = 0;
    robot_ctrl.monitor.remote_online = 0;
}



void Robot_Global_Update(void) {     //这里存储的是由传感器获取的传感器数据
}

// void Robot_Global_target_Update(void) {
//     if (robot_ctrl.control_mode == TUCHUAN_CONTROL) {
//
//         robot_ctrl.target.vx = robot_ctrl.tuchuan->rc.ch[2];
//         robot_ctrl.target.vy = robot_ctrl.tuchuan->rc.ch[3];
//         robot_ctrl.target.yaw += robot_ctrl.tuchuan->rc.ch[0] / 6600.f / 10.0f;
//         robot_ctrl.target.pitch += robot_ctrl.tuchuan->rc.ch[1] / 6600.f / 10.0f;
//         robot_ctrl.shoot_gear = robot_ctrl.tuchuan->rc.sw;
//         robot_ctrl.fire = robot_ctrl.tuchuan->rc.trigger;
//
//     }
//     else {
//         if (switch_is_mid(robot_ctrl.rc_handle->rc.s[1])) {
//
//         }
//
//     }
// }