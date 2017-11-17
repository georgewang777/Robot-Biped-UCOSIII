#ifndef __GEORGE_SYSTEM_H
#define __GEORGE_SYSTEM_H

#include "timer.h"
#include "usart.h"
#include "usart2.h"
#include "sys.h"
#include "delay.h"
#include "servo.h"
#include "stepper.h"
#include "tm1624.h"
#include "robot.h"
#include "wkup.h"
#include "led.h"
#include "includes.h"



void SysInit(void);    //ϵͳ��ʼ������

void process(void); //Robot����

#endif
