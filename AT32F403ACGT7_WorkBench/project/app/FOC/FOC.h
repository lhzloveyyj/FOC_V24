#ifndef __FOC_H
#define __FOC_H

#include "svpwm.h"
#include "at32f403a_407.h"  // Device header

#ifdef __cplusplus
extern "C" {
#endif

float g_udc ;
/**
 * @brief 电流采样状态
 */
typedef struct {
    uint16_t adA;               // A相ADC原始值
    uint16_t adB;               // B相ADC原始值
    uint16_t voltageAOffset;    // A相电压偏移
    uint16_t voltageBOffset;    // B相电压偏移
} FocCurrentState;

/**
 * @brief FOC运行状态结构体
 */
typedef struct {
    FocCurrentState current;    // 电流采样

    float uAlpha, uBeta;        // 电压 αβ
    float iAlpha, iBeta;        // 电流 αβ
    float ia, ib;               // 电流AB
    float ua, ub, uc;           // 电压ABC
    float uq, ud;               // 电压 dq
    float iq, id;               // 电流 dq

    float mechanicalAngle;      // 机械角度（rad）
    float electricalAngle;      // 电角度（rad）
    float correctedAngle;       // 修正后的电角度
    float zeroOffset;           // 零电角度偏移

    float (*GetMechanicalAngle)(void);                     // 获取机械角度函数指针
    void (*SetPwmCallback)(float pwmA, float pwmB, float pwmC); // 设置PWM函数指针
} FocState;

typedef FocState* PFocState;

extern uint16_t g_motorAdValues[2];  // ADC 原始值数组（外部使用）

extern PFocState g_pMotor;          // 电机 FOC 状态对象指针

#ifdef __cplusplus
}
#endif

#endif // _LOS_FOC_H
