#include "USART.h"

COMx_Define	COM1;
u8	xdata TX1_Buffer[COM_TX1_Lenth];	//发送缓冲

//串口1初始化
u8 USART_Configuration(void)
{
	u8	i;
	u32	j;
	
	COM1.id = 1;					//串口号
	COM1.TX_read    = 0;    		//发送读指针
	COM1.TX_write   = 0;			//发送写指针
	COM1.B_TX_busy  = 0;			//忙标志

	for(i=0; i<COM_TX1_Lenth; i++)	TX1_Buffer[i] = 0;

	PS = PolityLow;	//低优先级中断

	//模式1可变波特率
	SCON = (SCON & 0x3f) | UART_8bit_BRTx;
	
	j = (MAIN_Fosc / 4) / 9600ul;	//按1T计算
	if(j >= 65536UL)	return 2;	//错误
	j = 65536UL - j;

	//使用定时器2
	AUXR &= ~(1<<4);	//Timer stop
	AUXR |= 0x01;		//S1 BRT Use Timer2;
	AUXR &= ~(1<<3);	//Timer2 set As Timer
	AUXR |=  (1<<2);	//Timer2 set as 1T mode
	TH2 = (u8)(j>>8);
	TL2 = (u8)j;
	IE2  &= ~(1<<2);	//禁止中断
	AUXR &= ~(1<<3);	//定时
	AUXR |=  (1<<4);	//Timer run enable

	ES = ENABLE;	//允许中断
	REN = DISABLE;	//禁止接收

	P_SW1 = (P_SW1 & 0x3f) | (UART1_SW_P30_P31 & 0xc0);	//切换IO

	// if(COMx->UART_RXD_TXD_Short == ENABLE)	PCON2 |=  (1<<4);	//内部短路RXD与TXD, 做中继, ENABLE,DISABLE
	// else									PCON2 &= ~(1<<4);

	return	0;

}


/*************** 装载串口发送缓冲 *******************************/

void TX1_write2buff(u8 dat)	//写入发送缓冲，指针+1
{
	TX1_Buffer[COM1.TX_write] = dat;	//装发送缓冲
	if(++COM1.TX_write >= COM_TX1_Lenth)	COM1.TX_write = 0;

	if(COM1.B_TX_busy == 0)		//空闲
	{  
		COM1.B_TX_busy = 1;		//标志忙
		TI = 1;					//触发发送中断
	}
}


/********************* UART1中断函数************************/
void UART1_int (void) interrupt UART1_VECTOR
{
	if(TI)
	{
		TI = 0;
		if(COM1.TX_read != COM1.TX_write)
		{
		 	SBUF = TX1_Buffer[COM1.TX_read];
			if(++COM1.TX_read >= COM_TX1_Lenth)		COM1.TX_read = 0;
		}
		else	COM1.B_TX_busy = 0;
	}
}



