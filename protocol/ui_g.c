//
// Created by RM UI Designer
// Dynamic Edition
//

#include "string.h"
#include "ui_interface.h"
#include "ui_g.h"
#include "stdlib.h"

#define TOTAL_FIGURE 4
#define TOTAL_STRING 4

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
    ui_g_Ungroup_Automode->start_y = 804;
    ui_g_Ungroup_Automode->width = 2;
    ui_g_Ungroup_Automode->font_size = 20;
    ui_g_Ungroup_Automode->number = 12345;

    ui_g_Ungroup_FireMode->figure_type = 6;
    ui_g_Ungroup_FireMode->operate_type = 1;
    ui_g_Ungroup_FireMode->layer = 0;
    ui_g_Ungroup_FireMode->color = 0;
    ui_g_Ungroup_FireMode->start_x = 283;
    ui_g_Ungroup_FireMode->start_y = 756;
    ui_g_Ungroup_FireMode->width = 2;
    ui_g_Ungroup_FireMode->font_size = 20;
    ui_g_Ungroup_FireMode->number = 12345;

    ui_g_Ungroup_CAP_energy->figure_type = 6;
    ui_g_Ungroup_CAP_energy->operate_type = 1;
    ui_g_Ungroup_CAP_energy->layer = 0;
    ui_g_Ungroup_CAP_energy->color = 0;
    ui_g_Ungroup_CAP_energy->start_x = 284;
    ui_g_Ungroup_CAP_energy->start_y = 708;
    ui_g_Ungroup_CAP_energy->width = 2;
    ui_g_Ungroup_CAP_energy->font_size = 20;
    ui_g_Ungroup_CAP_energy->number = 12345;

    ui_g_Ungroup_TH_current_number->figure_type = 6;
    ui_g_Ungroup_TH_current_number->operate_type = 1;
    ui_g_Ungroup_TH_current_number->layer = 0;
    ui_g_Ungroup_TH_current_number->color = 0;
    ui_g_Ungroup_TH_current_number->start_x = 392;
    ui_g_Ungroup_TH_current_number->start_y = 662;
    ui_g_Ungroup_TH_current_number->width = 2;
    ui_g_Ungroup_TH_current_number->font_size = 20;
    ui_g_Ungroup_TH_current_number->number = 12345;

    ui_g_Ungroup_AUTO->figure_type = 7;
    ui_g_Ungroup_AUTO->operate_type = 1;
    ui_g_Ungroup_AUTO->layer = 0;
    ui_g_Ungroup_AUTO->color = 0;
    ui_g_Ungroup_AUTO->start_x = 175;
    ui_g_Ungroup_AUTO->start_y = 805;
    ui_g_Ungroup_AUTO->width = 2;
    ui_g_Ungroup_AUTO->font_size = 20;
    ui_g_Ungroup_AUTO->str_length = 4;
    strcpy(ui_g_Ungroup_AUTO->string, "AUT");

    ui_g_Ungroup_fire->figure_type = 7;
    ui_g_Ungroup_fire->operate_type = 1;
    ui_g_Ungroup_fire->layer = 0;
    ui_g_Ungroup_fire->color = 0;
    ui_g_Ungroup_fire->start_x = 178;
    ui_g_Ungroup_fire->start_y = 756;
    ui_g_Ungroup_fire->width = 2;
    ui_g_Ungroup_fire->font_size = 20;
    ui_g_Ungroup_fire->str_length = 4;
    strcpy(ui_g_Ungroup_fire->string, "FIR");

    ui_g_Ungroup_CAP->figure_type = 7;
    ui_g_Ungroup_CAP->operate_type = 1;
    ui_g_Ungroup_CAP->layer = 0;
    ui_g_Ungroup_CAP->color = 0;
    ui_g_Ungroup_CAP->start_x = 180;
    ui_g_Ungroup_CAP->start_y = 708;
    ui_g_Ungroup_CAP->width = 2;
    ui_g_Ungroup_CAP->font_size = 20;
    ui_g_Ungroup_CAP->str_length = 3;
    strcpy(ui_g_Ungroup_CAP->string, "CAP");

    ui_g_Ungroup_TH_current->figure_type = 7;
    ui_g_Ungroup_TH_current->operate_type = 1;
    ui_g_Ungroup_TH_current->layer = 0;
    ui_g_Ungroup_TH_current->color = 0;
    ui_g_Ungroup_TH_current->start_x = 177;
    ui_g_Ungroup_TH_current->start_y = 662;
    ui_g_Ungroup_TH_current->width = 2;
    ui_g_Ungroup_TH_current->font_size = 20;
    ui_g_Ungroup_TH_current->str_length = 10;
    strcpy(ui_g_Ungroup_TH_current->string, "TH_CURRENT");

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
