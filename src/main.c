#include <STRING.H>
#include "config.h"
#include "GPIO.h"
#include "USART.h"

#include "main.h"

#define ARRAY_LENGTH(arr)   sizeof(arr) / sizeof(arr[0])
#define ARRAY_ENDINDEX(arr) sizeof(arr) / sizeof(arr[0]) - 1

// 共阳7段数码管 0,1,2,3,4,5,6,7,8,9,H
const uint8_t displayCode[] = {0x40, 0x79, 0x24, 0x30, 0x19, 0x12, 0x03, 0x78, 0x0, 0x10, 0x09};

//  N   /   *   -
//  7   8   9   +
//  4   5   6   Enter
//  1   2   3   Del
//  0
const uint8_t keyCode[17] = {5, 5, 5, 5, 7, 8, 9, 5, 4, 5, 6, 4, 1, 2, 3, 4, 0};

// 五行四列
Key_Var keyVar[17] = {0};
// 功能键（未使用） 保留位（未使用） 6个键位
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
    // 行
    P2 = 0x1F;
    P2 &= ~(0x01 << (uint8_t)(key / 4)) & 0x1F;
    // 列
    P0                    = 0x0F;
    keyVar[key].key_level = (P0 >> (uint8_t)(key % 4)) & 0x01;
    // 状态机判断
    switch (keyVar[key].key_state) {
        case 0: // 待按下
            if (keyVar[key].key_level == 0) {
                keyVar[key].key_state = 1;
            }
            break;
        case 1: // 消抖判断
            if (keyVar[key].key_level == 1) {
                keyVar[key].key_state = 0;
            } else if (keyVar[key].key_level == 0) {
                keyVar[key].key_press = 1;
                keyVar[key].key_state = 2;
            }
            break;
        case 2: // 待松开
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
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.Pin  = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4;
    GPIO_InitStructure.Mode = GPIO_PullUp;
    GPIO_Inilize(GPIO_P1, &GPIO_InitStructure);

    GPIO_InitStructure.Pin  = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_Inilize(GPIO_P3, &GPIO_InitStructure);
}

void Uart1_Init(void) // 9600bps@11.0592MHz
{
    // PCON &= 0x7F;		//波特率不倍速
    // SCON = 0x40;		//8位数据,可变波特率
    // AUXR &= 0xBB;		//定时器2时钟12T模式
    // AUXR &= 0xFF;		//串口1选择定时器2为波特率发生器
    // TMOD &= 0x0F;		//设置定时器模式
    // TMOD |= 0x10;		//设置定时器模式
    // T2L = 0xFD;			//设置定时初始值
    // T2H = 0xFD;			//设置定时重载值
    // AUXR |= 0x10;		//定时器2开始计时

    // SCON = 0x50;		//8位数据,可变波特率
    // AUXR |= 0x01;		//串口1选择定时器2为波特率发生器
    // AUXR &= 0xFB;		//定时器时钟12T模式
    // T2L = 0xE8;			//设置定时初始值
    // T2H = 0xFF;			//设置定时初始值
    // AUXR |= 0x10;		//定时器2开始计时

    COMx_InitDefine COMx_InitStructure;                       // 结构定义
    COMx_InitStructure.UART_Mode          = UART_8bit_BRTx;   // 模式,       UART_ShiftRight,UART_8bit_BRTx,UART_9bit,UART_9bit_BRTx
    COMx_InitStructure.UART_BRT_Use       = BRT_Timer2;       // 使用波特率,   BRT_Timer1, BRT_Timer2 (注意: 串口2固定使用BRT_Timer2)
    COMx_InitStructure.UART_BaudRate      = 9600ul;           // 波特率, 一般 110 ~ 115200
    COMx_InitStructure.UART_RxEnable      = ENABLE;           // 接收允许,   ENABLE或DISABLE
    COMx_InitStructure.BaudRateDouble     = DISABLE;          // 波特率加倍, ENABLE或DISABLE
    COMx_InitStructure.UART_Interrupt     = ENABLE;           // 中断允许,   ENABLE或DISABLE
    COMx_InitStructure.UART_Polity        = PolityLow;        // 中断优先级, PolityLow,PolityHigh
    COMx_InitStructure.UART_P_SW          = UART1_SW_P30_P31; // 切换端口,   UART1_SW_P30_P31,UART1_SW_P36_P37,UART1_SW_P16_P17(必须使用内部时钟)
    COMx_InitStructure.UART_RXD_TXD_Short = DISABLE;          // 内部短路RXD与TXD, 做中继, ENABLE,DISABLE
    USART_Configuration(USART1, &COMx_InitStructure);         // 初始化串口1 USART1,USART2
}

// 串口发送一个字节数据
// void Uart1_SendByte(uint8_t Byte)
// {
//     SBUF = Byte;
//     // 检测是否完成
//     while (TI == 0);
//     TI = 0; // TI复位
// }

void Uart1_SendKey(void)
{
    uint8_t i;
    for (i = 0; i < ARRAY_ENDINDEX(keyByte); i++) {
        TX1_write2buff(keyByte[i]);
    }
    memset(keyByte, 0, sizeof(keyByte));
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