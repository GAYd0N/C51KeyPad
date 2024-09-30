#define        KB_NUMLOCK          0x53
#define        KB_PAD_DIV          0x54
#define        KB_PAD_MUL          0x55
#define        KB_PAD_MINUS        0x56  
#define        KB_PAD_PLUS         0x57
#define        KB_PAD_ENTER        0x58
#define        KB_PAD_1            0x59
#define        KB_PAD_2            0x5A
#define        KB_PAD_3            0x5B
#define        KB_PAD_4            0x5C
#define        KB_PAD_5            0x5D
#define        KB_PAD_6            0x5E
#define        KB_PAD_7            0x5F
#define        KB_PAD_8            0x60
#define        KB_PAD_9            0x61
#define        KB_PAD_0            0x62
#define        KB_PAD_DEL          0x63

typedef struct Key_Var
{
    u8 key_level;
    u8 key_state;
    u8 key_press;
} Key_Var;

void ScanKey(void);
void ProcessKey(void);
void GPIO_Init(void);
// void Uart1_Init(void);
// void Uart1_SendByte(uint8_t Byte);
void Uart1_SendKey(void);
void Delay1ms(void);
void DisplayNumber(u8 num);


