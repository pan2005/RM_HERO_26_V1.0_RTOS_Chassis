#include <string.h>
#include "ui_interface.h"
#include "ui_g.h"

#define TOTAL_FIGURE 9
#define TOTAL_STRING 5

ui_interface_figure_t ui_g_now_figures[TOTAL_FIGURE];
uint8_t ui_g_dirty_figure[TOTAL_FIGURE];
ui_interface_string_t ui_g_now_strings[TOTAL_STRING];
uint8_t ui_g_dirty_string[TOTAL_STRING];

uint8_t ui_g_max_send_count[TOTAL_FIGURE + TOTAL_STRING] = {
    3,
    3,
    3,
    3,
    3,
    3,
    3,
    3,
    3,
    3,
    3,
    3,
    3,
    3,
};

#ifndef MANUAL_DIRTY
ui_interface_figure_t ui_g_last_figures[TOTAL_FIGURE];
ui_interface_string_t ui_g_last_strings[TOTAL_STRING];
#endif

#define SCAN_AND_SEND() ui_scan_and_send(ui_g_now_figures, ui_g_dirty_figure, ui_g_now_strings, ui_g_dirty_string, TOTAL_FIGURE, TOTAL_STRING)

void ui_init_g() {
    ui_g_Ungroup_Automode->figure_type = 6;
    ui_g_Ungroup_Automode->operate_type = 1;
    ui_g_Ungroup_Automode->layer = 0;
    ui_g_Ungroup_Automode->color = 0;
    ui_g_Ungroup_Automode->start_x = 283;
    ui_g_Ungroup_Automode->start_y = 720;
    ui_g_Ungroup_Automode->width = 2;
    ui_g_Ungroup_Automode->font_size = 20;
    ui_g_Ungroup_Automode->number = 0;

    ui_g_Ungroup_FireMode->figure_type = 6;
    ui_g_Ungroup_FireMode->operate_type = 1;
    ui_g_Ungroup_FireMode->layer = 0;
    ui_g_Ungroup_FireMode->color = 0;
    ui_g_Ungroup_FireMode->start_x = 283;
    ui_g_Ungroup_FireMode->start_y = 780;
    ui_g_Ungroup_FireMode->width = 2;
    ui_g_Ungroup_FireMode->font_size = 20;
    ui_g_Ungroup_FireMode->number = 0;

    ui_g_Ungroup_TRIG_current_number->figure_type = 6;
    ui_g_Ungroup_TRIG_current_number->operate_type = 1;
    ui_g_Ungroup_TRIG_current_number->layer = 0;
    ui_g_Ungroup_TRIG_current_number->color = 0;
    ui_g_Ungroup_TRIG_current_number->start_x = 283;
    ui_g_Ungroup_TRIG_current_number->start_y = 660;
    ui_g_Ungroup_TRIG_current_number->width = 2;
    ui_g_Ungroup_TRIG_current_number->font_size = 20;
    ui_g_Ungroup_TRIG_current_number->number = 0;

    ui_g_Ungroup_CAP_voltage_number->figure_type = 6;
    ui_g_Ungroup_CAP_voltage_number->operate_type = 1;
    ui_g_Ungroup_CAP_voltage_number->layer = 0;
    ui_g_Ungroup_CAP_voltage_number->color = 2;
    ui_g_Ungroup_CAP_voltage_number->start_x = 650;
    ui_g_Ungroup_CAP_voltage_number->start_y = 210;
    ui_g_Ungroup_CAP_voltage_number->width = 2;
    ui_g_Ungroup_CAP_voltage_number->font_size = 24;
    ui_g_Ungroup_CAP_voltage_number->number = 1234;

    ui_g_Ungroup_PWR_power_number->figure_type = 6;
    ui_g_Ungroup_PWR_power_number->operate_type = 1;
    ui_g_Ungroup_PWR_power_number->layer = 0;
    ui_g_Ungroup_PWR_power_number->color = 1;
    ui_g_Ungroup_PWR_power_number->start_x = 1270;
    ui_g_Ungroup_PWR_power_number->start_y = 210;
    ui_g_Ungroup_PWR_power_number->width = 2;
    ui_g_Ungroup_PWR_power_number->font_size = 24;
    ui_g_Ungroup_PWR_power_number->number = 12345;

    ui_g_Ungroup_CAP_arc_bg->figure_type = 4;
    ui_g_Ungroup_CAP_arc_bg->operate_type = 1;
    ui_g_Ungroup_CAP_arc_bg->layer = 0;
    ui_g_Ungroup_CAP_arc_bg->color = 8;
    ui_g_Ungroup_CAP_arc_bg->start_angle = 225;
    ui_g_Ungroup_CAP_arc_bg->end_angle = 225;
    ui_g_Ungroup_CAP_arc_bg->width = 0;
    ui_g_Ungroup_CAP_arc_bg->start_x = 960;
    ui_g_Ungroup_CAP_arc_bg->start_y = 540;
    ui_g_Ungroup_CAP_arc_bg->rx = 360;
    ui_g_Ungroup_CAP_arc_bg->ry = 360;

    ui_g_Ungroup_CAP_arc_fill->figure_type = 4;
    ui_g_Ungroup_CAP_arc_fill->operate_type = 1;
    ui_g_Ungroup_CAP_arc_fill->layer = 1;
    ui_g_Ungroup_CAP_arc_fill->color = 2;
    ui_g_Ungroup_CAP_arc_fill->start_angle = 225;
    ui_g_Ungroup_CAP_arc_fill->end_angle = 225;
    ui_g_Ungroup_CAP_arc_fill->width = 12;
    ui_g_Ungroup_CAP_arc_fill->start_x = 960;
    ui_g_Ungroup_CAP_arc_fill->start_y = 540;
    ui_g_Ungroup_CAP_arc_fill->rx = 360;
    ui_g_Ungroup_CAP_arc_fill->ry = 360;

    ui_g_Ungroup_CHASSIS_bar_bg->figure_type = 4;
    ui_g_Ungroup_CHASSIS_bar_bg->operate_type = 1;
    ui_g_Ungroup_CHASSIS_bar_bg->layer = 0;
    ui_g_Ungroup_CHASSIS_bar_bg->color = 8;
    ui_g_Ungroup_CHASSIS_bar_bg->start_angle = 135;
    ui_g_Ungroup_CHASSIS_bar_bg->end_angle = 135;
    ui_g_Ungroup_CHASSIS_bar_bg->width = 0;
    ui_g_Ungroup_CHASSIS_bar_bg->start_x = 960;
    ui_g_Ungroup_CHASSIS_bar_bg->start_y = 540;
    ui_g_Ungroup_CHASSIS_bar_bg->rx = 360;
    ui_g_Ungroup_CHASSIS_bar_bg->ry = 360;

    ui_g_Ungroup_CHASSIS_bar_fill->figure_type = 4;
    ui_g_Ungroup_CHASSIS_bar_fill->operate_type = 1;
    ui_g_Ungroup_CHASSIS_bar_fill->layer = 1;
    ui_g_Ungroup_CHASSIS_bar_fill->color = 3;
    ui_g_Ungroup_CHASSIS_bar_fill->start_angle = 135;
    ui_g_Ungroup_CHASSIS_bar_fill->end_angle = 135;
    ui_g_Ungroup_CHASSIS_bar_fill->width = 12;
    ui_g_Ungroup_CHASSIS_bar_fill->start_x = 960;
    ui_g_Ungroup_CHASSIS_bar_fill->start_y = 540;
    ui_g_Ungroup_CHASSIS_bar_fill->rx = 360;
    ui_g_Ungroup_CHASSIS_bar_fill->ry = 360;

    ui_g_Ungroup_AUTO->figure_type = 7;
    ui_g_Ungroup_AUTO->operate_type = 1;
    ui_g_Ungroup_AUTO->layer = 0;
    ui_g_Ungroup_AUTO->color = 0;
    ui_g_Ungroup_AUTO->start_x = 183;
    ui_g_Ungroup_AUTO->start_y = 721;
    ui_g_Ungroup_AUTO->width = 2;
    ui_g_Ungroup_AUTO->font_size = 20;
    ui_g_Ungroup_AUTO->str_length = 3;
    strcpy(ui_g_Ungroup_AUTO->string, "AIM");

    ui_g_Ungroup_fire->figure_type = 7;
    ui_g_Ungroup_fire->operate_type = 1;
    ui_g_Ungroup_fire->layer = 0;
    ui_g_Ungroup_fire->color = 0;
    ui_g_Ungroup_fire->start_x = 178;
    ui_g_Ungroup_fire->start_y = 781;
    ui_g_Ungroup_fire->width = 2;
    ui_g_Ungroup_fire->font_size = 20;
    ui_g_Ungroup_fire->str_length = 4;
    strcpy(ui_g_Ungroup_fire->string, "FRIC");

    ui_g_Ungroup_TRIG->figure_type = 7;
    ui_g_Ungroup_TRIG->operate_type = 1;
    ui_g_Ungroup_TRIG->layer = 0;
    ui_g_Ungroup_TRIG->color = 0;
    ui_g_Ungroup_TRIG->start_x = 178;
    ui_g_Ungroup_TRIG->start_y = 661;
    ui_g_Ungroup_TRIG->width = 2;
    ui_g_Ungroup_TRIG->font_size = 20;
    ui_g_Ungroup_TRIG->str_length = 4;
    strcpy(ui_g_Ungroup_TRIG->string, "TRIG");

    ui_g_Ungroup_CAP->figure_type = 7;
    ui_g_Ungroup_CAP->operate_type = 1;
    ui_g_Ungroup_CAP->layer = 0;
    ui_g_Ungroup_CAP->color = 2;
    ui_g_Ungroup_CAP->start_x = 640;
    ui_g_Ungroup_CAP->start_y = 860;
    ui_g_Ungroup_CAP->width = 2;
    ui_g_Ungroup_CAP->font_size = 20;
    ui_g_Ungroup_CAP->str_length = 3;
    strcpy(ui_g_Ungroup_CAP->string, "CAP");

    ui_g_Ungroup_Power->figure_type = 7;
    ui_g_Ungroup_Power->operate_type = 1;
    ui_g_Ungroup_Power->layer = 0;
    ui_g_Ungroup_Power->color = 3;
    ui_g_Ungroup_Power->start_x = 1280;
    ui_g_Ungroup_Power->start_y = 860;
    ui_g_Ungroup_Power->width = 2;
    ui_g_Ungroup_Power->font_size = 20;
    ui_g_Ungroup_Power->str_length = 3;
    strcpy(ui_g_Ungroup_Power->string, "PWR");

    uint32_t idx = 0;
    for (int i = 0; i < TOTAL_FIGURE; i++) {
        ui_g_now_figures[i].figure_name[2] = idx & 0xFF;
        ui_g_now_figures[i].figure_name[1] = (idx >> 8) & 0xFF;
        ui_g_now_figures[i].figure_name[0] = (idx >> 16) & 0xFF;
        ui_g_now_figures[i].operate_type = 1;
#ifndef MANUAL_DIRTY
        ui_g_last_figures[i] = ui_g_now_figures[i];
#endif
        ui_g_dirty_figure[i] = 1;
        idx++;
    }
    for (int i = 0; i < TOTAL_STRING; i++) {
        ui_g_now_strings[i].figure_name[2] = idx & 0xFF;
        ui_g_now_strings[i].figure_name[1] = (idx >> 8) & 0xFF;
        ui_g_now_strings[i].figure_name[0] = (idx >> 16) & 0xFF;
        ui_g_now_strings[i].operate_type = 1;
#ifndef MANUAL_DIRTY
        ui_g_last_strings[i] = ui_g_now_strings[i];
#endif
        ui_g_dirty_string[i] = 1;
        idx++;
    }

    ui_g_Ungroup_CAP_arc_bg_max_send_count = 0;
    ui_g_Ungroup_CHASSIS_bar_bg_max_send_count = 0;
    ui_g_dirty_figure[2] = 0;
    ui_g_dirty_figure[4] = 0;

    SCAN_AND_SEND();

    for (int i = 0; i < TOTAL_FIGURE; i++) {
        ui_g_now_figures[i].operate_type = 2;
    }
    for (int i = 0; i < TOTAL_STRING; i++) {
        ui_g_now_strings[i].operate_type = 2;
    }
}

void ui_update_g() {
#ifndef MANUAL_DIRTY
    for (int i = 0; i < TOTAL_FIGURE; i++) {
        if (memcmp(&ui_g_now_figures[i], &ui_g_last_figures[i], sizeof(ui_g_now_figures[i])) != 0) {
            ui_g_dirty_figure[i] = ui_g_max_send_count[i];
            ui_g_last_figures[i] = ui_g_now_figures[i];
        }
    }
    for (int i = 0; i < TOTAL_STRING; i++) {
        if (memcmp(&ui_g_now_strings[i], &ui_g_last_strings[i], sizeof(ui_g_now_strings[i])) != 0) {
            ui_g_dirty_string[i] = ui_g_max_send_count[TOTAL_FIGURE + i];
            ui_g_last_strings[i] = ui_g_now_strings[i];
        }
    }
#endif
    SCAN_AND_SEND();
}