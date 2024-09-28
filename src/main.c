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
const uint8_t keyCode[17] = {5, 5, 5, 5, 7, 8, 9, 5, 4, 5, 6, 4, 1, 2, 3, 4, 0};

// ��������
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
    PCON &= 0x7F; // �����ʲ�����
    SCON = 0x50;  // 8λ����,�ɱ䲨����
    // AUXR &= 0xBF;		//��ʱ��ʱ��12Tģʽ
    // AUXR &= 0xFE;		//����1ѡ��ʱ��1Ϊ�����ʷ�����
    TMOD &= 0x0F; // ���ö�ʱ��ģʽ
    TMOD |= 0x20; // ���ö�ʱ��ģʽ
    TL1 = 0xFD;   // ���ö�ʱ��ʼֵ
    TH1 = 0xFD;   // ���ö�ʱ����ֵ
    ET1 = 0;      // ��ֹ��ʱ���ж�
    TR1 = 1;      // ��ʱ��1��ʼ��ʱ
}

// ���ڷ���һ���ֽ�����
void Uart1_SendByte(uint8_t Byte)
{
    SBUF = Byte;
    // ����Ƿ����
    while (TI == 1);
    TI = 0; // TI��λ
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