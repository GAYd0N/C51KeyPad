typedef unsigned char uint8_t;
typedef unsigned int uint16_t;
typedef unsigned long uint32_t;

typedef signed char int8_t;
typedef signed int int16_t;
typedef signed long int32_t;

void ScanKey();
void ProcessKey();
void UART_Init();
void DelayMs(uint16_t _ms);
void DisplayNumber(uint8_t num);

typedef struct Key_Var
{
    uint8_t key_level;
    uint8_t key_state;
    uint8_t key_press;
} Key_Var;
