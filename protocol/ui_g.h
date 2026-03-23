//
// Created by RM UI Designer
// Dynamic Edition
//

#ifndef UI_g_H
#define UI_g_H

#include "ui_interface.h"

extern ui_interface_figure_t ui_g_now_figures[9];
extern uint8_t ui_g_dirty_figure[9];
extern ui_interface_string_t ui_g_now_strings[5];
extern uint8_t ui_g_dirty_string[5];

extern uint8_t ui_g_max_send_count[14];

#define ui_g_Ungroup_CAP_voltage_number ((ui_interface_number_t*)&(ui_g_now_figures[0]))
#define ui_g_Ungroup_PWR_power_number ((ui_interface_number_t*)&(ui_g_now_figures[1]))
#define ui_g_Ungroup_CAP_arc_bg ((ui_interface_arc_t*)&(ui_g_now_figures[2]))
#define ui_g_Ungroup_CAP_arc_fill ((ui_interface_arc_t*)&(ui_g_now_figures[3]))
#define ui_g_Ungroup_CHASSIS_bar_bg ((ui_interface_arc_t*)&(ui_g_now_figures[4]))
#define ui_g_Ungroup_CHASSIS_bar_fill ((ui_interface_arc_t*)&(ui_g_now_figures[5]))
#define ui_g_Ungroup_Automode ((ui_interface_number_t*)&(ui_g_now_figures[6]))
#define ui_g_Ungroup_FireMode ((ui_interface_number_t*)&(ui_g_now_figures[7]))
#define ui_g_Ungroup_TRIG_current_number ((ui_interface_number_t*)&(ui_g_now_figures[8]))

#define ui_g_Ungroup_CAP (&(ui_g_now_strings[0]))
#define ui_g_Ungroup_Power (&(ui_g_now_strings[1]))
#define ui_g_Ungroup_AUTO (&(ui_g_now_strings[2]))
#define ui_g_Ungroup_fire (&(ui_g_now_strings[3]))
#define ui_g_Ungroup_TRIG (&(ui_g_now_strings[4]))

#define ui_g_Ungroup_CAP_voltage_number_max_send_count (ui_g_max_send_count[0])
#define ui_g_Ungroup_PWR_power_number_max_send_count (ui_g_max_send_count[1])
#define ui_g_Ungroup_CAP_arc_bg_max_send_count (ui_g_max_send_count[2])
#define ui_g_Ungroup_CAP_arc_fill_max_send_count (ui_g_max_send_count[3])
#define ui_g_Ungroup_CHASSIS_bar_bg_max_send_count (ui_g_max_send_count[4])
#define ui_g_Ungroup_CHASSIS_bar_fill_max_send_count (ui_g_max_send_count[5])
#define ui_g_Ungroup_Automode_max_send_count (ui_g_max_send_count[6])
#define ui_g_Ungroup_FireMode_max_send_count (ui_g_max_send_count[7])
#define ui_g_Ungroup_TRIG_current_number_max_send_count (ui_g_max_send_count[8])

#define ui_g_Ungroup_CAP_max_send_count (ui_g_max_send_count[9])
#define ui_g_Ungroup_Power_max_send_count (ui_g_max_send_count[10])
#define ui_g_Ungroup_AUTO_max_send_count (ui_g_max_send_count[11])
#define ui_g_Ungroup_fire_max_send_count (ui_g_max_send_count[12])
#define ui_g_Ungroup_TRIG_max_send_count (ui_g_max_send_count[13])

#ifdef MANUAL_DIRTY
#define ui_g_Ungroup_CAP_voltage_number_dirty (ui_g_dirty_figure[0])
#define ui_g_Ungroup_PWR_power_number_dirty (ui_g_dirty_figure[1])
#define ui_g_Ungroup_CAP_arc_bg_dirty (ui_g_dirty_figure[2])
#define ui_g_Ungroup_CAP_arc_fill_dirty (ui_g_dirty_figure[3])
#define ui_g_Ungroup_CHASSIS_bar_bg_dirty (ui_g_dirty_figure[4])
#define ui_g_Ungroup_CHASSIS_bar_fill_dirty (ui_g_dirty_figure[5])
#define ui_g_Ungroup_Automode_dirty (ui_g_dirty_figure[6])
#define ui_g_Ungroup_FireMode_dirty (ui_g_dirty_figure[7])
#define ui_g_Ungroup_TRIG_current_number_dirty (ui_g_dirty_figure[8])

#define ui_g_Ungroup_CAP_dirty (ui_g_dirty_string[0])
#define ui_g_Ungroup_Power_dirty (ui_g_dirty_string[1])
#define ui_g_Ungroup_AUTO_dirty (ui_g_dirty_string[2])
#define ui_g_Ungroup_fire_dirty (ui_g_dirty_string[3])
#define ui_g_Ungroup_TRIG_dirty (ui_g_dirty_string[4])
#endif

void ui_init_g();
void ui_update_g();

#endif // UI_g_H
