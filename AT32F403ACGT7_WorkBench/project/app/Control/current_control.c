#include "current_control.h"
#include "math.h"
#include "stdio.h"

/*************************************************************
** Function name:       CurrentPIControlID
** Descriptions:        D������ջ�
** Input parameters:    pFOC:�ṹ��ָ��
** Output parameters:   None
** Returned value:      None
** Remarks:             None
*************************************************************/
void CurrentPIControlID(PFOC_State pFOC)
{
    //��ȡʵ��ֵ
    pFOC->idPID.pre = pFOC->Id ;
    //��ȡĿ��ֵ
    pFOC->idPID.tar = pFOC->tarid;
    //����ƫ��
    pFOC->idPID.bias = pFOC->idPID.tar - pFOC->idPID.pre;
    //����PID���ֵ
    pFOC->idPID.out += pFOC->idPID.kp * (pFOC->idPID.bias - pFOC->idPID.lastBias) + pFOC->idPID.ki * pFOC->idPID.bias;
    //����ƫ��
    pFOC->idPID.lastBias = pFOC->idPID.bias;

    if (pFOC->idPID.out > fabs(pFOC->idPID.outMax)) {
        pFOC->idPID.out = fabs(pFOC->idPID.outMax);
    }

    if (pFOC->idPID.out < -fabs(pFOC->idPID.outMax)) {
        pFOC->idPID.out = -fabs(pFOC->idPID.outMax);
    }
}
/*************************************************************
** Function name:       CurrentPIControlIQ
** Descriptions:        Q������ջ�
** Input parameters:    pFOC:�ṹ��ָ��
** Output parameters:   None
** Returned value:      None
** Remarks:             None
*************************************************************/
void CurrentPIControlIQ(PFOC_State pFOC)
{
    //��ȡʵ��ֵ
    pFOC->iqPID.pre = pFOC->Iq;

    //��ȡĿ��ֵ
    pFOC->iqPID.tar = pFOC->tariq;

    /*************�ٶȻ�********************/
    pFOC->iqPID.tar = pFOC->speedPID.out;
    /***************************************/

    //����ƫ��
    pFOC->iqPID.bias = pFOC->iqPID.tar - pFOC->iqPID.pre;
    //����PID���ֵ
    pFOC->iqPID.out += pFOC->iqPID.kp * (pFOC->iqPID.bias - pFOC->iqPID.lastBias) + pFOC->iqPID.ki * pFOC->iqPID.bias;
    //����ƫ��
    pFOC->iqPID.lastBias = pFOC->iqPID.bias;

    if (pFOC->iqPID.out > fabs(pFOC->iqPID.outMax)) {
        pFOC->iqPID.out = fabs(pFOC->iqPID.outMax);
    }

    if (pFOC->iqPID.out < -fabs(pFOC->iqPID.outMax)) {
        pFOC->iqPID.out = -fabs(pFOC->iqPID.outMax);
    }
}


/******************************************************************************
  ����˵�������õ���PIDĿ��ֵ
  @brief  ���õ�����PID������d���q���Ŀ��ֵ
  @param  pFOC   ָ��FOC״̬�ṹ���ָ��
  @param  tarid  d�����Ŀ��ֵ
  @param  tariq  q�����Ŀ��ֵ
  @retval ��
******************************************************************************/
void SetCurrentPIDTar(PFOC_State pFOC,float tarid,float tariq)
{
    pFOC->tarid = tarid;
    pFOC->tariq = tariq;
	printf("set tar_id is %lf, tar_iq is %lf\r\n",tarid,tariq);
}

/******************************************************************************
  ����˵�������õ���PID���Ʋ���
  @brief  ���õ�����PID�������ı��������֡�΢��ϵ���Լ�����޷�
  @param  pFOC   ָ��FOC״̬�ṹ���ָ��
  @param  kp     ����ϵ��
  @param  ki     ����ϵ��
  @param  kd     ΢��ϵ��
  @param  outMax PID��������ֵ
  @retval ��
******************************************************************************/
void SetCurrentPIDParams(PFOC_State pFOC,float kp,float ki,float kd,float outMax)
{
    pFOC->idPID.kp = kp;
    pFOC->idPID.ki = ki;
    pFOC->idPID.kd = kd;
    pFOC->idPID.outMax = outMax;
	
	pFOC->iqPID.kp = kp;
    pFOC->iqPID.ki = ki;
    pFOC->iqPID.kd = kd;
    pFOC->iqPID.outMax = outMax;
	printf("set kp is %lf, ki is %lf, kd is %lf\r\n",kp,ki,kd);
}




