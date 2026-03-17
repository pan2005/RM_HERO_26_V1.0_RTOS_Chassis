//
// Created by PanZhaoHui on 26-2-12.
//

#include "irr_fliter.h"
/**
 * @brief 初始化陷波器
 * @param nf: 滤波器句柄
 * @param center_freq: 陷波频率 (200Hz)
 * @param sample_freq: 采样频率 (1000Hz)
 * @param bandwidth: 带宽/强度因子 (0.0 ~ <1.0)。
 *        针对云台同步带共振，建议 0.85 ~ 0.92。
 *        0.85: 陷波较浅，相位滞后小（推荐起点）。
 *        0.95: 陷波很深，但相位滞后大，可能影响稳定性。
 */
void NotchFilter_Init(NotchFilter_t *nf, float center_freq, float sample_freq, float bandwidth) {
    nf->x1 = nf->x2 = 0.0f;
    nf->y1 = nf->y2 = 0.0f;

    float omega = 2.0f * M_PI * center_freq / sample_freq;
    float sn = sinf(omega);
    float cs = cosf(omega);
    float r = bandwidth;

    // 极零点放置法设计系数
    // 归一化因子，保证 DC (0Hz) 增益为 1
    float scale = (1.0f + r * r - 2.0f * r * cs) / (2.0f - 2.0f * cs);

    // 注意：这里计算的是归一化后的系数
    // b系数
    nf->b0 = 1.0f * scale;
    nf->b1 = -2.0f * cs * scale;
    nf->b2 = 1.0f * scale;

    // a系数 (注意：在标准差分方程 y[n] = ... - a1*y[n-1] ... 中，这里的a1是正负号处理后的)
    // 这里的实现方式是：output = b*x - (a1*y1 + a2*y2)
    // 所以存储直接的系数
    nf->a1 = -2.0f * r * cs;
    nf->a2 = r * r;
}

/**
 * @brief 滤波器运行函数 (放在中断中调用)
 *        使用 static inline 以便编译器内联优化，减少函数调用开销
 */
float NotchFilter_Apply(NotchFilter_t *nf, float input) {
    float output = nf->b0 * input + nf->b1 * nf->x1 + nf->b2 * nf->x2
                 - nf->a1 * nf->y1 - nf->a2 * nf->y2;

    // 更新历史数据
    nf->x2 = nf->x1;
    nf->x1 = input;

    nf->y2 = nf->y1;
    nf->y1 = output;

    return output;
}
