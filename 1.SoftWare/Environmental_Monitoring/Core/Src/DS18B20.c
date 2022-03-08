#include "DS18B20.h"


 
uint8_t ROM_ID[8];
 
 
/*******************************************
DS18B20	单总线对延时精度要求非常高！！！
本程序使用了系统嘀嗒定时器来当做精准延时。
DQ :GPIOB_7
*******************************************/
 
 

 
/*功能：向DS18B20发送一个复位信号*/
void DS18B20_SendRestSingle(void)
{
	/*主机发送复位信号*/
	DS18B20_OUT();
	DQ_Low; //拉低总线480~960 us ，对 DS18B20 芯片进行复位
	delay_us(750);
	DQ_High;
	delay_us(15);         //释放总线15~60 us
}
 
/*
功能：检测DS18B20存在脉冲
返回值：
			0  DS18B20设备正常
			1  DS18B20设备响应复位信号失败
			2  DS18B20设备释放总线失败
*/
uint8_t DS18B20_CheckReadySingle(void)
{
	uint8_t cnt=0;
	/*1.检测存在脉冲*/
	DS18B20_IN();
	while(DQ_READ && cnt < 240) //等待DS18B20 拉低总线 （60~240 us 响应复位信号）
	{
		delay_us(1);
		cnt++;
	}
	if(cnt>240) return 1;
	/*2.检测DS18B20是否释放总线*/	
	cnt=0;
	DS18B20_IN();
	while((!DQ_READ) && cnt<240)  //判断DS18B20是否释放总线（60~240 us 响应复位信号之后会释放总线）
	{
		delay_us(1);
		cnt++;
	}
	if(cnt>240)	return 2;
	else return 0;
}
 
/*
功能：检测DS18B20设备是否正常
返回值：
			0  DS18B20设备正常
			1  DS18B20设备响应复位信号失败
			2  DS18B20设备释放总线失败
*/
uint8_t DS18B20_CheckDevice(void)
{
	DS18B20_SendRestSingle();/*1.主机发送复位信号*/
	return DS18B20_CheckReadySingle();/*2.检测存在脉冲*/
}
 
/*功能：向DS18B20写一个字节数据（命令）*/
uint8_t DS18B20_WriteByte(uint8_t cmd)
{
	uint8_t i=0;
	DS18B20_OUT();
	for(i=0;i<8;i++)
	{
		DQ_Low;
		delay_us(2);     //主机拉低总线写数据时间隙2us
		cmd&0x01 ? DQ_High : DQ_Low;  //如果该写入位为1，必须在15us之内把总线拉高，为0 保持60us即可。
		delay_us(60);    //确保DS18B20已经成功读取到该位数据
		DQ_High;  //一位发送完成
		cmd >>=1;
		delay_us(2);     //位间隙2us
	}
	return 0;
}
 
/*功能：从DS18B20读取一个字节数据*/
uint8_t DS18B20_ReadByte(void)
{
	uint8_t i,data=0;
	for(i=0;i<8;i++)
	{
		DS18B20_OUT();//初始化为输出模式
		DQ_Low;  
		delay_us(2);  //主机拉低总线读数据时间隙2us		
		DQ_High; //释放总线，准备读取位数据
		DS18B20_IN(); //初始化为输入模式
		delay_us(10); //等待DS18B20的数据输出
		data >>=1 ;  //高位补0，默认以0为准
		if(DQ_READ) data |=0x80;
		delay_us(60); //延时确保DS18B20采样周期已经过去（非常重要）
		DQ_High;  //释放总线准备读取下一位位数据
	}
	return data;
}
 
/*
函数功能: 读取一次DS18B20的温度数据
返 回 值: 读取的温度数据
考虑的情况:  总线上只是接了一个DS18B20的情况
*/
uint16_t DS18B20_GetTemperature(void)
{
	  uint16_t temp=0;
		uint8_t temp_H,temp_L;
		int intT,decT;
	  DS18B20_CheckDevice();   //发送复位脉冲、检测存在脉冲
		DS18B20_WriteByte(0xCC); //跳过ROM序列检测
		DS18B20_WriteByte(0x44); //启动一次温度转换
		
		//等待温度转换完成
		while(DS18B20_ReadByte()!=0xFF){}
		
		DS18B20_CheckDevice();   //发送复位脉冲、检测存在脉冲
		DS18B20_WriteByte(0xCC); //跳过ROM序列检测
		DS18B20_WriteByte(0xBE); //读取温度
		
		temp_L=DS18B20_ReadByte(); //读取的温度低位数据
		temp_H=DS18B20_ReadByte(); //读取的温度高位数据
		temp=temp_L|(temp_H<<8);   //合成温度
		intT = temp>>4 ;          /*合成实际温度整数部分****精度相对上面的更高*/ 
		decT = temp&0xF ;         /*合成实际温度小数部分*/
		return temp;
}
 
///*功能：从DS18B20读取温度值,多点测温*/
//uint16_t DS18B20_GetTemperature(void)
//{
//	uint16_t temp;
//	uint8_t TL,TH;
//	DS18B20_SendRestSingle();  										/*1.主机发送复位信号*/
//	if(DS18B20_CheckReadySingle()) return 0xFF;   /*2.检测存在脉冲*/
//	DS18B20_MatchROM();														/*3.匹配 ROM 指令 (仅适用于总线上有多个设备)*/
//	DS18B20_WriteByte(0xCC);  									/*3.跳跃 ROM 指令 (仅适用于总线上只有一个设备)*/
//	DS18B20_WriteByte(0x44);  										/*4.启动温度转换*/
//	while(DS18B20_ReadByte()!=0xFF);							//等待温度转换成功
//	DS18B20_SendRestSingle();										  /*5.主机发送复位信号*/
//	if(DS18B20_CheckReadySingle()) return 0xFF;   /*6.检测存在脉冲*/
//	DS18B20_MatchROM();														/*6.匹配 ROM 指令 (仅适用于总线上有多个设备)*/
//	DS18B20_WriteByte(0xCC);                    /*7.跳跃 ROM 指令 (仅适用于总线上只有一个设备)*/
//	DS18B20_WriteByte(0xBE);                      /*8.发送读取温度命令*/
//	
//	TL = DS18B20_ReadByte();                      /*9.读取温度高低位、校验位*/
//	TH = DS18B20_ReadByte();
//	DS18B20_SendRestSingle();                     /*10.主机发送复位信号，示意停止读取剩下的数据*/
//	temp = TH<<8 | TL;                            /*11.合成温度值*/
//	return temp;
//	
// temp = temp*0.0625;				/*直接合成实际温度*/
// value = temp * 100 + (value > 0 ? 0.5 : -0.5);	//将它放大100倍, 使显示时可显示小数点后两位, 并对小数点后第三进行4舍5入
////		
// intT = temp>>4 ;          /*合成实际温度整数部分****精度相对上面的更高*/ 
// decT = temp&0xF ;         /*合成实际温度小数部分*/
///*   crc = x^8 + x^5 + x^4 + 1*/	
//}
 
/*功能：从DS18B20读取ROM信息 (ROM_ID= 28-92-AF-AC-17-13-1-F1)*/
uint8_t DS18B20_ReadRomInfo(void)
{
	uint8_t i=0;
	DS18B20_WriteByte(0x33);  /*4.启动读取ROM*/
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
 
/*功能：匹配 DS18B20  ROM信息*/
uint8_t DS18B20_MatchROM(void)
{
	uint8_t i=0;
	DS18B20_WriteByte(0x55);  /*4.匹配64位 ROM 信息*/
	for(i=0;i<8;i++)
	{
		DS18B20_WriteByte(ROM_ID[i]);
	}
	return 0;
}
 
 