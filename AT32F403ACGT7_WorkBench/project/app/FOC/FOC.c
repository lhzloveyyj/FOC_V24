#include "at32f403a_407.h"              // Device header
#include "foc.h"
#include "foc_config.h"
#include "math.h"
#include "my_math.h"
#include "fast_sin.h"
#include "delay.h"
#include "stdio.h"
#include "mt6701.h"
#include "log.h"

float g_udc = 0.0f;

float  zero = 0.0f;	
uint16_t AD_Value[2]={0};

typedef struct {
    float i_d;  // d轴电流
    float i_q;  // q轴电流
    float v_d;  // d轴电压
    float v_q;  // q轴电压
    float theta; // 电机角度
    float omega; // 电机角速度
} MotorState;


/**
 * @brief     将角度值归一化到 [0, 2π) 区间
 * @param     angle   输入角度（单位：rad）
 * @return    归一化后的角度值，范围为 [0, 2π)
 */
float NormalizeAngle(float angle)
{
    float result = fmodf(angle, FOC_2PI);  // 使用浮点取余
    return (result >= 0.0f) ? result : (result + FOC_2PI);
}


/**
 * @brief     计算电角度：电角度 = 机械角度 × 极对数，并归一化到 [0, 2π)
 * @param     mechAngle   机械角度（单位：rad）
 * @return    电角度（单位：rad），范围 [0, 2π)
 */
float CalculateElectricalAngle(float mechAngle)
{
    float elecAngle = mechAngle * FOC_POLE_PAIRS;
    return NormalizeAngle(elecAngle);
}

/**
 * @brief     施加恒定 Ud 电压进行强制对准（锁定电角度）
 * @param     ud   d轴电压（单位：V）
 */
void MotorApplyStrongDrag(float ud)
{
    float uAlpha = 0.0f;
    float uBeta = 0.0f;
    float uq = 0.0f;

    // 获取修正后的电角度（theta_e）
    float angleEl = MotorGetCorrectedElecAngle(0.0f);

    // Park 逆变换（dq -> αβ）
    uAlpha = -uq * fast_sin(angleEl) + ud * fast_cos(angleEl);
    uBeta  =  uq * fast_cos(angleEl) + ud * fast_sin(angleEl);

    // Clarke 逆变换（αβ -> abc），带中点电压偏移
    float ua = uAlpha + g_udc / 2.0f;
    float ub = (FOC_SQRT3 * uBeta - uAlpha) / 2.0f + g_udc / 2.0f;
    float uc = (-FOC_SQRT3 * uBeta - uAlpha) / 2.0f + g_udc / 2.0f;

    MotorSetPwm(ua, ub, uc);
}

/**
 * @brief     获取修正后的电角度（减去零电角度偏移并归一化到 [0, 2π)）
 * @param     mechAngle    机械角度（单位：rad）
 * @return    电角度（单位：rad），范围 [0, 2π)
 */
float AngleGetCorrectedElec(float mechAngle)
{
    float elecAngle = CalculateElectricalAngle(mechAngle);       // 电角度 = 机械角 × 极对数
    float corrected = elecAngle - g_zeroOffset;             // 减去零电位偏移
    corrected = NormalizeAngle(corrected);                  // 归一化到 [0, 2π)

    // 修正 2π 问题：如果结果非常接近 2π，认为是 0
    if (fabsf(corrected - FOC_2PI) < FOC_EPSILON) {
        corrected = 0.0f;
    }

    return corrected;
}

/**
 * @brief     角度模块初始化，采集零电角度偏移（调用强拖，进行多次平均）
 * @param     readAngleFunc   用于读取机械角度的函数指针（单位：rad）
 */
void AngleInitZeroOffset(float (*readAngleFunc)(void))
{
    gpio_bits_reset(GPIOB, GPIO_PINS_3);  // 拉低启动引脚，表明开始初始化

    MotorApplyStrongDrag(1.0f);           // 施加 Ud 强拖，固定转子磁极方向
    delay_ms(2000);                       // 保持拖动 2 秒

    // 多次采样以降低抖动影响
    float sum = 0.0f;
    const int sampleCount = 10;

    for (int i = 0; i < sampleCount; i++) {
        float elecAngle = CalculateElectricalAngle(readAngleFunc());
        sum += elecAngle;
        delay_ms(10);
    }

    g_zeroOffset = sum / sampleCount;  // 计算平均值作为零电角度
    float corrected = AngleGetCorrectedElec(readAngleFunc());

    usb_printf("零电位角度：%f, %lf\r\n", g_zeroOffset, corrected);
    usb_printf("初始化完成\r\n");

    gpio_bits_set(GPIOB, GPIO_PINS_4);  // 设置完成信号
}


void adc_tigger(int time_pwm)
{
	tmr_channel_value_set(TMR2, TMR_SELECT_CHANNEL_3, time_pwm-10);
}

float g_pwmA = 0.0f;
float g_pwmB = 0.0f;
float g_pwmC = 0.0f;

/**
 * @brief     设置三相 PWM 输出（Ua、Ub、Uc 为 SVPWM 输出电压）
 * @param     ua   A相电压（单位：V）
 * @param     ub   B相电压（单位：V）
 * @param     uc   C相电压（单位：V）
 */
void MotorSetPwm(float ua, float ub, float uc)
{
    // 电压限幅保护（防止超出允许范围）
    ua = LimitValue(ua, 0.0f, g_voltageHigh);
    ub = LimitValue(ub, 0.0f, g_voltageHigh);
    uc = LimitValue(uc, 0.0f, g_voltageHigh);

    // 电压归一化到占空比 [0, 1]
    g_pwmA = LimitValue(ua / g_udc, 0.0f, 1.0f);
    g_pwmB = LimitValue(ub / g_udc, 0.0f, 1.0f);
    g_pwmC = LimitValue(uc / g_udc, 0.0f, 1.0f);

    // 输出到定时器 PWM 寄存器
    tmr_channel_value_set(TMR1, TMR_SELECT_CHANNEL_1, g_pwmA * FOC_ALL_DUTY);
    tmr_channel_value_set(TMR1, TMR_SELECT_CHANNEL_2, g_pwmB * FOC_ALL_DUTY);
    tmr_channel_value_set(TMR1, TMR_SELECT_CHANNEL_3, g_pwmC * FOC_ALL_DUTY);
}


// Clarke变换（电流）
void clarke_transform(float Ia, float Ib, float *Ialpha, float *Ibeta) {
    *Ialpha = Ia;
    *Ibeta = (1 / FOC_SQRT3) * (Ia + 2 * Ib);  // Clarke变换公式，线性组合
}

// Park变换（电流）
void park_transform(float Ialpha, float Ibeta, float angle_el, float *Id, float *Iq) {
    *Id = Ialpha * fast_cos(angle_el) + Ibeta * fast_sin(angle_el);  // Park变换公式
    *Iq = -Ialpha * fast_sin(angle_el) + Ibeta * fast_cos(angle_el);
}

// FOC核心函数：输入Uq、Ud和电角度，输出三路PWM
float Ualpha=0.0f,Ubate=0.0f;
float Ialpha=0.0f,Ibeta=0.0f;

float Ua=0.0f,Ub=0.0f,Uc=0.0f;
float Uq=0.0f,Ud=0.0f;
float Iq=0.0f,Id=0.0f;
// FOC 控制主函数
void FocContorl(PFOC_State pFOC, PSVpwm_State PSVpwm)
{
	//计算电角度
	getCorrectedElectricalAngle(pFOC);
	
	pFOC->current.ad_A = Motor1_AD_Value[1];
	pFOC->current.ad_B = Motor1_AD_Value[0];

	
	pFOC->Ia = (pFOC->current.ad_A - pFOC->current.voltage_a_offset)/4096.0f * ADC_REF_VOLTAGE * GAIN;
	pFOC->Ib = (pFOC->current.ad_B - pFOC->current.voltage_a_offset)/4096.0f * ADC_REF_VOLTAGE * GAIN;
	pFOC->Ia = 0 - pFOC->Ia - pFOC->Ib;
	
	clarke_transform(pFOC) ;
	park_transform(pFOC);
	//PID控制器
	pFOC->Ud = PI_Compute(&pi_Id, 0.0f, pFOC->Id);
	pFOC->Uq = PI_Compute(&pi_Id, 0.0f, pFOC->Iq);
	
	pFOC->Ud = 0.0f;
	pFOC->Uq = 2.0f;
	//逆park变换
	inv_park_transform(pFOC);
    
	SVpwm(PSVpwm, pFOC->Ualpha, pFOC->Ubeta);
	
	//逆clarke变换
	//inv_clarke_transform(pFOC);
	
	//设置SVPWM
	setSVpwm(pFOC, PSVpwm);
}






