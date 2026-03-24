//
// Created by bismarckkk on 2025/3/22.
// Dynamic Edition
//

#include <string.h>
#include <stdio.h>
#include "ui_interface.h"

#include "bsp_usart.h"
#include "cmsis_os2.h"
#include "main.h"
#include "usart.h"
uint8_t seq = 0;
int ui_self_id = 1;

static uint16_t ui_client_id_from_robot_id(uint16_t robot_id)
{
    if ((robot_id >= 1U && robot_id <= 7U) || (robot_id >= 101U && robot_id <= 107U)) {
        return robot_id + 256U;
    }

    return 0x0101U;
}

ui_string_frame_t _ui_string_frame;
ui_1_frame_t _ui_1_frame;
ui_2_frame_t _ui_2_frame;
ui_5_frame_t _ui_5_frame;
ui_7_frame_t _ui_7_frame;



void print_message(const uint8_t *message, const int length) {
    uint32_t wait_timeout = 20; // 设定 20 次重试
    while (huart6.gState != HAL_UART_STATE_READY && wait_timeout > 0) {
        osDelay(1);
        wait_timeout--;
    }

    // 2. 如果等了 20ms 还是 BUSY，说明硬件状态机崩溃了！
    if (huart6.gState != HAL_UART_STATE_READY) {
        // 【强制重置】终止 DMA 并强行把状态机掰正
        HAL_UART_AbortTransmit(&huart6);
        huart6.gState = HAL_UART_STATE_READY;
        huart6.hdmatx->State = HAL_DMA_STATE_READY;
        __HAL_UNLOCK(&huart6); // 强行解锁
    }

    // 3. 错误标志清除（防止溢出导致 DMA 拒发）
    if (__HAL_UART_GET_FLAG(&huart6, UART_FLAG_ORE) != RESET) {
        __HAL_UART_CLEAR_OREFLAG(&huart6);
    }

    // 4. 发送数据
    HAL_UART_Transmit_DMA(&huart6, (uint8_t*)message, length);
    osDelay(5);
}

const unsigned char CRC8_TAB[256] = {
    0x00, 0x5e, 0xbc, 0xe2, 0x61, 0x3f, 0xdd, 0x83, 0xc2, 0x9c, 0x7e, 0x20, 0xa3, 0xfd, 0x1f, 0x41,
    0x9d, 0xc3, 0x21, 0x7f, 0xfc, 0xa2, 0x40, 0x1e, 0x5f, 0x01, 0xe3, 0xbd, 0x3e, 0x60, 0x82, 0xdc, 0x23,
    0x7d, 0x9f, 0xc1, 0x42, 0x1c, 0xfe, 0xa0, 0xe1, 0xbf, 0x5d, 0x03, 0x80, 0xde, 0x3c, 0x62, 0xbe, 0xe0,
    0x02, 0x5c, 0xdf, 0x81, 0x63, 0x3d, 0x7c, 0x22, 0xc0, 0x9e, 0x1d, 0x43, 0xa1, 0xff, 0x46, 0x18, 0xfa,
    0xa4, 0x27, 0x79, 0x9b, 0xc5, 0x84, 0xda, 0x38, 0x66, 0xe5, 0xbb, 0x59, 0x07, 0xdb, 0x85, 0x67,
    0x39, 0xba, 0xe4, 0x06, 0x58, 0x19, 0x47, 0xa5, 0xfb, 0x78, 0x26, 0xc4, 0x9a, 0x65, 0x3b, 0xd9, 0x87,
    0x04, 0x5a, 0xb8, 0xe6, 0xa7, 0xf9, 0x1b, 0x45, 0xc6, 0x98, 0x7a, 0x24, 0xf8, 0xa6, 0x44, 0x1a, 0x99,
    0xc7, 0x25, 0x7b, 0x3a, 0x64, 0x86, 0xd8, 0x5b, 0x05, 0xe7, 0xb9,
    0x8c, 0xd2, 0x30, 0x6e, 0xed, 0xb3, 0x51, 0x0f, 0x4e, 0x10, 0xf2, 0xac, 0x2f, 0x71, 0x93, 0xcd, 0x11,
    0x4f, 0xad, 0xf3, 0x70, 0x2e, 0xcc, 0x92, 0xd3, 0x8d, 0x6f, 0x31, 0xb2, 0xec, 0x0e, 0x50, 0xaf, 0xf1,
    0x13, 0x4d, 0xce, 0x90, 0x72, 0x2c, 0x6d, 0x33, 0xd1, 0x8f, 0x0c, 0x52, 0xb0, 0xee, 0x32, 0x6c, 0x8e,
    0xd0, 0x53, 0x0d, 0xef, 0xb1, 0xf0, 0xae, 0x4c, 0x12, 0x91, 0xcf, 0x2d, 0x73, 0xca, 0x94, 0x76, 0x28,
    0xab, 0xf5, 0x17, 0x49, 0x08, 0x56, 0xb4, 0xea, 0x69, 0x37, 0xd5, 0x8b, 0x57, 0x09, 0xeb, 0xb5,
    0x36, 0x68, 0x8a, 0xd4, 0x95, 0xcb, 0x29, 0x77, 0xf4, 0xaa, 0x48, 0x16, 0xe9, 0xb7, 0x55, 0x0b, 0x88,
    0xd6, 0x34, 0x6a, 0x2b, 0x75, 0x97, 0xc9, 0x4a, 0x14, 0xf6, 0xa8,
    0x74, 0x2a, 0xc8, 0x96, 0x15, 0x4b, 0xa9, 0xf7, 0xb6, 0xe8, 0x0a, 0x54, 0xd7, 0x89, 0x6b, 0x35,
};

unsigned char calc_crc8(unsigned char *pchMessage, unsigned int dwLength) {
    unsigned char ucCRC8 = 0xff;
    unsigned char ucIndex;
    while (dwLength--) {
        ucIndex = ucCRC8 ^ (*pchMessage++);
        ucCRC8 = CRC8_TAB[ucIndex];
    }
    return (ucCRC8);
}

const uint16_t wCRC_Table[256] = {
    0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf,
    0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7,
    0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e,
    0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876,
    0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd,
    0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5,
    0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c,
    0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974,
    0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb,
    0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3,
    0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a,
    0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72,
    0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9,
    0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1,
    0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738,
    0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70,
    0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7,
    0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff,
    0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036,
    0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e,
    0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5,
    0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd,
    0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134,
    0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c,
    0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3,
    0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb,
    0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232,
    0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a,
    0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1,
    0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9,
    0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330,
    0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78
};

uint16_t calc_crc16(uint8_t *pchMessage, uint32_t dwLength)
{
    uint16_t wCRC = 0xffff;
    uint8_t chData;
    if (pchMessage == NULL)
    {
        return 0xFFFF;
    }
    while(dwLength--)
    {
        chData = *pchMessage++;
        (wCRC) = ((uint16_t)(wCRC) >> 8) ^ wCRC_Table[((uint16_t)(wCRC) ^ (uint16_t)(chData)) & 0x00ff];
    }
    return wCRC;
}

#define UI_FRAME_HEADER_SIZE 5U
#define UI_CMD_ID_SIZE 2U
#define UI_DATA_HEADER_SIZE 6U
#define UI_FRAME_TAIL_SIZE 2U
#define UI_GRAPHIC_SIZE 15U
#define UI_STRING_DATA_SIZE 30U
#define UI_FIGURE_FRAME_MAX_SIZE (UI_FRAME_HEADER_SIZE + UI_CMD_ID_SIZE + UI_DATA_HEADER_SIZE + UI_GRAPHIC_SIZE * 7U + UI_FRAME_TAIL_SIZE)
#define UI_STRING_FRAME_SIZE (UI_FRAME_HEADER_SIZE + UI_CMD_ID_SIZE + UI_DATA_HEADER_SIZE + UI_GRAPHIC_SIZE + UI_STRING_DATA_SIZE + UI_FRAME_TAIL_SIZE)
#define UI_DELETE_FRAME_SIZE (UI_FRAME_HEADER_SIZE + UI_CMD_ID_SIZE + UI_DATA_HEADER_SIZE + 2U + UI_FRAME_TAIL_SIZE)
#define UI_MAX_FIGURE_PACKETS_PER_UPDATE 2U
#define UI_FIGURE_PACKET_DELAY_MS 30U

static uint8_t ui_figure_tx_buf[UI_FIGURE_FRAME_MAX_SIZE];
static uint8_t ui_string_tx_buf[UI_STRING_FRAME_SIZE];
static uint8_t ui_delete_tx_buf[UI_DELETE_FRAME_SIZE];

static void pack_frame_header(uint8_t *buf, uint16_t data_length)
{
    buf[0] = 0xA5;
    buf[1] = (uint8_t)(data_length & 0xFFU);
    buf[2] = (uint8_t)((data_length >> 8) & 0xFFU);
    buf[3] = seq++;
    buf[4] = calc_crc8(buf, 4);
}

static void pack_interactive_header(uint8_t *buf, uint16_t data_cmd_id)
{
    uint16_t client_id = ui_client_id_from_robot_id((uint16_t)ui_self_id);

    buf[0] = (uint8_t)(0x0301U & 0xFFU);
    buf[1] = (uint8_t)((0x0301U >> 8) & 0xFFU);
    buf[2] = (uint8_t)(data_cmd_id & 0xFFU);
    buf[3] = (uint8_t)((data_cmd_id >> 8) & 0xFFU);
    buf[4] = (uint8_t)(ui_self_id & 0xFFU);
    buf[5] = (uint8_t)((ui_self_id >> 8) & 0xFFU);
    buf[6] = (uint8_t)(client_id & 0xFFU);
    buf[7] = (uint8_t)((client_id >> 8) & 0xFFU);
}

static void finalize_and_send_frame(uint8_t *buf, uint16_t total_length)
{
    uint16_t crc16 = calc_crc16(buf, total_length - UI_FRAME_TAIL_SIZE);

    buf[total_length - 2U] = (uint8_t)(crc16 & 0xFFU);
    buf[total_length - 1U] = (uint8_t)((crc16 >> 8) & 0xFFU);
    SEND_MESSAGE(buf, total_length);
}

static void pack_figure15(uint8_t out[UI_GRAPHIC_SIZE], const ui_interface_figure_t *figure)
{
    uint32_t cfg1;
    uint32_t cfg2;
    uint32_t cfg3;

    memcpy(&out[0], figure->figure_name, 3);

    cfg1 = ((uint32_t)(figure->operate_type & 0x07U)) |
           (((uint32_t)(figure->figure_type & 0x07U)) << 3) |
           (((uint32_t)(figure->layer & 0x0FU)) << 6) |
           (((uint32_t)(figure->color & 0x0FU)) << 10) |
           (((uint32_t)(figure->_a & 0x01FFU)) << 14) |
           (((uint32_t)(figure->_b & 0x01FFU)) << 23);

    cfg2 = ((uint32_t)(figure->width & 0x03FFU)) |
           (((uint32_t)(figure->start_x & 0x07FFU)) << 10) |
           (((uint32_t)(figure->start_y & 0x07FFU)) << 21);

    cfg3 = ((uint32_t)(figure->_c & 0x03FFU)) |
           (((uint32_t)(figure->_d & 0x07FFU)) << 10) |
           (((uint32_t)(figure->_e & 0x07FFU)) << 21);

    out[3] = (uint8_t)(cfg1 & 0xFFU);
    out[4] = (uint8_t)((cfg1 >> 8) & 0xFFU);
    out[5] = (uint8_t)((cfg1 >> 16) & 0xFFU);
    out[6] = (uint8_t)((cfg1 >> 24) & 0xFFU);
    out[7] = (uint8_t)(cfg2 & 0xFFU);
    out[8] = (uint8_t)((cfg2 >> 8) & 0xFFU);
    out[9] = (uint8_t)((cfg2 >> 16) & 0xFFU);
    out[10] = (uint8_t)((cfg2 >> 24) & 0xFFU);
    out[11] = (uint8_t)(cfg3 & 0xFFU);
    out[12] = (uint8_t)((cfg3 >> 8) & 0xFFU);
    out[13] = (uint8_t)((cfg3 >> 16) & 0xFFU);
    out[14] = (uint8_t)((cfg3 >> 24) & 0xFFU);
}

static void pack_number15(uint8_t out[UI_GRAPHIC_SIZE], const ui_interface_number_t *number)
{
    ui_interface_figure_t packed = {0};

    memcpy(packed.figure_name, number->figure_name, 3);
    packed.operate_type = number->operate_type;
    packed.figure_type = number->figure_type;
    packed.layer = number->layer;
    packed.color = number->color;
    packed._a = number->font_size;
    packed._b = 0U;
    packed.width = number->width;
    packed.start_x = number->start_x;
    packed.start_y = number->start_y;
    packed._c = (uint32_t)number->number & 0x03FFU;
    packed._d = ((uint32_t)number->number >> 10) & 0x07FFU;
    packed._e = ((uint32_t)number->number >> 21) & 0x07FFU;
    pack_figure15(out, &packed);
}

static void pack_string15(uint8_t out[UI_GRAPHIC_SIZE], const ui_interface_string_t *string)
{
    ui_interface_figure_t packed = {0};

    memcpy(packed.figure_name, string->figure_name, 3);
    packed.operate_type = string->operate_type;
    packed.figure_type = string->figure_type;
    packed.layer = string->layer;
    packed.color = string->color;
    packed._a = string->font_size;
    packed._b = string->str_length;
    packed.width = string->width;
    packed.start_x = string->start_x;
    packed.start_y = string->start_y;
    pack_figure15(out, &packed);
}

static void send_figure_frame(const ui_interface_figure_t *figures, uint8_t count, uint16_t data_cmd_id)
{
    uint8_t *frame = ui_figure_tx_buf;
    uint16_t data_length = UI_DATA_HEADER_SIZE + (uint16_t)UI_GRAPHIC_SIZE * count;
    uint16_t total_length = UI_FRAME_HEADER_SIZE + UI_CMD_ID_SIZE + data_length + UI_FRAME_TAIL_SIZE;

    memset(frame, 0, total_length);

    pack_frame_header(frame, data_length);
    pack_interactive_header(&frame[UI_FRAME_HEADER_SIZE], data_cmd_id);

    for (uint8_t i = 0; i < count; i++) {
        if (figures[i].figure_type == 6U) {
            pack_number15(&frame[13U + i * UI_GRAPHIC_SIZE], (const ui_interface_number_t *)&figures[i]);
        } else {
            pack_figure15(&frame[13U + i * UI_GRAPHIC_SIZE], &figures[i]);
        }
    }

    finalize_and_send_frame(frame, total_length);
}

static uint16_t ui_figure_cmd_id_from_count(uint8_t count)
{
    if (count >= 7U) {
        return 0x0104U;
    }

    if (count >= 5U) {
        return 0x0103U;
    }

    if (count >= 2U) {
        return 0x0102U;
    }

    return 0x0101U;
}

static uint8_t ui_figure_pack_size_from_remain(int remain_size)
{
    if (remain_size >= 7) {
        return 7U;
    }

    if (remain_size >= 5) {
        return 5U;
    }

    if (remain_size >= 2) {
        return 2U;
    }

    return 1U;
}

static void send_string_frame(const ui_interface_string_t *string)
{
    uint8_t *frame = ui_string_tx_buf;
    uint16_t data_length = UI_DATA_HEADER_SIZE + UI_GRAPHIC_SIZE + UI_STRING_DATA_SIZE;
    uint16_t total_length = UI_FRAME_HEADER_SIZE + UI_CMD_ID_SIZE + data_length + UI_FRAME_TAIL_SIZE;

    memset(frame, 0, total_length);

    pack_frame_header(frame, data_length);
    pack_interactive_header(&frame[UI_FRAME_HEADER_SIZE], 0x0110U);
    pack_string15(&frame[13], string);
    memcpy(&frame[28], string->string, UI_STRING_DATA_SIZE);
    finalize_and_send_frame(frame, total_length);
}

static void send_delete_frame(uint8_t delete_type, uint8_t layer)
{
    uint8_t *frame = ui_delete_tx_buf;
    uint16_t data_length = UI_DATA_HEADER_SIZE + 2U;
    uint16_t total_length = UI_FRAME_HEADER_SIZE + UI_CMD_ID_SIZE + data_length + UI_FRAME_TAIL_SIZE;

    memset(frame, 0, total_length);

    pack_frame_header(frame, data_length);
    pack_interactive_header(&frame[UI_FRAME_HEADER_SIZE], 0x0100U);
    frame[13] = delete_type;
    frame[14] = layer;
    finalize_and_send_frame(frame, total_length);
}

#define DEFINE_FRAME_PROC(num, id)                          \
void ui_proc_ ## num##_frame(ui_ ## num##_frame_t *msg) {   \
    msg->header.SOF = 0xA5;                                 \
    msg->header.length = 6 + 15 * num;                      \
    msg->header.seq = seq++;                                \
    msg->header.crc8 = calc_crc8((uint8_t*)msg, 4);        \
    msg->header.cmd_id = 0x0301;                            \
    msg->header.sub_id = id;                                \
    msg->header.send_id = ui_self_id;                       \
    msg->header.recv_id = ui_self_id + 256;                 \
    msg->crc16 = calc_crc16((uint8_t*)msg, 13 + 15 * num); \
}

DEFINE_FRAME_PROC(1, 0x0101)
DEFINE_FRAME_PROC(2, 0x0102)
DEFINE_FRAME_PROC(5, 0x0103)
DEFINE_FRAME_PROC(7, 0x0104)

void ui_proc_string_frame(ui_string_frame_t *msg) {
    msg->option.str_length = strlen(msg->option.string);
    msg->header.SOF = 0xA5;
    msg->header.length = 15 + msg->option.str_length;
    msg->header.seq = seq++;
    msg->header.crc8 = calc_crc8((uint8_t *) msg, 4);
    msg->header.cmd_id = 0x0301;
    msg->header.sub_id = 0x0110;
    msg->header.send_id = ui_self_id;
    msg->header.recv_id = ui_self_id + 256;
    msg->crc16 = calc_crc16((uint8_t *) msg, 13 + 15 + msg->option.str_length);
}

void ui_proc_delete_frame(ui_delete_frame_t *msg) {
    msg->header.SOF = 0xA5;
    msg->header.length = 8;
    msg->header.seq = seq++;
    msg->header.crc8 = calc_crc8((uint8_t *) msg, 4);
    msg->header.cmd_id = 0x0301;
    msg->header.sub_id = 0x0100;
    msg->header.send_id = ui_self_id;
    msg->header.recv_id = ui_self_id + 256;
    msg->crc16 = calc_crc16((uint8_t *) msg, 15);
}

ui_delete_frame_t ui_delete_frame;

void ui_delete_layer(const uint8_t delete_type, const uint8_t layer) {
    ui_delete_frame.delete_type = delete_type;
    ui_delete_frame.layer = layer;
    send_delete_frame(delete_type, layer);
}

void ui_scan_and_send(const ui_interface_figure_t *ui_now_figures, uint8_t *ui_dirty_figure,
                      const ui_interface_string_t *ui_now_strings, uint8_t *ui_dirty_string, 
                      const int total_figures, const int total_strings) {
    if (total_figures > 0) {
        int total_figure = 0;
        uint8_t sent_packets = 0U;

        for (int i = 0; i < total_figures; i++) {
            if (ui_dirty_figure[i] > 0) {
                total_figure++;
            }
        }

        while (total_figure > 0 && sent_packets < UI_MAX_FIGURE_PACKETS_PER_UPDATE) {
            uint8_t pack_size = ui_figure_pack_size_from_remain(total_figure);
            uint8_t packed_count = 0U;

            for (int i = 0; i < total_figures && packed_count < pack_size; i++) {
                if (ui_dirty_figure[i] == 0) {
                    continue;
                }

                if (pack_size == 7U) {
                    _ui_7_frame.data[packed_count] = ui_now_figures[i];
                } else if (pack_size == 5U) {
                    _ui_5_frame.data[packed_count] = ui_now_figures[i];
                } else if (pack_size == 2U) {
                    _ui_2_frame.data[packed_count] = ui_now_figures[i];
                } else {
                    _ui_1_frame.data[packed_count] = ui_now_figures[i];
                }

                ui_dirty_figure[i]--;
                packed_count++;
                total_figure--;
            }

            if (packed_count > 0U) {
                if (pack_size == 7U) {
                    send_figure_frame(_ui_7_frame.data, packed_count, ui_figure_cmd_id_from_count(packed_count));
                } else if (pack_size == 5U) {
                    send_figure_frame(_ui_5_frame.data, packed_count, ui_figure_cmd_id_from_count(packed_count));
                } else if (pack_size == 2U) {
                    send_figure_frame(_ui_2_frame.data, packed_count, ui_figure_cmd_id_from_count(packed_count));
                } else {
                    send_figure_frame(_ui_1_frame.data, 1U, 0x0101U);
                }
                sent_packets++;

                if (total_figure > 0 && sent_packets < UI_MAX_FIGURE_PACKETS_PER_UPDATE) {
                    osDelay(UI_FIGURE_PACKET_DELAY_MS);
                }
            } else {
                break;
            }
        }
    }
    if (total_strings > 0) {
        for (int i = 0; i < total_strings; i++) {
            if (ui_dirty_string[i] > 0) {
                send_string_frame(&ui_now_strings[i]);
                ui_dirty_string[i]--;
                osDelay(10);
            }
        }
    }
}
