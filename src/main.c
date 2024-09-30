#include "config.h"
#include "USART.h"

#include "main.h"

#define ARRAY_LENGTH(arr)   sizeof(arr) / sizeof(arr[0])
#define ARRAY_ENDINDEX(arr) sizeof(arr) / sizeof(arr[0]) - 1

// 共阳7段数码管 0,1,2,3,4,5,6,7,8,9,H
const u8 displayCode[] = {0x40, 0x79, 0x24, 0x30, 0x19, 0x12, 0x03, 0x78, 0x0, 0x10, 0x09};

//  N   /   *   -
//  7   8   9   +
//  4   5   6   Enter
//  1   2   3   Del
//  0
const u8 keyCode[17] = {
    KB_NUMLOCK, KB_PAD_DIV, KB_PAD_MUL, KB_PAD_MINUS, 
    KB_PAD_7, KB_PAD_8, KB_PAD_9, KB_PAD_PLUS, 
    KB_PAD_4, KB_PAD_5, KB_PAD_6, KB_PAD_ENTER, 
    KB_PAD_1, KB_PAD_2, KB_PAD_3, KB_PAD_DEL, 
    KB_PAD_0
};

// 五行四列
Key_Var keyVar[17] = {0};
// 功能键（未使用） 保留位（未使用） 6个键位
u8 keyByte[8]   = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
u8 keyByteIndex = 2;

void main()
{
    EA = 1;
    GPIO_Init();
    // Uart1_Init();
    USART_Configuration();
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
    static u8 key;
    // 行
    P2 = 0x1F;
    P2 &= ~(0x01 << (u8)(key / 4)) & 0x1F;
    // 列
    P0 = 0x0F;
    keyVar[key].key_level = (P0 >> (u8)(key % 4)) & 0x01;
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
    u8 i;
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
    // 准双向口模式
    P1M0 = 0x00;
    P1M1 = 0x00;

    P3M0 = 0x00;
    P3M1 = 0x00;
}

// void Uart1_Init(void) // 9600bps@11.0592MHz
// {
//     PCON &= 0x7F;		//波特率不倍速
//     SCON = 0x40;		//8位数据,可变波特率
//     AUXR &= 0xBB;		//定时器2时钟12T模式
//     AUXR &= 0xFF;		//串口1选择定时器2为波特率发生器
//     TMOD &= 0x0F;		//设置定时器模式
//     TMOD |= 0x10;		//设置定时器模式
//     T2L = 0xFD;			//设置定时初始值
//     T2H = 0xFD;			//设置定时重载值
//     AUXR |= 0x10;		//定时器2开始计时

//     SCON = 0x50;		//8位数据,可变波特率
//     AUXR |= 0x01;		//串口1选择定时器2为波特率发生器
//     AUXR &= 0xFB;		//定时器时钟12T模式
//     T2L = 0xE8;			//设置定时初始值
//     T2H = 0xFF;			//设置定时初始值
//     AUXR |= 0x10;		//定时器2开始计时
// }

// 串口发送一个字节数据
// void Uart1_SendByte(u8 Byte)
// {
//     SBUF = Byte;
//     // 检测是否完成
//     while (TI == 0);
//     TI = 0; // TI复位
// }

void Uart1_SendKey(void)
{
    u8 i;
    for (i = 0; i < ARRAY_ENDINDEX(keyByte); i++) {
        TX1_write2buff(keyByte[i]);
    }
    // 0、1未使用
    for (i = 2; i < ARRAY_ENDINDEX(keyByte); i++)
        keyByte[i] = 0;
}

void Delay1ms(void) //@11.0592MHz
{
    u8 i, j;

    _nop_();
    _nop_();
    _nop_();
    i = 11;
    j = 190;
    do {
        while (--j);
    } while (--i);
}

void DisplayNumber(u8 num)
{
    if (num < 0)
        num = 0;
    else if (num > ARRAY_ENDINDEX(displayCode))
        num = ARRAY_ENDINDEX(displayCode);
    P3 = displayCode[num];
}