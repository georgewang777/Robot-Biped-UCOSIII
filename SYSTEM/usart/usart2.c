#include "sys.h"
#include "usart2.h"
#include "george_system.h"
void uart2_init(u32 bound)
{
	 //GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

	 //USART2_TX   PA.2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//USART2_RX	  PA.3
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);  
	//Usart1 NVIC ����

	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//

	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���USART1

	//USART ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;//һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_Init(USART2, &USART_InitStructure);

	//ʹ�ܴ��ڽ����ж�
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); 

	USART_Cmd(USART2, ENABLE);                    //ʹ�ܴ��� 
}
/*SEND ONE BYTE*/
void UART2_send_byte(uint8_t byte) //
{
	while(!((USART2->SR)&(1<<7)));//�ȴ�������
	USART2->DR=byte; //����һ���ֽ�
}

/*SEND A BUFFER*/
void UART2_Send(uint8_t *Buffer, uint32_t Length)
{
	while(Length != 0)
	{
		while(!((USART2->SR)&(1<<7)));//�ȴ�������
		USART2->DR= *Buffer;
		Buffer++;
		Length--;
	}
}
//�����ַ�������
void USART2_Puts(char * str)
{
	while (*str)
	{
		USART_SendData(USART2, *str++);

		/* Loop until the end of transmission */
		while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET); //���Ӣ�Ĳο���521ҳ����TXE������ʱ��һ֡���ݴ������
	//	USART_ClearFlag(USART2,USART_FLAG_TC);
	}
}
char RxBuffer[15];
u8 RxCount;
extern u8 status;
void WriteUart2Buff(char ch)
{
	 RxBuffer[RxCount]=ch;
	 RxCount++;
	if(RxCount>=15) 
	{
		RxCount=0;
	}
}

void USART2_IRQHandler(void)                	//����1�жϷ������
{
	OSIntEnter();
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
	{
	  WriteUart2Buff(USART_ReceiveData(USART2));//��ȡ���յ������ݣ�д������		
		USART_ClearFlag(USART2,USART_FLAG_RXNE);	
  } 
	OSIntExit();
} 
/*********�������2�������ݻ���*****************/
void ClearUart2(void)  //
{
	unsigned char i;
	for(i = 0; i <10;i++)
	{
		 RxBuffer[i] = 0;
	}
}
int strcmp_str(char *str1,char *str2,int count)
{
	int i=0;
	while(*str1 == *str2)
	{
		i++;
		if(i==count)
		{
			return 0;
		}
		str1++;
		str2++;
	}
	return -1;
}

//�������ݽ���
void UsartRace_Data(void)
{
//	if(RxBuffer[0]==0x59 && RxBuffer[1]==0x48 &&RxBuffer[3]==0x5A && RxBuffer[4]==0x49)
//	{
//		process();                    //�����˽���
////		switch(RxBuffer[2])
////		{
////			case 0x01:status = 1;break;
////			case 0x02:status = 2;break;
////			case 0x03:status = 3;break;
////			case 0x04:status = 4;break;
////			case 0x05:status = 5;break;
////			case 0x06:status = 6;break;
////			case 0x07:status = 7;break;
//		}
	//}
	if(!(strcmp_str(RxBuffer,"Forward",6)))  //ǰ��
	{
		status = 1;
		//angle4 = (RxBuffer[4]-'0')*10 + (RxBuffer[5]-'0');
		//dir4 = RxBuffer[3]-'0';
	}
	else if(!(strcmp_str(RxBuffer,"Backward",8))) //����
	{
		status =2;
	}
	else if(!(strcmp_str(RxBuffer,"TurnRight",5))) // ��ת
	{
		status =3;
	}
	else if(!(strcmp_str(RxBuffer,"TurnLeft",8))) // ��ת
	{
		status =4;
	}
	else if(!(strcmp_str(RxBuffer,"Dance",5)))   // ��ת
	{
		status =5;
	}
	else if(!(strcmp_str(RxBuffer,"Shake",5)))   // ����
	{
		status =6;
	}
	else if(!(strcmp_str(RxBuffer,"Head",4)))   // ����
	{
		status =7;
	}
//	else if(!(strcmp_str(RxBuffer,"LedOn",5)))   // ����
//	{
//		status =8;
//	}
//	else if(!(strcmp_str(RxBuffer,"LedOff",5)))   // ����
//	{
//		status =9;
//	}
	else
	{
		status = 0;
	}
}
