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
	unsigned char soiltemp_int;		//土壤温度的整数部分
	unsigned char soiltemp_deci;	//土壤温度的小数部分
	unsigned char soilhumi_int;		//土壤湿度的整数部分
	unsigned char soilhumi_deci;	//土壤湿度的小数部分
	unsigned char ph_int;					//酸碱度的整数部分
	unsigned char ph_deci;				//酸碱度的小数部分
	unsigned char air_humi_int;		//空气湿度的整数部分
	unsigned char air_humi_deci;	//空气湿度的小数部分
	unsigned char air_temp_int;	 	//空气温度的整数部分
	unsigned char air_temp_deci;	//空气温度的小数部分
	unsigned char Illumination;						//光照强度
	unsigned char Worm;						//虫害数据
	unsigned char air_check_sum;	//校验和
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
