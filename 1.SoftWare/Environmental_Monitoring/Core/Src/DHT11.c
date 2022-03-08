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
		DAT_OUT(); 		//����io��Ϊ���ģʽ
    HAL_GPIO_WritePin(DHT11_GPIO_Port, DHT11_DAT_GPIO_Pin, GPIO_PIN_RESET);	
    HAL_Delay(30);    		//��������18ms
    HAL_GPIO_WritePin(DHT11_GPIO_Port, DHT11_DAT_GPIO_Pin, GPIO_PIN_SET);	
		delay_us(30);					//��ʱ30us
}

void DHT11_Stop()	   
{              
		DAT_OUT(); 		//����io��Ϊ���ģʽ
    HAL_GPIO_WritePin(DHT11_GPIO_Port, DHT11_DAT_GPIO_Pin, GPIO_PIN_SET);	
}



/* 
 * ��DHT11��ȡһ���ֽڣ�MSB����
 */
uint8_t Read_Byte(void)
{
	uint8_t i, temp=0;

	for(i=0;i<8;i++)    
	{	 
		/*ÿbit��50us�͵�ƽ���ÿ�ʼ����ѯֱ���ӻ����� ��50us �͵�ƽ ����*/  
		while(HAL_GPIO_ReadPin(DHT11_GPIO_Port, DHT11_DAT_GPIO_Pin)==0);

		/*DHT11 ��26~28us�ĸߵ�ƽ��ʾ��0������70us�ߵ�ƽ��ʾ��1����
		 *ͨ����� x us��ĵ�ƽ��������������״ ��x ���������ʱ 
		 */
		delay_us(40); //��ʱx us �����ʱ��Ҫ��������0������ʱ�伴��	   	  

		if(HAL_GPIO_ReadPin(DHT11_GPIO_Port, DHT11_DAT_GPIO_Pin)==1)/* x us����Ϊ�ߵ�ƽ��ʾ���ݡ�1�� */
		{
			/* �ȴ�����1�ĸߵ�ƽ���� */
			while(HAL_GPIO_ReadPin(DHT11_GPIO_Port, DHT11_DAT_GPIO_Pin)==1);

			temp|=(uint8_t)(0x01<<(7-i));  //�ѵ�7-iλ��1��MSB���� 
		}
		else	 // x us��Ϊ�͵�ƽ��ʾ���ݡ�0��
		{			   
			temp&=(uint8_t)~(0x01<<(7-i)); //�ѵ�7-iλ��0��MSB����
		}
	}
	return temp;
}