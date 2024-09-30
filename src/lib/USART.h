#ifndef __USART_H
#define __USART_H	 

#include	"config.h"

#define	COM_TX1_Lenth	128

#define	USART1	1

#define	UART_8bit_BRTx	(1<<6)	//8位数据,可变波特率

#define	UART1_SW_P30_P31	0

// stc15w204s无计时器1
#define	BRT_Timer2	2

typedef struct
{ 
	u8	id;				//串口号

	u8	TX_read;		//发送读指针
	u8	TX_write;		//发送写指针
	u8	B_TX_busy;		//忙标志

} COMx_Define; 

extern	COMx_Define	COM1;
extern	u8	xdata TX1_Buffer[COM_TX1_Lenth];	//发送缓冲

u8	USART_Configuration(void);
void TX1_write2buff(u8 dat);	//写入发送缓冲，指针+1

#endif

