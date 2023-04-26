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
// ADC1ת���ĵ�ѹֵͨ��MDA��ʽ����SRAM
extern __IO uint16_t ADC_ConvertedValue;

// �ֲ����������ڱ���ת�������ĵ�ѹֵ 	 
int ADC_ConvertedValueLocal; 
//��ʾATK-HC05ģ�������״̬
void HC05_Role_Show(void)
{
	
	if(HC05_Get_Role()==1)printf("ROLE:Master \n");	//����
	else printf("ROLE:Slave \n");			 		//�ӻ�
	/*4. ��������������*/
	if(HC05_Set_Cmd("AT+NAME=HEATR"))printf("4 ������������ʧ��!\r\n");
	else printf("4 ������������Ϊ HEATR \r\n");
	/*5. ���������������*/
	if(HC05_Set_Cmd("AT+PSWD=1234"))printf("5 ���������������ʧ��!\r\n"); //���������4λ
	else printf("5 ���������������Ϊ 1234 \r\n");
}
//��ʾATK-HC05ģ�������״̬
void HC05_Sta_Show(void)
{												 
	if(HC05_LED)printf("STA:Connected \r\n");			//���ӳɹ�
	else printf("STA:Disconnect \r\n");	 			//δ����				 
}	

 int main(void)
 {	
	 
	u8 t;
	u8 key;
	 u8 sendmask=0; 
	int sendcnt=1000;
	u8 sendbuf[20];	  
	u8 reclen=0;  	
	delay_init();	    	 //��ʱ������ʼ��	  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	//uart_init(115200);	 	//���ڳ�ʼ��Ϊ9600
	uart_init(9600);	 	//���ڳ�ʼ��Ϊ9600
	 
	ADCx_Init();
  usmart_dev.init(72); 	//��ʼ��USMART		

	delay_ms(1000);			//�ȴ�����ģ���ϵ��ȶ�
 	while(HC05_Init()) 		//��ʼ��ATK-HC05ģ��  
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
			sendmask=!sendmask;				//����/ֹͣ����  	 
			
		}else delay_ms(10);	   
		if(t==2)
		{
			ADC_ConvertedValueLocal =(float) ADC_ConvertedValue; // ��ȡת����ADֵ
			printf("%d\r\n",ADC_ConvertedValueLocal);
			u3_printf(" %d",ADC_ConvertedValueLocal);		//���͵�����ģ��
			HC05_Sta_Show();  	  
			t=0;
			LED0=!LED0; 	     
		}	  
		if(USART3_RX_STA&0X8000)			//���յ�һ��������
		{
		
 			reclen=USART3_RX_STA&0X7FFF;	//�õ����ݳ���
		  	USART3_RX_BUF[reclen]=0;	 	//���������
			if(reclen==9||reclen==8) 		//����DS1���
			{
				if(strcmp((const char*)USART3_RX_BUF,"+LED1 ON")==0)LED1=0;	//��LED1
				if(strcmp((const char*)USART3_RX_BUF,"+LED1 OFF")==0)LED1=1;//�ر�LED1
			}
 			printf("%s\n",USART3_RX_BUF);//��ʾ���յ�������
 			USART3_RX_STA=0;	 
		}	 															     				   
		t++;	
	}
}
