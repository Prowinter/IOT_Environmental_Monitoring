/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
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
#include "stdint.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
typedef struct {
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
	unsigned char Illumination;		//光照强度
	unsigned char Worm;						//虫害数据
	unsigned char air_check_sum;	//校验和
}Monitor_Data_TypeDef;
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
unsigned short int MODBUS_CRC16_v3( const unsigned char *buf, unsigned int len );
void AND_Report(unsigned char function,Monitor_Data_TypeDef *monitor,unsigned char len);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define DS18B20_Pin GPIO_PIN_0
#define DS18B20_GPIO_Port GPIOB
#define DHT11_Pin GPIO_PIN_1
#define DHT11_GPIO_Port GPIOB
#define Worm_Pin GPIO_PIN_11
#define Worm_GPIO_Port GPIOB
#define LED_Pin GPIO_PIN_12
#define LED_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */
#define Worm_Get (HAL_GPIO_ReadPin(Worm_GPIO_Port,Worm_Pin))
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
