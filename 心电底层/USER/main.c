#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "lcd.h"
#include "usart.h"
#include "hc05.h"
#include "usart3.h"			 	 
#include "string.h"	   
#include "usmart.h"	
#include "bsp_adc.h"
// ADC1转换的电压值通过MDA方式传到SRAM
extern __IO uint16_t ADC_ConvertedValue;

// 局部变量，用于保存转换计算后的电压值 	 
int ADC_ConvertedValueLocal; 
//显示ATK-HC05模块的主从状态
void HC05_Role_Show(void)
{
	
	if(HC05_Get_Role()==1)printf("ROLE:Master \n");	//主机
	else printf("ROLE:Slave \n");			 		//从机
	/*4. 设置蓝牙的名称*/
	if(HC05_Set_Cmd("AT+NAME=HEATR"))printf("4 蓝牙名称设置失败!\r\n");
	else printf("4 蓝牙名称设置为 HEATR \r\n");
	/*5. 设置蓝牙配对密码*/
	if(HC05_Set_Cmd("AT+PSWD=1234"))printf("5 蓝牙配对密码设置失败!\r\n"); //密码必须是4位
	else printf("5 蓝牙配对密码设置为 1234 \r\n");
}
//显示ATK-HC05模块的连接状态
void HC05_Sta_Show(void)
{												 
	if(HC05_LED)printf("STA:Connected \r\n");			//连接成功
	else printf("STA:Disconnect \r\n");	 			//未连接				 
}	

 int main(void)
 {	
	 
	u8 t;
	u8 key;
	 u8 sendmask=0; 
	int sendcnt=1000;
	u8 sendbuf[20];	  
	u8 reclen=0;  	
	delay_init();	    	 //延时函数初始化	  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	//uart_init(115200);	 	//串口初始化为9600
	uart_init(9600);	 	//串口初始化为9600
	 
	ADCx_Init();
  usmart_dev.init(72); 	//初始化USMART		

	delay_ms(1000);			//等待蓝牙模块上电稳定
 	while(HC05_Init()) 		//初始化ATK-HC05模块  
	{
		printf("ATK-HC05 Error!\n"); 
		delay_ms(500);
		printf("Please Check!!!\n"); 
		delay_ms(100);
	}	 										   	   

	HC05_Role_Show();
	delay_ms(100);
	USART3_RX_STA=0;
 	while(1) 
	{		
		key=KEY_Scan(0);
		if(key==KEY0_PRES)
		{
			sendmask=!sendmask;				//发送/停止发送  	 
			
		}else delay_ms(10);	   
		if(t==2)
		{
			ADC_ConvertedValueLocal =(float) ADC_ConvertedValue; // 读取转换的AD值
			printf("%d\r\n",ADC_ConvertedValueLocal);
			u3_printf(" %d",ADC_ConvertedValueLocal);		//发送到蓝牙模块
			HC05_Sta_Show();  	  
			t=0;
			LED0=!LED0; 	     
		}	  
		if(USART3_RX_STA&0X8000)			//接收到一次数据了
		{
		
 			reclen=USART3_RX_STA&0X7FFF;	//得到数据长度
		  	USART3_RX_BUF[reclen]=0;	 	//加入结束符
			if(reclen==9||reclen==8) 		//控制DS1检测
			{
				if(strcmp((const char*)USART3_RX_BUF,"+LED1 ON")==0)LED1=0;	//打开LED1
				if(strcmp((const char*)USART3_RX_BUF,"+LED1 OFF")==0)LED1=1;//关闭LED1
			}
 			printf("%s\n",USART3_RX_BUF);//显示接收到的数据
 			USART3_RX_STA=0;	 
		}	 															     				   
		t++;	
	}
}
