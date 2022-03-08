#include "DS18B20.h"


 
uint8_t ROM_ID[8];
 
 
/*******************************************
DS18B20	�����߶���ʱ����Ҫ��ǳ��ߣ�����
������ʹ����ϵͳ��શ�ʱ����������׼��ʱ��
DQ :GPIOB_7
*******************************************/
 
 

 
/*���ܣ���DS18B20����һ����λ�ź�*/
void DS18B20_SendRestSingle(void)
{
	/*�������͸�λ�ź�*/
	DS18B20_OUT();
	DQ_Low; //��������480~960 us ���� DS18B20 оƬ���и�λ
	delay_us(750);
	DQ_High;
	delay_us(15);         //�ͷ�����15~60 us
}
 
/*
���ܣ����DS18B20��������
����ֵ��
			0  DS18B20�豸����
			1  DS18B20�豸��Ӧ��λ�ź�ʧ��
			2  DS18B20�豸�ͷ�����ʧ��
*/
uint8_t DS18B20_CheckReadySingle(void)
{
	uint8_t cnt=0;
	/*1.����������*/
	DS18B20_IN();
	while(DQ_READ && cnt < 240) //�ȴ�DS18B20 �������� ��60~240 us ��Ӧ��λ�źţ�
	{
		delay_us(1);
		cnt++;
	}
	if(cnt>240) return 1;
	/*2.���DS18B20�Ƿ��ͷ�����*/	
	cnt=0;
	DS18B20_IN();
	while((!DQ_READ) && cnt<240)  //�ж�DS18B20�Ƿ��ͷ����ߣ�60~240 us ��Ӧ��λ�ź�֮����ͷ����ߣ�
	{
		delay_us(1);
		cnt++;
	}
	if(cnt>240)	return 2;
	else return 0;
}
 
/*
���ܣ����DS18B20�豸�Ƿ�����
����ֵ��
			0  DS18B20�豸����
			1  DS18B20�豸��Ӧ��λ�ź�ʧ��
			2  DS18B20�豸�ͷ�����ʧ��
*/
uint8_t DS18B20_CheckDevice(void)
{
	DS18B20_SendRestSingle();/*1.�������͸�λ�ź�*/
	return DS18B20_CheckReadySingle();/*2.����������*/
}
 
/*���ܣ���DS18B20дһ���ֽ����ݣ����*/
uint8_t DS18B20_WriteByte(uint8_t cmd)
{
	uint8_t i=0;
	DS18B20_OUT();
	for(i=0;i<8;i++)
	{
		DQ_Low;
		delay_us(2);     //������������д����ʱ��϶2us
		cmd&0x01 ? DQ_High : DQ_Low;  //�����д��λΪ1��������15us֮�ڰ��������ߣ�Ϊ0 ����60us���ɡ�
		delay_us(60);    //ȷ��DS18B20�Ѿ��ɹ���ȡ����λ����
		DQ_High;  //һλ�������
		cmd >>=1;
		delay_us(2);     //λ��϶2us
	}
	return 0;
}
 
/*���ܣ���DS18B20��ȡһ���ֽ�����*/
uint8_t DS18B20_ReadByte(void)
{
	uint8_t i,data=0;
	for(i=0;i<8;i++)
	{
		DS18B20_OUT();//��ʼ��Ϊ���ģʽ
		DQ_Low;  
		delay_us(2);  //�����������߶�����ʱ��϶2us		
		DQ_High; //�ͷ����ߣ�׼����ȡλ����
		DS18B20_IN(); //��ʼ��Ϊ����ģʽ
		delay_us(10); //�ȴ�DS18B20���������
		data >>=1 ;  //��λ��0��Ĭ����0Ϊ׼
		if(DQ_READ) data |=0x80;
		delay_us(60); //��ʱȷ��DS18B20���������Ѿ���ȥ���ǳ���Ҫ��
		DQ_High;  //�ͷ�����׼����ȡ��һλλ����
	}
	return data;
}
 
/*
��������: ��ȡһ��DS18B20���¶�����
�� �� ֵ: ��ȡ���¶�����
���ǵ����:  ������ֻ�ǽ���һ��DS18B20�����
*/
uint16_t DS18B20_GetTemperature(void)
{
	  uint16_t temp=0;
		uint8_t temp_H,temp_L;
		int intT,decT;
	  DS18B20_CheckDevice();   //���͸�λ���塢����������
		DS18B20_WriteByte(0xCC); //����ROM���м��
		DS18B20_WriteByte(0x44); //����һ���¶�ת��
		
		//�ȴ��¶�ת�����
		while(DS18B20_ReadByte()!=0xFF){}
		
		DS18B20_CheckDevice();   //���͸�λ���塢����������
		DS18B20_WriteByte(0xCC); //����ROM���м��
		DS18B20_WriteByte(0xBE); //��ȡ�¶�
		
		temp_L=DS18B20_ReadByte(); //��ȡ���¶ȵ�λ����
		temp_H=DS18B20_ReadByte(); //��ȡ���¶ȸ�λ����
		temp=temp_L|(temp_H<<8);   //�ϳ��¶�
		intT = temp>>4 ;          /*�ϳ�ʵ���¶���������****�����������ĸ���*/ 
		decT = temp&0xF ;         /*�ϳ�ʵ���¶�С������*/
		return temp;
}
 
///*���ܣ���DS18B20��ȡ�¶�ֵ,������*/
//uint16_t DS18B20_GetTemperature(void)
//{
//	uint16_t temp;
//	uint8_t TL,TH;
//	DS18B20_SendRestSingle();  										/*1.�������͸�λ�ź�*/
//	if(DS18B20_CheckReadySingle()) return 0xFF;   /*2.����������*/
//	DS18B20_MatchROM();														/*3.ƥ�� ROM ָ�� (���������������ж���豸)*/
//	DS18B20_WriteByte(0xCC);  									/*3.��Ծ ROM ָ�� (��������������ֻ��һ���豸)*/
//	DS18B20_WriteByte(0x44);  										/*4.�����¶�ת��*/
//	while(DS18B20_ReadByte()!=0xFF);							//�ȴ��¶�ת���ɹ�
//	DS18B20_SendRestSingle();										  /*5.�������͸�λ�ź�*/
//	if(DS18B20_CheckReadySingle()) return 0xFF;   /*6.����������*/
//	DS18B20_MatchROM();														/*6.ƥ�� ROM ָ�� (���������������ж���豸)*/
//	DS18B20_WriteByte(0xCC);                    /*7.��Ծ ROM ָ�� (��������������ֻ��һ���豸)*/
//	DS18B20_WriteByte(0xBE);                      /*8.���Ͷ�ȡ�¶�����*/
//	
//	TL = DS18B20_ReadByte();                      /*9.��ȡ�¶ȸߵ�λ��У��λ*/
//	TH = DS18B20_ReadByte();
//	DS18B20_SendRestSingle();                     /*10.�������͸�λ�źţ�ʾ��ֹͣ��ȡʣ�µ�����*/
//	temp = TH<<8 | TL;                            /*11.�ϳ��¶�ֵ*/
//	return temp;
//	
// temp = temp*0.0625;				/*ֱ�Ӻϳ�ʵ���¶�*/
// value = temp * 100 + (value > 0 ? 0.5 : -0.5);	//�����Ŵ�100��, ʹ��ʾʱ����ʾС�������λ, ����С������������4��5��
////		
// intT = temp>>4 ;          /*�ϳ�ʵ���¶���������****�����������ĸ���*/ 
// decT = temp&0xF ;         /*�ϳ�ʵ���¶�С������*/
///*   crc = x^8 + x^5 + x^4 + 1*/	
//}
 
/*���ܣ���DS18B20��ȡROM��Ϣ (ROM_ID= 28-92-AF-AC-17-13-1-F1)*/
uint8_t DS18B20_ReadRomInfo(void)
{
	uint8_t i=0;
	DS18B20_WriteByte(0x33);  /*4.������ȡROM*/
	for(i=0;i<8;i++)
	{
		ROM_ID[i]=DS18B20_ReadByte();
	}
	for(i=0;i<8;i++)
	{
		printf("%X",ROM_ID[i]);
		if(i==7) printf("\n");
		else printf("-");
	}
	return 0;
}
/*
ROM_ID[0]=0x28
ROM_ID[1]=0x92
ROM_ID[2]=0xAF
ROM_ID[3]=0xAC
ROM_ID[4]=0x17
ROM_ID[5]=0x13
ROM_ID[6]=0x1
ROM_ID[7]=0xF1
*/
 
/*���ܣ�ƥ�� DS18B20  ROM��Ϣ*/
uint8_t DS18B20_MatchROM(void)
{
	uint8_t i=0;
	DS18B20_WriteByte(0x55);  /*4.ƥ��64λ ROM ��Ϣ*/
	for(i=0;i<8;i++)
	{
		DS18B20_WriteByte(ROM_ID[i]);
	}
	return 0;
}
 
 