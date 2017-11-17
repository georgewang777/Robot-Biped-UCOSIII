#include "george_system.h"
extern u8 Pulse1,Pulse2,Pulse3,Pulse4;
extern u8 led_flag; 
//extern OS_EVENT *msg_action; 
u8 status;
extern char RxBuffer[15];
void SysInit(void)
{
	SystemInit();          //配置系统时钟为72M
	delay_init();	    	   //延时函数初始化	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
	uart_init(115200);
	uart2_init(115200);
	Pulse1 = 140;
	Pulse2 = 130;
	Pulse3 = 150;
	Pulse4 = 130;
  TIM2_Int_Init(50-1,7200-1);//定时器时钟72M，分频系数7200，所以72M/7200=10Khz的计数频率，计数50次为5ms  
  Servo_Init();  
	Step_Motor_GPIO_Init();
	Tm1624_Init();
	//WKUP_Init();              //待机唤醒
}

void process(void)
{
//	if(status!= 0)
//	{
//		OSMboxPost(msg_action,(void*)1); //发送消息
//	}
	//switch(status)
	switch(RxBuffer[2])
	{
		case 0x01: printf("zhanshen1\n");Forward();break;    //前进
		case 0x02: printf("zhanshen2\n");Backward();break;   //后退
		case 0x03: printf("zhanshen3\n");Turn_Left();break; //右转
		case 0x04: printf("zhanshen4\n");Turn_Right();break;  //左转
		case 0x05: printf("zhanshen5\n");Shake();     break;  //跳舞
		case 0x06:printf("zhanshen6\n");Dance();     break;  //抖脚
		case 0x07: printf("zhanshen7\n");Head_Action();break;//头部
		//case 8: led_flag = 1; break;
		//case 9: led_flag = 0; break;
		//default: RxBuffer[2] = 0;break;
		default: status = 0; break;
	}
	//OSMboxPost(msg_action,(void*)0); //发送消息
}
