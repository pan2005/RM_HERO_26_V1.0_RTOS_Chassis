//
// Created by PanZhaoHui on 25-12-15.
//

#include "usart_protocol.h"

#include <stddef.h>
#include <string.h>

static uint8_t Calc_Checksum(uint8_t *data, uint16_t len) {
    uint8_t sum = 0;
    for (uint16_t i = 0; i < len; i++) sum += data[i];
    return sum;
}

// 封包函数：将结构体打包成字节数组
void Protocol_Pack_ChassisInfo(ChassisInfo_t *info, uint8_t *tx_buf, uint16_t *tx_len) {
    uint16_t idx = 0;
    uint8_t data_len = sizeof(ChassisInfo_t);

    tx_buf[idx++] = FRAME_HEADER;          // 0. 头
    tx_buf[idx++] = CMD_ID_CHASSIS_INFO;   // 1. 命令ID
    tx_buf[idx++] = data_len;              // 2. 数据长度

    memcpy(&tx_buf[idx], info, data_len);  // 3. 数据
    idx += data_len;

    tx_buf[idx] = Calc_Checksum(tx_buf, idx); // 4. 校验和 (计算前面所有字节)
    idx++;

    tx_buf[idx++] = FRAME_TAIL;            // 5. 尾

    *tx_len = idx; // 返回总长度
}

// 解析函数
uint8_t Protocol_Parse(uint8_t *rx_buf, uint16_t rx_len, ChassisInfo_t *out_info) {
    // 1. 基础长度校验
    if (rx_buf == NULL || rx_len < PROT_MIN_LEN) return 0;

    // 2. 校验头尾
    if (rx_buf[0] != FRAME_HEADER || rx_buf[rx_len - 1] != FRAME_TAIL) return 0;

    // 3. 校验和比对
    if (rx_buf[rx_len - 2] != Calc_Checksum(rx_buf, rx_len - 2)) return 0;

    // 4. 解析数据
    uint8_t cmd_id = rx_buf[1];
    uint8_t data_len = rx_buf[2];

    if (cmd_id == CMD_ID_CHASSIS_INFO && data_len == sizeof(ChassisInfo_t)) {
        memcpy(out_info, &rx_buf[3], sizeof(ChassisInfo_t));
        return 1; // 成功
    }

    return 0; // 失败
}