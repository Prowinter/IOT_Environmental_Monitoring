#ifndef __DHT11_H__
#define __DHT11_H__
#include "main.h"

#define DLY_TIM_Handle  htim2
extern TIM_HandleTypeDef DLY_TIM_Handle;
#define Bit_RESET 0;
#define Bit_SET 1;

#define GPIOB_MODER_L            *(unsigned int*)(GPIOB_BASE+0x00)
#define GPIOB_MODER_H            *(unsigned int*)(GPIOB_BASE+0x04)

#define DHT11_DAT_GPIOx GPIOB
#define DHT11_DAT_Pin_Num 1
#define DHT11_DAT_GPIO_Pin GPIO_PIN_1
#define Get_DAT_Pin_Num (DHT11_DAT_Pin_Num>7?DHT11_DAT_Pin_Num-8:DHT11_DAT_Pin_Num)

#define DAT_IN()	{GPIOB_MODER_L&=0XFFFFFF0F;GPIOB_MODER_L|=8<<(4*Get_DAT_Pin_Num);}
#define DAT_OUT()	{GPIOB_MODER_L&=0XFFFFFF0F;GPIOB_MODER_L|=3<<(4*Get_DAT_Pin_Num);}


void delay_us(unsigned short int nus);
void DHT11_Start(void);
void DHT11_Stop(void);
uint8_t Read_Byte(void);
#endif

