#ifndef __USART_H
#define __USART_H	 

#include	"config.h"

#define	COM_TX1_Lenth	128

#define	USART1	1

#define	UART_8bit_BRTx	(1<<6)	//8λ����,�ɱ䲨����

#define	UART1_SW_P30_P31	0

// stc15w204s�޼�ʱ��1
#define	BRT_Timer2	2

typedef struct
{ 
	u8	id;				//���ں�

	u8	TX_read;		//���Ͷ�ָ��
	u8	TX_write;		//����дָ��
	u8	B_TX_busy;		//æ��־

} COMx_Define; 

extern	COMx_Define	COM1;
extern	u8	xdata TX1_Buffer[COM_TX1_Lenth];	//���ͻ���

u8	USART_Configuration(void);
void TX1_write2buff(u8 dat);	//д�뷢�ͻ��壬ָ��+1

#endif

