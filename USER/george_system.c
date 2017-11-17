#include "george_system.h"
extern u8 Pulse1,Pulse2,Pulse3,Pulse4;
extern u8 led_flag; 
//extern OS_EVENT *msg_action; 
u8 status;
extern char RxBuffer[15];
void SysInit(void)
{
	SystemInit();          //����ϵͳʱ��Ϊ72M
	delay_init();	    	   //��ʱ������ʼ��	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(115200);
	uart2_init(115200);
	Pulse1 = 140;
	Pulse2 = 130;
	Pulse3 = 150;
	Pulse4 = 130;
  TIM2_Int_Init(50-1,7200-1);//��ʱ��ʱ��72M����Ƶϵ��7200������72M/7200=10Khz�ļ���Ƶ�ʣ�����50��Ϊ5ms  
  Servo_Init();  
	Step_Motor_GPIO_Init();
	Tm1624_Init();
	//WKUP_Init();              //��������
}

void process(void)
{
//	if(status!= 0)
//	{
//		OSMboxPost(msg_action,(void*)1); //������Ϣ
//	}
	//switch(status)
	switch(RxBuffer[2])
	{
		case 0x01: printf("zhanshen1\n");Forward();break;    //ǰ��
		case 0x02: printf("zhanshen2\n");Backward();break;   //����
		case 0x03: printf("zhanshen3\n");Turn_Left();break; //��ת
		case 0x04: printf("zhanshen4\n");Turn_Right();break;  //��ת
		case 0x05: printf("zhanshen5\n");Shake();     break;  //����
		case 0x06:printf("zhanshen6\n");Dance();     break;  //����
		case 0x07: printf("zhanshen7\n");Head_Action();break;//ͷ��
		//case 8: led_flag = 1; break;
		//case 9: led_flag = 0; break;
		//default: RxBuffer[2] = 0;break;
		default: status = 0; break;
	}
	//OSMboxPost(msg_action,(void*)0); //������Ϣ
}
