#include "watchdog.h"
/*****************************************************
������       void IWDG_Init(void)
��;��       �������Ź���ʼ��
����ֵ��     ��
˵����     stm32�Ķ������Ź����ڲ�ר�ŵ�40Khz����ʱ������
 ****************************************************/
void IWDG_Init(void)
{  
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);    //д��0x5555�������������Ĵ���д�빦��
    IWDG_SetPrescaler(IWDG_Prescaler_256);             //����ʱ�ӷ�Ƶ��40K/256=156HZ(6.4ms)
 //   IWDG_SetReload(625);                             //ι��ʱ��5s/6.4ms=781(���ܴ���0xfff)
    IWDG_SetReload(781); 
    IWDG_ReloadCounter();                             //ι��
    IWDG_Enable();                                   //ʹ�ܹ���
}
/*****************************************************
������       void IWDG_Feed(void)
��;��       ζ������
 ����ֵ�� ��
 ****************************************************/
void IWDG_Feed(void)
{
    IWDG_ReloadCounter();           
}
