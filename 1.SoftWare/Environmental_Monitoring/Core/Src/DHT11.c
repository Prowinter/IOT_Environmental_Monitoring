#include "DHT11.h"

void delay_us(unsigned short int nus)
{
	__HAL_TIM_SetCounter(&DLY_TIM_Handle,0);
    HAL_TIM_Base_Start(&DLY_TIM_Handle);
    while(__HAL_TIM_GetCounter(&DLY_TIM_Handle)<nus);
    HAL_TIM_Base_Stop(&DLY_TIM_Handle);
}

void DHT11_Start()	   
{              
	DAT_OUT(); 		//配置io口为输出模式
    HAL_GPIO_WritePin(DHT11_GPIO_Port, DHT11_DAT_GPIO_Pin, GPIO_PIN_RESET);	
    HAL_Delay(30);    		//拉低至少18ms
    HAL_GPIO_WritePin(DHT11_GPIO_Port, DHT11_DAT_GPIO_Pin, GPIO_PIN_SET);	
	delay_us(30);					//延时30us
}

void DHT11_Stop()	   
{              
	DAT_OUT(); 		//配置io口为输出模式
    HAL_GPIO_WritePin(DHT11_GPIO_Port, DHT11_DAT_GPIO_Pin, GPIO_PIN_SET);	
}



/* 
 * 从DHT11读取一个字节，MSB先行
 */
uint8_t Read_Byte(void)
{
	uint8_t i, temp=0;

	for(i=0;i<8;i++)    
	{	 
		/*每bit以50us低电平标置开始，轮询直到从机发出 的50us 低电平 结束*/  
		while(HAL_GPIO_ReadPin(DHT11_GPIO_Port, DHT11_DAT_GPIO_Pin)==0);

		/*DHT11 以26~28us的高电平表示“0”，以70us高电平表示“1”，
		 *通过检测 x us后的电平即可区别这两个状 ，x 即下面的延时 
		 */
		delay_us(40); //延时x us 这个延时需要大于数据0持续的时间即可	   	  

		if(HAL_GPIO_ReadPin(DHT11_GPIO_Port, DHT11_DAT_GPIO_Pin)==1)/* x us后仍为高电平表示数据“1” */
		{
			/* 等待数据1的高电平结束 */
			while(HAL_GPIO_ReadPin(DHT11_GPIO_Port, DHT11_DAT_GPIO_Pin)==1);

			temp|=(uint8_t)(0x01<<(7-i));  //把第7-i位置1，MSB先行 
		}
		else	 // x us后为低电平表示数据“0”
		{			   
			temp&=(uint8_t)~(0x01<<(7-i)); //把第7-i位置0，MSB先行
		}
	}
	return temp;
}