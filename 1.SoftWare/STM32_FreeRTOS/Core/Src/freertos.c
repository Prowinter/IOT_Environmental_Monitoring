/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
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

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "event_groups.h"
#include "DHT11.h"
#include "DS18B20.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define ADC_FLAG      0x01
#define DHT11_FLAG    0x02
#define PH_FLAG       0x04
#define DS18B20_FLAG  0x08
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
extern EventGroupHandle_t EvenGroup;
extern ADC_HandleTypeDef hadc1;
extern unsigned char PH_RX_BUF[];
extern unsigned short int PH_RX_STA;
Monitor_Data_TypeDef Monitor_Data;
/* USER CODE END Variables */
osThreadId StatuTaskHandle;
osThreadId InfoUpdataHandle;
osThreadId ADCHandle;
osThreadId DHT11Handle;
osThreadId PHHandle;
osThreadId DS18B20Handle;
osMessageQId Uart1_QueueHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
extern void float_to_int_deci(Monitor_Data_TypeDef *monitor,float soil_humi);
extern uint8_t Read_DHT11(Monitor_Data_TypeDef *monitor);
/* USER CODE END FunctionPrototypes */

void StatuHelloTask(void const * argument);
void InfoUpdataTask(void const * argument);
void ADCTask(void const * argument);
void DHT11Task(void const * argument);
void PHTask(void const * argument);
void DS18B20Task(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* definition and creation of Uart1_Queue */
  osMessageQDef(Uart1_Queue, 16, uint8_t);
  Uart1_QueueHandle = osMessageCreate(osMessageQ(Uart1_Queue), NULL);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of StatuTask */
  osThreadDef(StatuTask, StatuHelloTask, osPriorityNormal, 0, 128);
  StatuTaskHandle = osThreadCreate(osThread(StatuTask), NULL);

  /* definition and creation of InfoUpdata */
  osThreadDef(InfoUpdata, InfoUpdataTask, osPriorityNormal, 0, 256);
  InfoUpdataHandle = osThreadCreate(osThread(InfoUpdata), NULL);

  /* definition and creation of ADC */
  osThreadDef(ADC, ADCTask, osPriorityNormal, 0, 256);
  ADCHandle = osThreadCreate(osThread(ADC), NULL);

  /* definition and creation of DHT11 */
  osThreadDef(DHT11, DHT11Task, osPriorityNormal, 0, 256);
  DHT11Handle = osThreadCreate(osThread(DHT11), NULL);

  /* definition and creation of PH */
  osThreadDef(PH, PHTask, osPriorityNormal, 0, 256);
  PHHandle = osThreadCreate(osThread(PH), NULL);

  /* definition and creation of DS18B20 */
  osThreadDef(DS18B20, DS18B20Task, osPriorityNormal, 0, 256);
  DS18B20Handle = osThreadCreate(osThread(DS18B20), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StatuHelloTask */
/**
  * @brief  Function implementing the StatuTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StatuHelloTask */
void StatuHelloTask(void const * argument)
{
  /* USER CODE BEGIN StatuHelloTask */
  /* Infinite loop */
  for(;;)
  {
    HAL_GPIO_TogglePin(LED_GPIO_Port,LED_Pin);
    osDelay(1000);
  }
  /* USER CODE END StatuHelloTask */
}

/* USER CODE BEGIN Header_InfoUpdataTask */
/**
* @brief Function implementing the InfoUpdata thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_InfoUpdataTask */
void InfoUpdataTask(void const * argument)
{
  /* USER CODE BEGIN InfoUpdataTask */
  /* Infinite loop */
  for(;;)
  {
    xEventGroupWaitBits(EvenGroup,ADC_FLAG | DHT11_FLAG | PH_FLAG | DS18B20_FLAG, pdTRUE, pdTRUE, portMAX_DELAY);
    AND_Report(0xAA,&Monitor_Data,18);
    // printf_("InfoUpdataTask\r\n");
    // osDelay(1000);
  }
  /* USER CODE END InfoUpdataTask */
}

/* USER CODE BEGIN Header_ADCTask */
/**
* @brief Function implementing the ADC thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_ADCTask */
void ADCTask(void const * argument)
{
  /* USER CODE BEGIN ADCTask */
  /* Infinite loop */
  for(;;)
  {
    xEventGroupSetBits(EvenGroup,ADC_FLAG);
		HAL_ADC_Start(&hadc1);
		HAL_ADC_PollForConversion(&hadc1,50);
		Monitor_Data.Illumination = HAL_ADC_GetValue(&hadc1)*100/4096;
		HAL_ADC_Start(&hadc1);
		HAL_ADC_PollForConversion(&hadc1,50);
		float soil_humi = HAL_ADC_GetValue(&hadc1)*3.3f/4096;
    float_to_int_deci(&Monitor_Data,soil_humi);
		HAL_ADC_Stop(&hadc1);

    //虫害
    Monitor_Data.Worm = Worm_Get ? 1 : 0;
    // printf_("ADCTask\r\n");
    // osDelay(1000);
  }
  /* USER CODE END ADCTask */
}

/* USER CODE BEGIN Header_DHT11Task */
/**
* @brief Function implementing the DHT11 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_DHT11Task */
void DHT11Task(void const * argument)
{
  /* USER CODE BEGIN DHT11Task */
  /* Infinite loop */
  for(;;)
  {
    xEventGroupSetBits(EvenGroup,DHT11_FLAG);
    Read_DHT11(&Monitor_Data);
    // printf_("DHT11Task\r\n");
    // osDelay(1000);
  }
  /* USER CODE END DHT11Task */
}

/* USER CODE BEGIN Header_PHTask */
/**
* @brief Function implementing the PH thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_PHTask */
void PHTask(void const * argument)
{
  /* USER CODE BEGIN PHTask */
  uint8_t Res;
  /* Infinite loop */
  for(;;)
  {
    if(xQueueReceive(Uart1_QueueHandle,&Res,portMAX_DELAY)!=pdFALSE){
      if(PH_RX_STA == 0)				//	帧头
      {	
        if(Res == 0x01)
        {
          PH_RX_BUF[PH_RX_STA] = Res;
          PH_RX_STA = 0x0001;
        }else PH_RX_STA = 0;
      }else if(PH_RX_STA == 0x0001)	//功能码
      {
        if(Res == 0x03)
        {
          PH_RX_BUF[PH_RX_STA] = Res;
          PH_RX_STA = 0x0002;
        }else PH_RX_STA = 0;
      }else if(PH_RX_STA == 0x0002)	//数据长度
      {
        if(Res == 0x02)
        {
          PH_RX_BUF[PH_RX_STA] = Res;
          PH_RX_STA = 0x0003;
        }else PH_RX_STA = 0;
      }else if(PH_RX_STA == 0x0003)	//PH_1
      {
        PH_RX_BUF[PH_RX_STA] = Res;
        PH_RX_STA = 0x0004;
      }else if(PH_RX_STA == 0x0004)	//PH_2
      {
        PH_RX_BUF[PH_RX_STA] = Res;
        PH_RX_STA = 0x0005;
      }else if(PH_RX_STA == 0x0005)	//Check_1
      {
        PH_RX_BUF[PH_RX_STA] = Res;
        PH_RX_STA = 0x0006;
      }else if(PH_RX_STA == 0x0006)	//Check_2
      {
        PH_RX_BUF[PH_RX_STA] = Res;
        unsigned short int CRC_RETURN = MODBUS_CRC16_v3(PH_RX_BUF, 5);
        if((PH_RX_BUF[PH_RX_STA] == CRC_RETURN>>8) && (PH_RX_BUF[PH_RX_STA-1] == (CRC_RETURN&0x00FF)))
        {
            unsigned short int PH = 0;
            PH = (unsigned short int)PH_RX_BUF[3]<<8 | PH_RX_BUF[4];
            Monitor_Data.ph_int = PH/10;
            Monitor_Data.ph_deci = PH%10;
            PH_RX_STA = 0;
            xEventGroupSetBits(EvenGroup,PH_FLAG);
        }else PH_RX_STA=0;
      }
    }
    // printf_("PHTask\r\n");
  }
  /* USER CODE END PHTask */
}

/* USER CODE BEGIN Header_DS18B20Task */
/**
* @brief Function implementing the DS18B20 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_DS18B20Task */
void DS18B20Task(void const * argument)
{
  /* USER CODE BEGIN DS18B20Task */
  /* Infinite loop */
  for(;;)
  {
    xEventGroupSetBits(EvenGroup,DS18B20_FLAG);
		if(DS18B20_CheckDevice() == 0)
		{
			int soil_temp = 0;
			soil_temp = DS18B20_GetTemperature();
			Monitor_Data.soiltemp_int = soil_temp>>4 ;           /*合成实际温度整数部分*/ 
			Monitor_Data.soiltemp_deci = soil_temp&0xF ;         /*合成实际温度小数部分*/
		}
    printf_("DS18B20Task\r\n");
    osDelay(1000);
  }
  /* USER CODE END DS18B20Task */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

