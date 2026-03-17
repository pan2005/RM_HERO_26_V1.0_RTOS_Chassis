//
// Created by PanZhaoHui on 26-2-12.
//

#ifndef IRR_FLITER_H
#define IRR_FLITER_H


#include <math.h>

// 定义 PI，如果 math.h 没有定义
#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

typedef struct {
    // 系数
    float b0, b1, b2;
    float a1, a2;

    // 状态历史 (History)
    float x1, x2;
    float y1, y2;
} NotchFilter_t;

void NotchFilter_Init(NotchFilter_t *nf, float center_freq, float sample_freq, float bandwidth);
float NotchFilter_Apply(NotchFilter_t *nf, float input);
#endif //IRR_FLITER_H
