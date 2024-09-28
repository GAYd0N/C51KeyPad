#include "REG52.H"
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
const uint8_t keyCode[17] = {2,4,5,6, 7,8,9, 5, 4,5,6, 8, 1,2,3 ,6, 0};

// ��������
Key_Var keyVar[17] = { 0 };

void main()
{
    UART_Init();
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
    //��
    P2 = 0x1F;
    P2 &= ~(0x01 << (uint8_t)(key / 4)) & 0x1F;
    //��
    P0 = 0x0F;
    keyVar[key].key_level = (P0 >> (uint8_t)(key % 4)) & 0x01;
    //״̬���ж�
    switch(keyVar[key].key_state)
    {
        case 0:	//������
            if(keyVar[key].key_level == 0)
            {
                keyVar[key].key_state = 1;
            }
            break;
        case 1:	//�����ж�
            if(keyVar[key].key_level == 1)
            {
                keyVar[key].key_state = 0;
            }
            else if(keyVar[key].key_level == 0)
            {
                keyVar[key].key_press = 1;
                keyVar[key].key_state = 2;
            }
            break;
        case 2://���ɿ�
            if(keyVar[key].key_level == 1)
            {
                keyVar[key].key_state = 0;
                DisplayNumber(10);
            }
            break;
    }

    if (++key > ARRAY_ENDINDEX(keyVar))
    {
        key = 0;
    }
}

void ProcessKey()
{
    uint8_t i;
    for (i = 0; i < ARRAY_LENGTH(keyVar); i++)
    {
        if (keyVar[i].key_press == 1)
        {
            keyVar[i].key_press = 0;
            DisplayNumber(keyCode[i]);
        }
    }
}

void UART_Init()
{
    SM0 = 0;
    SM1 = 1;
    SM2 = 0;
    TI = 0;
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