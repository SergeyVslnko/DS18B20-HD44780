#include "delay.h"


typedef enum
{
  THERM_MODE_9BIT  = 0x1F,
  THERM_MODE_10BIT = 0x3F,
  THERM_MODE_11BIT = 0x5F,
  THERM_MODE_12BIT = 0x7F
} THERM_MODE;





#define Wire_Port GPIOA
#define Wire_Pin GPIO_PIN_6
BitStatus wire_init(void);
bool therm_reset(void);
void therm_init_mode(THERM_MODE mode);
void therm_write_byte(unsigned char byte);
unsigned char therm_read_byte(void);
bool therm_read_bit(void);
void therm_write_bit(bool bBit);
char GetTemperature(void);
void read_rom();
volatile unsigned char* get_temperature( volatile unsigned char*);
void itoa(int numb,volatile unsigned char*);
//bool calc_CRC(unsigned char* e);