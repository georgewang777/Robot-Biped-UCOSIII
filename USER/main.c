/************************************************
Author: George Wang
Email:  georgewang.7sl@gmail.com
************************************************/
#include "george_system.h"

extern u8 RxCount;    //Index
u8 Mk_UsartAll=0;	   //����һ��������ɱ�־
u8 led_flag = 0;         //�ƹ⿪�ر�־λ 1 �� 0 ��
u8 pre_cnt_rs2=0;    //���ݱ�־
u8 tim2_count;    	//��ʱ��2������־
/**************UCOSII��������************************/

//UCOSIII���������ȼ��û�������ʹ�ã�ALIENTEK
//����Щ���ȼ��������UCOSIII��5��ϵͳ�ڲ�����
//���ȼ�0���жϷ������������� OS_IntQTask()
//���ȼ�1��ʱ�ӽ������� OS_TickTask()
//���ȼ�2����ʱ���� OS_TmrTask()
//���ȼ�OS_CFG_PRIO_MAX-2��ͳ������ OS_StatTask()
//���ȼ�OS_CFG_PRIO_MAX-1���������� OS_IdleTask()

#define START_TASK_PRIO		3            //�������ȼ�
#define START_STK_SIZE 		128          //�����ջ��С
OS_TCB StartTaskTCB;                   //������ƿ�
CPU_STK START_TASK_STK[START_STK_SIZE];//�����ջ	
void start_task(void *p_arg);          //������

#define TASK1_TASK_PRIO		5            //�������ȼ�
#define TASK1_STK_SIZE 		128          //�����ջ��С	
OS_TCB Task1_TaskTCB;                  //������ƿ�
CPU_STK TASK1_TASK_STK[TASK1_STK_SIZE];//�����ջ	
void task1_task(void *p_arg);          //������

#define TASK2_TASK_PRIO		4            //�������ȼ�
#define TASK2_STK_SIZE 		128          //�����ջ��С	
OS_TCB Task2_TaskTCB;                  //������ƿ�
CPU_STK TASK2_TASK_STK[TASK2_STK_SIZE];//�����ջ	
void task2_task(void *p_arg);          //������

OS_SEM MY_SEM;                         //�ź��������ڷ��ʹ�������Դ
//������
int main(void)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	SysInit();
	OSInit(&err);		      //��ʼ��UCOSIII
	OS_CRITICAL_ENTER();	//�����ٽ���			 
	//������ʼ����
	OSTaskCreate((OS_TCB 	* )&StartTaskTCB,	    	   //������ƿ�
				 (CPU_CHAR	* )"start task", 		           //��������
                 (OS_TASK_PTR )start_task, 			   //������
                 (void		* )0,					           //���ݸ��������Ĳ���
                 (OS_PRIO	  )START_TASK_PRIO,      //�������ȼ�
                 (CPU_STK   * )&START_TASK_STK[0], //�����ջ����ַ
                 (CPU_STK_SIZE)START_STK_SIZE/10,	 //�����ջ�����λ
                 (CPU_STK_SIZE)START_STK_SIZE,		 //�����ջ��С
                 (OS_MSG_QTY  )0,					         //�����ڲ���Ϣ�����ܹ����յ������Ϣ��Ŀ,Ϊ0ʱ��ֹ������Ϣ
                 (OS_TICK	  )0,					           //��ʹ��ʱ��Ƭ��תʱ��ʱ��Ƭ���ȣ�Ϊ0ʱΪĬ�ϳ��ȣ�
                 (void   	* )0,					           //�û�����Ĵ洢��
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, //����ѡ��
                 (OS_ERR 	* )&err);				          //��Ÿú�������ʱ�ķ���ֵ
	OS_CRITICAL_EXIT();	//�˳��ٽ���	 
	OSStart(&err);      //����UCOSIII
}

//��ʼ����������
void start_task(void *p_arg)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	p_arg = p_arg;

	CPU_Init();
	
#if OS_CFG_STAT_TASK_EN > 0u
   OSStatTaskCPUUsageInit(&err);  	//ͳ������                
#endif
	
#ifdef CPU_CFG_INT_DIS_MEAS_EN		//���ʹ���˲����жϹر�ʱ��
    CPU_IntDisMeasMaxCurReset();	
#endif
	
#if	OS_CFG_SCHED_ROUND_ROBIN_EN  //��ʹ��ʱ��Ƭ��ת��ʱ��
  //ʹ��ʱ��Ƭ��ת���ȹ���,ʱ��Ƭ����Ϊ1��ϵͳʱ�ӽ��ģ���1*5=5ms
	OSSchedRoundRobinCfg(DEF_ENABLED,1,&err);  
#endif	
	
	OS_CRITICAL_ENTER();	//�����ٽ���
	//����һ���ź���
//	OSSemCreate ((OS_SEM*	)&MY_SEM,
//                 (CPU_CHAR*	)"MY_SEM",
//                 (OS_SEM_CTR)0,		
//                 (OS_ERR*	)&err);
	//����TASK1����
	OSTaskCreate((OS_TCB 	* )&Task1_TaskTCB,		
				 (CPU_CHAR	* )"Task1 task", 		
                 (OS_TASK_PTR )task1_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )TASK1_TASK_PRIO,     
                 (CPU_STK   * )&TASK1_TASK_STK[0],	
                 (CPU_STK_SIZE)TASK1_STK_SIZE/10,	
                 (CPU_STK_SIZE)TASK1_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);				
				 
	//����TASK2����
	OSTaskCreate((OS_TCB 	* )&Task2_TaskTCB,		
				 (CPU_CHAR	* )"task2 task", 		
                 (OS_TASK_PTR )task2_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )TASK2_TASK_PRIO,     	
                 (CPU_STK   * )&TASK2_TASK_STK[0],	
                 (CPU_STK_SIZE)TASK2_STK_SIZE/10,	
                 (CPU_STK_SIZE)TASK2_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,				
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, 
                 (OS_ERR 	* )&err);			 
	OS_CRITICAL_EXIT();	        //�˳��ٽ���
	OSTaskDel((OS_TCB*)0,&err);	//ɾ��start_task��������
								 
}

//task1������
void task1_task(void *p_arg)
{	
	OS_ERR err;
	OSTaskSuspend((OS_TCB*)&Task2_TaskTCB,&err);
	while(1)
	{
//		OSSemPend(&MY_SEM,0,OS_OPT_PEND_BLOCKING,0,&err); 	//�����ź���
		if(Mk_UsartAll==1)              //���յ�����
		{
			Mk_UsartAll=0;
			OSTaskResume((OS_TCB*)&Task2_TaskTCB,&err);
			//OSSemPost (&MY_SEM,OS_OPT_POST_1,&err);	//�����ź���
			RxCount=0;
			UsartRace_Data();                       //������յ�������
			process();                              //�����˽���
			ClearUart2();
			OSTaskSuspend((OS_TCB*)&Task2_TaskTCB,&err);
		}
	};
}

//task2������
void task2_task(void *p_arg)
{
	//OS_ERR err;
	while(1)
	{
		//OSSemPend(&MY_SEM,0,OS_OPT_PEND_BLOCKING,0,&err); 	//�����ź���
		Tm1624_Display2();
	};
}


//��ʱ��2�жϷ�����
void TIM2_IRQHandler(void)
{
	OSIntEnter();
	if(TIM_GetITStatus(TIM2,TIM_IT_Update)==SET) //����ж�
	{
		if(RxCount!=0)  //�����ݽ���
		{
			if(RxCount == pre_cnt_rs2)    //�������
			{
				tim2_count++;
				if(tim2_count>=4)
				{
					  Mk_UsartAll=1;             //��������������
					  tim2_count=0;
				}
			}
			else
			{
				tim2_count = 0;
				pre_cnt_rs2 = RxCount;
			}
		}
		
	}
	TIM_ClearITPendingBit(TIM2,TIM_IT_Update);  //����жϱ�־λ
	
	OSIntExit();
}


