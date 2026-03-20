
#include "robot_global.h"
#include "bsp_can.h"
#include <string.h>
#include "bsp_delay.h"
#include "Supercapacitor.h"

robot_ctrl_info_t robot_ctrl;

/**
 * @brief 全局控制变量初始化
 * @note  在系统上电时调用，确保所有模式初始为安全状态
 */
void Robot_Global_Init(control_mode_e control_mode_p) {
    // 结构体整体清零 (将所有浮点数置0，指针置空)
    //  memset(&robot_ctrl, 0, sizeof(robot_ctrl_info_t));
    BSP_CAN_Init();
    Super_cap_init(&super_cap_return_pack);

    delay_init();

    robot_ctrl.chassis_mode = CHASSIS_FOLLOW;
}



void Robot_Global_Update(void) {     //这里存储的是由传感器获取的传感器数据
}

