/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "DHT11.h"
#include "DS18B20.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
Monitor_Data_TypeDef monitor;
extern unsigned char PH_RX_BUF[];
extern unsigned short int PH_RX_STA;
unsigned char PH_DATA[8] = {0x01,0x03,0x00,0x08,0x00,0x01,0x05,0xc8};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif
PUTCHAR_PROTOTYPE
{
    // HAL库函数版本
    // HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1 , 0xffff);
    // return ch;

    // 寄存器版本
    while ((USART2->SR & 0X40) == 0);
    USART2->DR = (uint8_t) ch;
    return ch;
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	int soil_temp = 0;
	float air_temp=0,soil_humi=0;
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_ADC1_Init();
  MX_TIM2_Init();
  MX_TIM1_Init();
  /* USER CODE BEGIN 2 */
	HAL_ADCEx_Calibration_Start(&hadc1);    //AD校准
	HAL_TIM_Base_Start_IT(&htim1);
	HAL_UART_Receive_IT(&huart1, (uint8_t *)PH_RX_BUF, 1);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		// 温湿度数据获取
		Read_DHT11(&monitor);
		
		HAL_ADC_Start(&hadc1);
		HAL_ADC_PollForConversion(&hadc1,50);//等待ADC转换完成
		monitor.Illumination = HAL_ADC_GetValue(&hadc1)*100/4096;

		
		HAL_ADC_Start(&hadc1);
		HAL_ADC_PollForConversion(&hadc1,50);//等待ADC转换完成
		soil_humi = HAL_ADC_GetValue(&hadc1)*3.3f/4096;
		HAL_ADC_Stop(&hadc1);
		
		HAL_Delay(1000);
		float_to_int_deci(&monitor,soil_humi);
		monitor.Worm= Worm_Get?1:0;
		if(DS18B20_CheckDevice() == 0)
		{
			
			soil_temp = DS18B20_GetTemperature();
			monitor.soiltemp_int = soil_temp>>4 ;          /*合成实际温度整数部分****精度相对上面的更高*/ 
			monitor.soiltemp_deci = soil_temp&0xF ;         /*合成实际温度小数部分*/
		}
		
		if(PH_RX_STA ==  0x0007)
		{
//			printf("PH_MODEL_OK!\r\n");
			unsigned short int PH = 0;
			PH = (unsigned short int)PH_RX_BUF[3]<<8 | PH_RX_BUF[4];
			monitor.ph_int = PH/10;
			monitor.ph_deci = PH%10;
			PH_RX_STA = 0;
		}
//		printf("Illumination = %d,PH = %d.%d\r\n",monitor.Illumination,monitor.ph_int,monitor.ph_deci);
//		printf("soil_temp = %d.%d,soilhumi = %d.%d\r\n",monitor.soiltemp_int,monitor.soiltemp_deci,monitor.soilhumi_int,monitor.soilhumi_deci);
//		printf("air_humi = %d.%d,air_temp = %d.%d\r\n",monitor.air_humi_int,monitor.air_humi_deci,monitor.air_temp_int,monitor.air_temp_deci);
		
		
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void float_to_int_deci(Monitor_Data_TypeDef *monitor,float soil_humi)
{
	monitor->soilhumi_int = (int)soil_humi;
	monitor->soilhumi_deci = (int)(soil_humi*100)%100;
}




/*
 * 一次完整的数据传输为40bit，高位先出
 * 8bit 湿度整数 + 8bit 湿度小数 + 8bit 温度整数 + 8bit 温度小数 + 8bit 校验和 
 */
uint8_t Read_DHT11(Monitor_Data_TypeDef *monitor)
{  
	DHT11_Start();
	DAT_IN();

	/*判断从机是否有低电平响应信号 如不响应则跳出，响应则向下运行*/   
	if(HAL_GPIO_ReadPin(DHT11_GPIO_Port, DHT11_DAT_GPIO_Pin) == 0)     
	{
		/*轮询直到从机发出 的50us 低电平 响应信号结束*/  
		while(HAL_GPIO_ReadPin(DHT11_GPIO_Port, DHT11_DAT_GPIO_Pin)==0);

		/*轮询直到从机发出的 30us 高电平 标置信号结束*/
		while(HAL_GPIO_ReadPin(DHT11_GPIO_Port, DHT11_DAT_GPIO_Pin)==1);
		/*开始接收数据*/   
		monitor->air_humi_int= Read_Byte();

		monitor->air_humi_deci= Read_Byte();

		monitor->air_temp_int= Read_Byte();

		monitor->air_temp_deci= Read_Byte();

		monitor->air_check_sum= Read_Byte();


		/*读取结束，引脚改为输出模式*/
		DAT_OUT();
		/*主机拉高*/
		HAL_GPIO_WritePin(DHT11_GPIO_Port, DHT11_DAT_GPIO_Pin, GPIO_PIN_SET);	

		/*检查读取的数据是否正确*/
		if(monitor->air_check_sum == monitor->air_humi_int + monitor->air_humi_deci + monitor->air_temp_int+ monitor->air_temp_deci)
			return SUCCESS;
		else 
			return ERROR;
	}
	else
	{		
		return ERROR;
	} 
  
	DHT11_Stop();
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
