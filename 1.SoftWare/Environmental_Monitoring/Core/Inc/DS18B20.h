#ifndef __DS18B20_H
#define __DS18B20_H
#include "DHT11.h"  //1usÑÓÊ±
#include "main.h"
#include "stdint.h"


#define GPIOB_MODER_L            *(unsigned int*)(GPIOB_BASE+0x00)
#define GPIOB_MODER_H            *(unsigned int*)(GPIOB_BASE+0x04)

#define DS18B20_DAT_GPIOx GPIOB
#define DS18B20_DAT_Pin_Num 0
#define DS18B20_DAT_GPIO_Pin GPIO_PIN_1
#define Get_DS18B20_DAT_Pin_Num (DS18B20_DAT_Pin_Num>7?DS18B20_DAT_Pin_Num-8:DS18B20_DAT_Pin_Num)


#define DS18B20_IN()	{GPIOB_MODER_L&=0XFFFFFFF0;GPIOB_MODER_L|=8<<(4*Get_DS18B20_DAT_Pin_Num);}
#define DS18B20_OUT()	{GPIOB_MODER_L&=0XFFFFFFF0;GPIOB_MODER_L|=3<<(4*Get_DS18B20_DAT_Pin_Num);}


 
 
uint8_t DS18B20_CheckDevice(void);
void DS18B20_SendRestSingle(void);
uint8_t DS18B20_CheckReadySingle(void);
uint8_t DS18B20_WriteByte(uint8_t cmd);
uint8_t DS18B20_ReadByte(void);
uint16_t DS18B20_GetTemperature(void);
uint8_t DS18B20_ReadRomInfo(void);
uint8_t DS18B20_MatchROM(void);


#define DQ_High HAL_GPIO_WritePin(DS18B20_GPIO_Port,DS18B20_Pin,GPIO_PIN_SET)
#define	DQ_Low HAL_GPIO_WritePin(DS18B20_GPIO_Port,DS18B20_Pin,GPIO_PIN_RESET)
#define DQ_READ HAL_GPIO_ReadPin(DS18B20_GPIO_Port,DS18B20_Pin)
#endif