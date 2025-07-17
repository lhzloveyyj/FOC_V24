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

/*******************************全局变量***************************/
float g_udc = 24.0f;
float g_zeroOffset = 0.0f;

float  zero = 0.0f;	
uint16_t g_motorAdValues[3]={0};

int cnt =0;

/*******************************************************************/

/*****************定义电机1的FOC状态结构体*************************/
FocState Motor = {
    // current 可视情况初始化
//  .current = {
//      .ad_A = 0,
//      .ad_B = 0,
//      .voltage_a_offset = 0,
//      .voltage_b_offset = 0,
//      .Mflag = 1
//  },

    .uAlpha = 0.0f, .uBeta = 0.0f, 	
    .iAlpha = 0.0f, .iBeta = 0.0f, 	
    .ia = 0.0f, .ib = 0.0f, .ic = 0.0f,			
    .ua = 0.0f, .ub = 0.0f, .uc = 0.0f, 		
    .uq = 0.0f, .ud = 0.0f, 			
    .iq = 0.0f, .id = 0.0f, 			

    .mechanicalAngle = 0.0f,
    .electricalAngle = 0.0f,
    .correctedAngle = 0.0f,
    .zeroOffset = 0.0f,

    //.idPID = {0},
    //.iqPID = {0},

    .speedLastAngle = 0.0f,
    .speed = 0.0f,
    //.speedPID = {0},

    .setPwmCallback = NULL  // 或者设为具体函数名
};


PFocState g_pMotor = &Motor;

void MotorSetPwm(float ua, float ub, float uc);

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
 * @brief     施加恒定 Ud 电压进行强制对准（锁定电角度）
 * @param     ud   d轴电压（单位：V）
 */
void MotorApplyStrongDrag(float ud)
{
    float uAlpha = 0.0f;
    float uBeta = 0.0f;
    float uq = 0.0f;

    // 获取修正后的电角度（theta_e）
    float angleEl = AngleGetCorrectedElec(0.0f);

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
 * @brief     角度模块初始化，采集零电角度偏移（调用强拖，进行多次平均）
 * @param     readAngleFunc   用于读取机械角度的函数指针（单位：rad）
 */
void AngleInitZeroOffset(void)
{
    gpio_bits_reset(GPIOB, GPIO_PINS_3);  // 开启设置LED

    MotorApplyStrongDrag(FOC_STRONGDRAG);           // 施加 Ud 强拖，固定转子磁极方向
    delay_ms(1500);                       // 保持拖动 2 秒

    // 多次采样以降低抖动影响
    float sum = 0.0f;
    const int sampleCount = 10;
	float mechanicalAngle = 0.0f;

    for (int i = 0; i < sampleCount; i++) {
		mechanicalAngle = Mt6701GetAngleWrapper();
        float elecAngle = CalculateElectricalAngle(mechanicalAngle);
        sum += elecAngle;
        delay_ms(10);
    }
	
	mechanicalAngle = Mt6701GetAngleWrapper();
    g_zeroOffset = sum / sampleCount;  // 计算平均值作为零电角度
    float corrected = AngleGetCorrectedElec(mechanicalAngle);

    printf("零电位角度：%f, %lf\r\n", g_zeroOffset, corrected);
    printf("初始化完成\r\n");

    gpio_bits_set(GPIOB, GPIO_PINS_4);  // 设置完成信号
	MotorApplyStrongDrag(0.0f);
}


void adc_tigger(int time_pwm)
{
	tmr_channel_value_set(TMR2, TMR_SELECT_CHANNEL_3, time_pwm-10);
}

float g_pwmA = 0.0f;
float g_pwmB = 0.0f;
float g_pwmC = 0.0f;

/******************************************************************************
  函数说明：电机 PWM输出设置函数
  @brief  根据输入占空比设置电机1三相PWM输出
  @param  pwm_a 相A占空比
  @param  pwm_b 相B占空比
  @param  pwm_c 相C占空比
  @retval 无
******************************************************************************/
static void setpwm_channel(float pwm_a, float pwm_b, float pwm_c)
{
	tmr_channel_value_set(TMR1, TMR_SELECT_CHANNEL_1, pwm_a * FOC_ALL_DUTY);
    tmr_channel_value_set(TMR1, TMR_SELECT_CHANNEL_2, pwm_b * FOC_ALL_DUTY);
    tmr_channel_value_set(TMR1, TMR_SELECT_CHANNEL_3, pwm_c * FOC_ALL_DUTY);
	
	tmr_channel_value_set(TMR1, TMR_SELECT_CHANNEL_4, FOC_ALL_DUTY * 0.95f);
}

/**
 * @brief     设置三相 PWM 输出（Ua、Ub、Uc 为 SVPWM 输出电压）
 * @param     ua   A相电压（单位：V）
 * @param     ub   B相电压（单位：V）
 * @param     uc   C相电压（单位：V）
 */
void MotorSetPwm(float ua, float ub, float uc)
{
    // 电压限幅保护（防止超出允许范围）
    ua = LimitValue(ua, 0.0f, g_udc);
    ub = LimitValue(ub, 0.0f, g_udc);
    uc = LimitValue(uc, 0.0f, g_udc);

    // 电压归一化到占空比 [0, 1]
    g_pwmA = LimitValue(ua / g_udc, 0.0f, 1.0f);
    g_pwmB = LimitValue(ub / g_udc, 0.0f, 1.0f);
    g_pwmC = LimitValue(uc / g_udc, 0.0f, 1.0f);

    // 输出到定时器 PWM 寄存器
    setpwm_channel(g_pwmA, g_pwmB, g_pwmC);
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

/******************************************************************************
  函数说明：设置SVPWM输出
  @brief  根据SVPWM算法计算结果设置PWM输出
  @param  pFOC 指向FOC状态结构体的指针
  @param  PSVpwm 指向SVPWM状态结构体的指针
  @retval 无
******************************************************************************/
static void setSVpwm(PSVpwm_State PSVpwm)
{
	
    setpwm_channel(PSVpwm->Ta, PSVpwm->Tb, PSVpwm->Tc);
}

/******************************************************************************
  函数说明：逆Park变换
  @brief  将dq坐标系的电压转换为αβ坐标系，以便进行SVPWM计算
  @param  pFOC 指向FOC状态结构体的指针
  @retval 无
******************************************************************************/
static void inv_park_transform(float Uq, float Ud, float corr_angle, float *Out_Ualpha, float *Out_Ubeta)
{
	*Out_Ualpha = -Uq * fast_sin(corr_angle) + Ud * fast_cos(corr_angle);
	*Out_Ubeta  =  Uq * fast_cos(corr_angle) + Ud * fast_sin(corr_angle);
}

/******************************************************************************
  函数说明：逆Clarke变换
  @brief  将αβ坐标系的电压转换为三相电压，适用于PWM输出
  @param  pFOC 指向FOC状态结构体的指针
  @retval 无
******************************************************************************/
static void inv_clarke_transform(float Ualpha, float Ubeta, float *Out_Ua, float *Out_Ub, float *Out_Uc)
{
	*Out_Ua = Ualpha + g_udc/2;
	*Out_Ub = (FOC_SQRT3 * Ubeta - Ualpha)/2 + g_udc/2;
	*Out_Uc = (-FOC_SQRT3 * Ubeta - Ualpha)/2 + g_udc/2;
}

// FOC 控制主函数
void FocContorl(PFocState pFOC,  PSVpwm_State PSVpwm)
{
	//获取机械角度
	pFOC->mechanicalAngle = Mt6701GetAngleWrapper();
	
	//计算电角度
	pFOC->correctedAngle = AngleGetCorrectedElec(pFOC->mechanicalAngle);
	
	//pFOC->current.ad_A = Motor1_AD_Value[1];
	//pFOC->current.ad_B = Motor1_AD_Value[0];

	
	//pFOC->Ia = (pFOC->current.ad_A - pFOC->current.voltage_a_offset)/4096.0f * ADC_REF_VOLTAGE * GAIN;
	//pFOC->Ib = (pFOC->current.ad_B - pFOC->current.voltage_a_offset)/4096.0f * ADC_REF_VOLTAGE * GAIN;
	//pFOC->Ia = 0 - pFOC->Ia - pFOC->Ib;
	
	//clarke_transform(pFOC) ;
	//park_transform(pFOC);
	//PID控制器
	//pFOC->Ud = PI_Compute(&pi_Id, 0.0f, pFOC->Id);
	//pFOC->Uq = PI_Compute(&pi_Id, 0.0f, pFOC->Iq);
	
	pFOC->ud = 0.0f;
	pFOC->uq = 6.0f;
	
	//逆park变换
	inv_park_transform(pFOC->uq, pFOC->ud, pFOC->correctedAngle, &(pFOC->uAlpha), &(pFOC->uBeta));
	
	//逆clarke变换
	inv_clarke_transform(pFOC->uAlpha, pFOC->uBeta , &(pFOC->ua), &(pFOC->ub), &(pFOC->uc));
	
	//设置PWM
	MotorSetPwm(pFOC->ua, pFOC->ub, pFOC->uc);
	
	//SVpwm(PSVpwm, pFOC->uAlpha, pFOC->uBeta);
	
	//设置SVPWM
	//setSVpwm(PSVpwm);
}






