//#include <STRING.H>
//#include "config.h"
#include "USART.h"

#include "main.h"

#define ARRAY_LENGTH(arr)   sizeof(arr) / sizeof(arr[0])
#define ARRAY_ENDINDEX(arr) sizeof(arr) / sizeof(arr[0]) - 1

// ����7������� 0,1,2,3,4,5,6,7,8,9,H
const uint8_t displayCode[] = {0x40, 0x79, 0x24, 0x30, 0x19, 0x12, 0x03, 0x78, 0x0, 0x10, 0x09};

//  N   /   *   -
//  7   8   9   +
//  4   5   6   Enter
//  1   2   3   Del
//  0
const uint8_t keyCode[17] = {
    KB_NUMLOCK, KB_PAD_DIV, KB_PAD_MUL, KB_PAD_MINUS, 
    KB_PAD_7, KB_PAD_8, KB_PAD_9, KB_PAD_PLUS, 
    KB_PAD_4, KB_PAD_5, KB_PAD_6, KB_PAD_ENTER, 
    KB_PAD_1, KB_PAD_2, KB_PAD_3, KB_PAD_DEL, 
    KB_PAD_0
};

// ��������
Key_Var keyVar[17] = {0};
// ���ܼ���δʹ�ã� ����λ��δʹ�ã� 6����λ
uint8_t keyByte[8]   = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t keyByteIndex = 2;

void main()
{
    EA = 1;
    GPIO_Init();
    Uart1_Init();
    Delay1ms();
    DisplayNumber(10);
    while (1) {
        ScanKey();
        ProcessKey();
        Uart1_SendKey();
    }
}

void ScanKey(void)
{
    static uint8_t key;
    // ��
    P2 = 0x1F;
    P2 &= ~(0x01 << (uint8_t)(key / 4)) & 0x1F;
    // ��
    P0                    = 0x0F;
    keyVar[key].key_level = (P0 >> (uint8_t)(key % 4)) & 0x01;
    // ״̬���ж�
    switch (keyVar[key].key_state) {
        case 0: // ������
            if (keyVar[key].key_level == 0) {
                keyVar[key].key_state = 1;
            }
            break;
        case 1: // �����ж�
            if (keyVar[key].key_level == 1) {
                keyVar[key].key_state = 0;
            } else if (keyVar[key].key_level == 0) {
                keyVar[key].key_press = 1;
                keyVar[key].key_state = 2;
            }
            break;
        case 2: // ���ɿ�
            if (keyVar[key].key_level == 1) {
                keyVar[key].key_state = 0;
            }
            break;
    }

    if (++key > ARRAY_ENDINDEX(keyVar)) {
        key = 0;
    }
}

void ProcessKey(void)
{
    uint8_t i;
    for (i = 0; i < ARRAY_LENGTH(keyVar); i++) {
        if (keyVar[i].key_press == 1) {
            keyVar[i].key_press = 0;
            DisplayNumber(keyCode[i]);
            if (keyByteIndex > ARRAY_ENDINDEX(keyByte)) {
                keyByteIndex = 2;
            }
            keyByte[keyByteIndex] = keyCode[i];
            keyByteIndex++;
        }
    }
}

void GPIO_Init(void)
{
    // ׼˫���ģʽ
    P1M0 = 0x00;
    P1M1 = 0x00;

    P3M0 = 0x00;
    P3M1 = 0x00;
}

void Uart1_Init(void) // 9600bps@11.0592MHz
{
    // PCON &= 0x7F;		//�����ʲ�����
    // SCON = 0x40;		//8λ����,�ɱ䲨����
    // AUXR &= 0xBB;		//��ʱ��2ʱ��12Tģʽ
    // AUXR &= 0xFF;		//����1ѡ��ʱ��2Ϊ�����ʷ�����
    // TMOD &= 0x0F;		//���ö�ʱ��ģʽ
    // TMOD |= 0x10;		//���ö�ʱ��ģʽ
    // T2L = 0xFD;			//���ö�ʱ��ʼֵ
    // T2H = 0xFD;			//���ö�ʱ����ֵ
    // AUXR |= 0x10;		//��ʱ��2��ʼ��ʱ

    // SCON = 0x50;		//8λ����,�ɱ䲨����
    // AUXR |= 0x01;		//����1ѡ��ʱ��2Ϊ�����ʷ�����
    // AUXR &= 0xFB;		//��ʱ��ʱ��12Tģʽ
    // T2L = 0xE8;			//���ö�ʱ��ʼֵ
    // T2H = 0xFF;			//���ö�ʱ��ʼֵ
    // AUXR |= 0x10;		//��ʱ��2��ʼ��ʱ

    COMx_InitDefine COMx_InitStructure;                       // �ṹ����
    COMx_InitStructure.UART_Mode          = UART_8bit_BRTx;   // ģʽ,       UART_ShiftRight,UART_8bit_BRTx,UART_9bit,UART_9bit_BRTx
    COMx_InitStructure.UART_BRT_Use       = BRT_Timer2;       // ʹ�ò�����,   BRT_Timer1, BRT_Timer2 (ע��: ����2�̶�ʹ��BRT_Timer2)
    COMx_InitStructure.UART_BaudRate      = 9600ul;           // ������, һ�� 110 ~ 115200
    COMx_InitStructure.UART_RxEnable      = ENABLE;           // ��������,   ENABLE��DISABLE
    COMx_InitStructure.BaudRateDouble     = DISABLE;          // �����ʼӱ�, ENABLE��DISABLE
    COMx_InitStructure.UART_Interrupt     = ENABLE;           // �ж�����,   ENABLE��DISABLE
    COMx_InitStructure.UART_Polity        = PolityLow;        // �ж����ȼ�, PolityLow,PolityHigh
    COMx_InitStructure.UART_P_SW          = UART1_SW_P30_P31; // �л��˿�,   UART1_SW_P30_P31,UART1_SW_P36_P37,UART1_SW_P16_P17(����ʹ���ڲ�ʱ��)
    COMx_InitStructure.UART_RXD_TXD_Short = DISABLE;          // �ڲ���·RXD��TXD, ���м�, ENABLE,DISABLE
    USART_Configuration(&COMx_InitStructure);         // ��ʼ������1 USART1,USART2
}

// ���ڷ���һ���ֽ�����
// void Uart1_SendByte(uint8_t Byte)
// {
//     SBUF = Byte;
//     // ����Ƿ����
//     while (TI == 0);
//     TI = 0; // TI��λ
// }

void Uart1_SendKey(void)
{
    uint8_t i;
    for (i = 0; i < ARRAY_ENDINDEX(keyByte); i++) {
        TX1_write2buff(keyByte[i]);
    }
    for (i = 0; i < ARRAY_ENDINDEX(keyByte); i++)
        keyByte[i] = 0;
}

void Delay1ms(void) //@11.0592MHz
{
    uint8_t i, j;

    _nop_();
    _nop_();
    _nop_();
    i = 11;
    j = 190;
    do {
        while (--j);
    } while (--i);
}

void DisplayNumber(uint8_t num)
{
    if (num < 0)
        num = 0;
    else if (num > ARRAY_ENDINDEX(displayCode))
        num = ARRAY_ENDINDEX(displayCode);
    P3 = displayCode[num];
}