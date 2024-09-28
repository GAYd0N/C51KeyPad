#include "REG52.H"
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

void main()
{
    Uart1_Init();
    DisplayNumber(10);
    DelayMs(100);
    while (1) {
        ScanKey();
        ProcessKey();
    }
}

void ScanKey()
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
                DisplayNumber(10);
            }
            break;
    }

    if (++key > ARRAY_ENDINDEX(keyVar)) {
        key = 0;
    }
}

void ProcessKey()
{
    uint8_t i;
    for (i = 0; i < ARRAY_LENGTH(keyVar); i++) {
        if (keyVar[i].key_press == 1) {
            keyVar[i].key_press = 0;
            DisplayNumber(keyCode[i]);
            Uart1_SendByte(keyCode[i]);
        }
    }
}

void Uart1_Init() // 9600bps@12.000MHz
{
    PCON &= 0x7F; // 波特率不倍速
    SCON = 0x50;  // 8位数据,可变波特率
    // AUXR &= 0xBF;		//定时器时钟12T模式
    // AUXR &= 0xFE;		//串口1选择定时器1为波特率发生器
    TMOD &= 0x0F; // 设置定时器模式
    TMOD |= 0x20; // 设置定时器模式
    TL1 = 0xFD;   // 设置定时初始值
    TH1 = 0xFD;   // 设置定时重载值
    ET1 = 0;      // 禁止定时器中断
    TR1 = 1;      // 定时器1开始计时
}

// 串口发送一个字节数据
void Uart1_SendByte(uint8_t Byte)
{
    SBUF = Byte;
    // 检测是否完成
    while (TI == 1);
    TI = 0; // TI复位
}

void DelayMs(uint16_t ms)
{
    uint8_t i;
    for (; ms > 0; ms--)
        for (i = 500; i > 0; i--);
}

void DisplayNumber(uint8_t num)
{
    if (num < 0)
        num = 0;
    else if (num > ARRAY_ENDINDEX(displayCode))
        num = ARRAY_ENDINDEX(displayCode);
    P3 = displayCode[num];
}