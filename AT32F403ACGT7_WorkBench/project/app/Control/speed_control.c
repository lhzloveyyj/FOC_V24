#include "speed_control.h"
#include "filter.h"
#include "math.h"
#include "stdio.h"

void CalculateSpeed(PFOC_State pFOC, float dt, PLPF_Speed pSpeedFilter)
{
    float angle_diff = (pFOC->mechanical_angle - pFOC->speed_last_angle);

    // ����� 0 �� 2�� ������
    if (angle_diff > 3.14159f)
        angle_diff -= 6.28318f;
    else if (angle_diff < -3.14159f)
        angle_diff += 6.28318f;

    // ����ԭʼ�ٶ�
    pFOC->speed = angle_diff / dt;

    // ʹ���˲���ƽ���ٶ�
    LPF_Speed_Update(pSpeedFilter, pFOC->speed, &(pFOC->speed));

    // ������һ�εĽǶ�ֵ
    pFOC->speed_last_angle = pFOC->mechanical_angle;
}

/*************************************************************
** Function name:       SpeedPIControlID
** Descriptions:        �ٶȱջ�
** Input parameters:    pFOC:�ṹ��ָ��
** Output parameters:   None
** Returned value:      None
** Remarks:             None
*************************************************************/
void SpeedPIControl(PFOC_State pFOC)
{
    //��ȡʵ��ֵ
    pFOC->speedPID.pre = pFOC->speed ;
    //��ȡĿ��ֵ
    pFOC->speedPID.tar = pFOC->tar_speed;
    //����ƫ��
    pFOC->speedPID.bias = pFOC->speedPID.tar - pFOC->speedPID.pre;
    //����PID���ֵ
    pFOC->speedPID.out += pFOC->speedPID.kp * (pFOC->speedPID.bias - pFOC->speedPID.lastBias) + pFOC->speedPID.ki * pFOC->speedPID.bias;
    //����ƫ��
    pFOC->speedPID.lastBias = pFOC->speedPID.bias;

    if (pFOC->speedPID.out > fabs(pFOC->speedPID.outMax)) {
        pFOC->speedPID.out = fabs(pFOC->speedPID.outMax);
    }

    if (pFOC->speedPID.out < -fabs(pFOC->speedPID.outMax)) {
        pFOC->speedPID.out = -fabs(pFOC->speedPID.outMax);
    }
}

/******************************************************************************
  ����˵���������ٶ�PIDĿ��ֵ
  @brief  �����ٶȻ�PID�������ٶȵ�Ŀ��ֵ
  @param  pFOC   ָ��FOC״̬�ṹ���ָ��
  @param  tarspeed  �ٶ�Ŀ��ֵ
  @retval ��
******************************************************************************/
void SetSpeedPIDTar(PFOC_State pFOC,float tarspeed)
{
    pFOC->tar_speed = tarspeed;
	printf("set tarspeed is %lf\r\n",tarspeed);
}

/******************************************************************************
  ����˵���������ٶ�PID���Ʋ���
  @brief  �����ٶ�PID�������ı��������֡�΢��ϵ���Լ�����޷�
  @param  pFOC   ָ��FOC״̬�ṹ���ָ��
  @param  kp     ����ϵ��
  @param  ki     ����ϵ��
  @param  kd     ΢��ϵ��
  @param  outMax PID��������ֵ
  @retval ��
******************************************************************************/
void SetSpeedPIDParams(PFOC_State pFOC,float kp,float ki,float kd,float outMax)
{
    pFOC->speedPID.kp = kp;
    pFOC->speedPID.ki = ki;
    pFOC->speedPID.kd = kd;
    pFOC->speedPID.outMax = outMax;
printf("set kp is %lf, ki is %lf, kd is %lf\r\n",kp,ki,kd);
}


