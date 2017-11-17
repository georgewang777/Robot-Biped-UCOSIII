/************************************************
Author: George Wang
Email:  georgewang.7sl@gmail.com
************************************************/
#include "george_system.h"

extern u8 RxCount;    //Index
u8 Mk_UsartAll=0;	   //接收一组数据完成标志
u8 led_flag = 0;         //灯光开关标志位 1 开 0 关
u8 pre_cnt_rs2=0;    //数据标志
u8 tim2_count;    	//定时器2计数标志
/**************UCOSII任务设置************************/

//UCOSIII中以下优先级用户程序不能使用，ALIENTEK
//将这些优先级分配给了UCOSIII的5个系统内部任务
//优先级0：中断服务服务管理任务 OS_IntQTask()
//优先级1：时钟节拍任务 OS_TickTask()
//优先级2：定时任务 OS_TmrTask()
//优先级OS_CFG_PRIO_MAX-2：统计任务 OS_StatTask()
//优先级OS_CFG_PRIO_MAX-1：空闲任务 OS_IdleTask()

#define START_TASK_PRIO		3            //任务优先级
#define START_STK_SIZE 		128          //任务堆栈大小
OS_TCB StartTaskTCB;                   //任务控制块
CPU_STK START_TASK_STK[START_STK_SIZE];//任务堆栈	
void start_task(void *p_arg);          //任务函数

#define TASK1_TASK_PRIO		5            //任务优先级
#define TASK1_STK_SIZE 		128          //任务堆栈大小	
OS_TCB Task1_TaskTCB;                  //任务控制块
CPU_STK TASK1_TASK_STK[TASK1_STK_SIZE];//任务堆栈	
void task1_task(void *p_arg);          //任务函数

#define TASK2_TASK_PRIO		4            //任务优先级
#define TASK2_STK_SIZE 		128          //任务堆栈大小	
OS_TCB Task2_TaskTCB;                  //任务控制块
CPU_STK TASK2_TASK_STK[TASK2_STK_SIZE];//任务堆栈	
void task2_task(void *p_arg);          //任务函数

OS_SEM MY_SEM;                         //信号量，用于访问共享区资源
//主函数
int main(void)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	SysInit();
	OSInit(&err);		      //初始化UCOSIII
	OS_CRITICAL_ENTER();	//进入临界区			 
	//创建开始任务
	OSTaskCreate((OS_TCB 	* )&StartTaskTCB,	    	   //任务控制块
				 (CPU_CHAR	* )"start task", 		           //任务名字
                 (OS_TASK_PTR )start_task, 			   //任务函数
                 (void		* )0,					           //传递给任务函数的参数
                 (OS_PRIO	  )START_TASK_PRIO,      //任务优先级
                 (CPU_STK   * )&START_TASK_STK[0], //任务堆栈基地址
                 (CPU_STK_SIZE)START_STK_SIZE/10,	 //任务堆栈深度限位
                 (CPU_STK_SIZE)START_STK_SIZE,		 //任务堆栈大小
                 (OS_MSG_QTY  )0,					         //任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
                 (OS_TICK	  )0,					           //当使能时间片轮转时的时间片长度，为0时为默认长度，
                 (void   	* )0,					           //用户补充的存储区
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, //任务选项
                 (OS_ERR 	* )&err);				          //存放该函数错误时的返回值
	OS_CRITICAL_EXIT();	//退出临界区	 
	OSStart(&err);      //开启UCOSIII
}

//开始任务任务函数
void start_task(void *p_arg)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	p_arg = p_arg;

	CPU_Init();
	
#if OS_CFG_STAT_TASK_EN > 0u
   OSStatTaskCPUUsageInit(&err);  	//统计任务                
#endif
	
#ifdef CPU_CFG_INT_DIS_MEAS_EN		//如果使能了测量中断关闭时间
    CPU_IntDisMeasMaxCurReset();	
#endif
	
#if	OS_CFG_SCHED_ROUND_ROBIN_EN  //当使用时间片轮转的时候
  //使能时间片轮转调度功能,时间片长度为1个系统时钟节拍，既1*5=5ms
	OSSchedRoundRobinCfg(DEF_ENABLED,1,&err);  
#endif	
	
	OS_CRITICAL_ENTER();	//进入临界区
	//创建一个信号量
//	OSSemCreate ((OS_SEM*	)&MY_SEM,
//                 (CPU_CHAR*	)"MY_SEM",
//                 (OS_SEM_CTR)0,		
//                 (OS_ERR*	)&err);
	//创建TASK1任务
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
				 
	//创建TASK2任务
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
	OS_CRITICAL_EXIT();	        //退出临界区
	OSTaskDel((OS_TCB*)0,&err);	//删除start_task任务自身
								 
}

//task1任务函数
void task1_task(void *p_arg)
{	
	OS_ERR err;
	OSTaskSuspend((OS_TCB*)&Task2_TaskTCB,&err);
	while(1)
	{
//		OSSemPend(&MY_SEM,0,OS_OPT_PEND_BLOCKING,0,&err); 	//请求信号量
		if(Mk_UsartAll==1)              //接收到数据
		{
			Mk_UsartAll=0;
			OSTaskResume((OS_TCB*)&Task2_TaskTCB,&err);
			//OSSemPost (&MY_SEM,OS_OPT_POST_1,&err);	//发送信号量
			RxCount=0;
			UsartRace_Data();                       //处理接收到的数据
			process();                              //机器人进程
			ClearUart2();
			OSTaskSuspend((OS_TCB*)&Task2_TaskTCB,&err);
		}
	};
}

//task2任务函数
void task2_task(void *p_arg)
{
	//OS_ERR err;
	while(1)
	{
		//OSSemPend(&MY_SEM,0,OS_OPT_PEND_BLOCKING,0,&err); 	//请求信号量
		Tm1624_Display2();
	};
}


//定时器2中断服务函数
void TIM2_IRQHandler(void)
{
	OSIntEnter();
	if(TIM_GetITStatus(TIM2,TIM_IT_Update)==SET) //溢出中断
	{
		if(RxCount!=0)  //有数据接收
		{
			if(RxCount == pre_cnt_rs2)    //接收完成
			{
				tim2_count++;
				if(tim2_count>=4)
				{
					  Mk_UsartAll=1;             //交给主函数处理
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
	TIM_ClearITPendingBit(TIM2,TIM_IT_Update);  //清除中断标志位
	
	OSIntExit();
}


