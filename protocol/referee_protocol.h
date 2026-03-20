//
// Created by PanZhaoHui on 26-3-17.
//

#ifndef REFEREE_PROTOCOL_H
#define REFEREE_PROTOCOL_H

#include "robot_global.h"

#if defined(__GNUC__)
#define REFEREE_PACKED __attribute__((packed))
#else
#define REFEREE_PACKED
#endif

void Referee_Data_Solve(uint8_t *frame, uint16_t len);

#endif //REFEREE_PROTOCOL_H
