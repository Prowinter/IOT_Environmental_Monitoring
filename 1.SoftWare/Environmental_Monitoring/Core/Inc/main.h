/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
typedef struct{
	unsigned char soiltemp_int;		//�����¶ȵ���������
	unsigned char soiltemp_deci;	//�����¶ȵ�С������
	unsigned char soilhumi_int;		//����ʪ�ȵ���������
	unsigned char soilhumi_deci;	//����ʪ�ȵ�С������
	unsigned char ph_int;					//���ȵ���������
	unsigned char ph_deci;				//���ȵ�С������
	unsigned char air_humi_int;		//����ʪ�ȵ���������
	unsigned char air_humi_deci;	//����ʪ�ȵ�С������
	unsigned char air_temp_int;	 	//�����¶ȵ���������
	unsigned char air_temp_deci;	//�����¶ȵ�С������
	unsigned char Illumination;						//����ǿ��
	unsigned char Worm;						//�溦����
	unsigned char air_check_sum;	//У���
}Monitor_Data_TypeDef;
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
uint8_t Read_DHT11(Monitor_Data_TypeDef *monitor);
void float_to_int_deci(Monitor_Data_TypeDef *monitor,float soil_humi);
/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define DS18B20_Pin GPIO_PIN_0
#define DS18B20_GPIO_Port GPIOB
#define DHT11_Pin GPIO_PIN_1
#define DHT11_GPIO_Port GPIOB
#define Worm_Pin GPIO_PIN_11
#define Worm_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */
#define Worm_Get (HAL_GPIO_ReadPin(Worm_GPIO_Port,Worm_Pin))
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
