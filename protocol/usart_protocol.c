#include "usart_protocol.h"
#include <string.h>

static uint8_t Calc_Checksum(uint8_t *data, uint16_t len) {
    uint8_t sum = 0;
    for (uint16_t i = 0; i < len; i++) sum += data[i];
    return sum;
}

/**
 * @brief 打包云台信息 (Gimbal -> Chassis)
 */
void Protocol_Pack_GimbalInfo(GimbalInfo_t *info, uint8_t *tx_buf, uint16_t *tx_len) {
    uint16_t idx = 0;
    uint8_t data_len = sizeof(GimbalInfo_t);

    tx_buf[idx++] = FRAME_HEADER;
    tx_buf[idx++] = CMD_ID_GIMBAL_INFO;
    tx_buf[idx++] = data_len;

    memcpy(&tx_buf[idx], info, data_len);
    idx += data_len;

    tx_buf[idx] = Calc_Checksum(tx_buf, idx);
    idx++;

    tx_buf[idx++] = FRAME_TAIL;
    *tx_len = idx;
}

/**
 * @brief 打包底盘信息 (Chassis -> Gimbal)
 */
void Protocol_Pack_ChassisInfo(ChassisInfo_t *info, uint8_t *tx_buf, uint16_t *tx_len) {
    uint16_t idx = 0;
    uint8_t data_len = sizeof(ChassisInfo_t);

    tx_buf[idx++] = FRAME_HEADER;
    tx_buf[idx++] = CMD_ID_CHASSIS_CTRL;
    tx_buf[idx++] = data_len;

    memcpy(&tx_buf[idx], info, data_len);
    idx += data_len;

    tx_buf[idx] = Calc_Checksum(tx_buf, idx);
    idx++;

    tx_buf[idx++] = FRAME_TAIL;
    *tx_len = idx;
}

/**
 * @brief 通用解析函数
 * @param out_cmd_id 输出解析到的命令ID
 * @param out_struct 输出解析到的结构体数据（需确保外部传入足够的空间）
 * @return 1:解析成功, 0:解析失败
 */
uint8_t Protocol_Parse(uint8_t *rx_buf, uint16_t rx_len, uint8_t *out_cmd_id, void *out_struct) {
    if (rx_buf == NULL || rx_len < PROT_MIN_LEN) return 0;

    // 1. 校验头尾
    if (rx_buf[0] != FRAME_HEADER || rx_buf[rx_len - 1] != FRAME_TAIL) return 0;

    // 2. 校验和
    if (rx_buf[rx_len - 2] != Calc_Checksum(rx_buf, rx_len - 2)) return 0;

    // 3. 提取元数据
    uint8_t cmd_id = rx_buf[1];
    uint8_t data_len = rx_buf[2];
    *out_cmd_id = cmd_id;

    // 4. 根据 CMD_ID 分发解析逻辑
    switch (cmd_id) {
        case CMD_ID_GIMBAL_INFO:
            if (data_len == sizeof(GimbalInfo_t)) {
                memcpy(out_struct, &rx_buf[3], sizeof(GimbalInfo_t));
                return 1;
            }
            break;

        case CMD_ID_CHASSIS_CTRL:
            if (data_len == sizeof(ChassisInfo_t)) {
                memcpy(out_struct, &rx_buf[3], sizeof(ChassisInfo_t));
                return 1;
            }
            break;

        default:
            break;
    }

    return 0;
}