#include "1wire.h"
#include "HD44780.h"
#include "stm8s.h"
#define THERM_INPUT_MODE()  Wire_Port->DDR &= ~Wire_Pin
#define THERM_OUTPUT_MODE() Wire_Port->DDR |=  Wire_Pin
#define THERM_LOW()         Wire_Port->ODR &= (u8)(~Wire_Pin)
#define THERM_HIGH()        Wire_Port->ODR |= (u8)Wire_Pin
#define THERM_READ()        (Wire_Port->IDR & (u8)Wire_Pin)

#define THERM_CMD_CONVERTTEMP 0x44
#define THERM_CMD_RSCRATCHPAD 0xBE
#define THERM_CMD_SKIPROM 0xCC

#define THERM_CMD_WSCRATCHPAD 0x4E
#define THERM_CMD_CPYSCRATCHPAD 0x48
#define THERM_CMD_RECEEPROM 0xB8
#define THERM_CMD_RPWRSUPPLY 0xB4
#define THERM_CMD_SEARCHROM 0xF0
#define THERM_CMD_READROM 0x33
#define THERM_CMD_MATCHROM 0x55
#define THERM_CMD_ALARMSEARCH 0xEC


volatile char iCurrentTemp = 0;
u8 pos=0; //глобальный указатель для itoa();

bool therm_reset(void)
{
  unsigned char i = 0x08;
 
  THERM_OUTPUT_MODE();
 // THERM_HIGH();
  //Delay_mks(10);
  THERM_LOW(); 
  Delay_mks(480);
  THERM_INPUT_MODE();
  Delay_mks(50);  //тут мы нашли нужную задержку
  i = THERM_READ();
  return (i==0x00)? TRUE:FALSE;
}
void therm_write_bit(bool bBit)
{
  THERM_OUTPUT_MODE();
  THERM_LOW();
 
   Delay_mks(1);
 
  if (bBit) 
  {
    THERM_INPUT_MODE();
  }
   
   Delay_mks(100);  //60 to 100
   
  THERM_INPUT_MODE();
}
 
/**
  * @brief Чтение бита
  * @par
  * Параметры отсутствуют
  * @retval
  * Значение бита.
*/
bool therm_read_bit(void)
{
  bool bBit = FALSE;
 
  THERM_OUTPUT_MODE();
  THERM_LOW(); 
 
   Delay_mks(1);
 
  THERM_INPUT_MODE();
 
   Delay_mks(100);  // тут я заменил 15 на 100
   
  if (THERM_READ()) 
  {
    bBit = TRUE;
  }
 
   Delay_mks(100);
 
  return bBit;
}
 
/**
  * @brief Чтение байта
  * @par
  * Параметры отсутствуют
  * @retval
  * Значение байта.
*/
static unsigned char therm_read_byte(void)
{
  unsigned char i = 8;
  unsigned char n = 0;
  while (i--)
  {    
    // Сдвинуть на одну позицию вправо и сохранить значение бита
    n >>= 1;
    n |= (therm_read_bit() << 7);
  }
  return n;
}
 
/**
  * @brief Запись байта
  * @param[in] byte байт
  * @retval
  * Возвращаемое значение отсутствует
*/
void therm_write_byte(unsigned char byte)
{
  unsigned char i = 8;
 
  while (i--)
  {
    // Записать текущий бит и сдвинуть на 1 позицию вправо
    // для доступа к следующему биту
    therm_write_bit(byte & 1);
    byte >>= 1;
  }  
}


/**
  * @brief Установить режим работы термодатчика
  * @param[in] mode Режим работы
  * @retval
  * Возвращаемое значение отсутствует
*/
void therm_init_mode(THERM_MODE mode)
{
  therm_reset();
  therm_write_byte(THERM_CMD_SKIPROM);
  therm_write_byte(THERM_CMD_WSCRATCHPAD);
  therm_write_byte(0);
  therm_write_byte(0);
  therm_write_byte(mode);
}

/**
  * @brief Чтение температуры.
  * @par
  * Параметры отсутствуют
  * @retval
  * Температура.
*/
char GetTemperature(void)
{
  char iResult = 0;
  volatile  unsigned char temperature[2] = {0, 0};
  unsigned char digit;
  short iReadLimit;
 
  // Запрет прерываний во время опроса датчика
  disableInterrupts();  
   
  // Сброс и сразу переход к преобразованию температуры
  iResult = therm_reset();
  therm_write_byte(THERM_CMD_SKIPROM);
  therm_write_byte(THERM_CMD_CONVERTTEMP);
   
  // Ожидание завершения преобразования
 // iReadLimit = 10;
 //while (!therm_read_bit() && (--iReadLimit > 0)) { ; }
  Delay_mks(1000); //для  12 битного ацп
  // Сброс и чтение байт температуры
  therm_reset();
  therm_write_byte(THERM_CMD_SKIPROM);
  therm_write_byte(THERM_CMD_RSCRATCHPAD);  
  temperature[0] = therm_read_byte();
  temperature[1] = therm_read_byte();
 
  // Разрешить прерывания
  enableInterrupts();  
 /*  int i=0;
   int j=0;
for(j;j<2;j++)
{
for(i;i<8;i++)
{
  if(temperature[i+j*8])
LCD_printchar('1');
else
LCD_printchar('0');
}
Delay_ms(5000);
LCD_CLEAR_DISPLAY();
i=0;
}*/
  digit = 0;
 
  digit  = (  temperature[0]         >> 4 ) & 0x0F;  
  digit |= ( (temperature[1] & 0x0F) << 4 ) & 0xF0;  
 
  // Отрицательная температура - инвертировать и прибавить 1
  if (temperature[1] & 0x80)
  {
    iResult = ~digit; 
    iResult++;
  } else
  {
    iResult = digit; 
  }
  
  iCurrentTemp = iResult;



  return iResult;
}

volatile unsigned char* get_temperature( volatile unsigned char* t)
{
  pos=0;
  char iResult_int = 0;
  char iResult_float=0;
  volatile  unsigned char temperature[9];
  short iReadLimit;
 
  
  u8 mode;

  u8 mask;
  u16 tconv=750;
  disableInterrupts();  
   
  therm_reset();
  therm_write_byte(THERM_CMD_SKIPROM);
  therm_write_byte(THERM_CMD_RECEEPROM);
 // Delay_mks(tconv);
  while(!therm_read_bit()){};
  int i=0;
  therm_reset();
  therm_write_byte(THERM_CMD_SKIPROM);
  therm_write_byte(THERM_CMD_RSCRATCHPAD);
  for(i=0;i<9;i++)
    temperature[i]=therm_read_byte();
  mode=temperature[4]& 0x03;

  
  therm_reset();
  therm_write_byte(THERM_CMD_SKIPROM);
  therm_write_byte(THERM_CMD_CONVERTTEMP);
  iReadLimit = 10;
  while (!therm_read_bit() && (--iReadLimit > 0)) { ; }
 // while (!therm_read_bit());
 // Delay_mks((tconv/ (1<<(3-mode))));
  
  therm_reset();
  therm_write_byte(THERM_CMD_SKIPROM);
  therm_write_byte(THERM_CMD_RSCRATCHPAD);
   for(i=0;i<9;i++)
    temperature[i]=therm_read_byte();
  
     enableInterrupts();  
     temperature[0]&= (0xff<<3-mode); //зануляем ненужные биты в зависимости от режима работы
  
  iResult_int  = (temperature[0]         >> 4 ) & 0x0F;  
  iResult_int |= ((temperature[1] & 0x0F) << 4 ) & 0xF0;  
  iResult_float = temperature[0] & 0x0f;
 
  // Отрицательная температура - инвертировать и прибавить 1
  if (temperature[1] & 0x80)
  {
    iResult_int = ~iResult_int; 
    iResult_int++;
    iResult_float= ~iResult_float;
    iResult_float++;
  } 
  int temp=0;
  if(temperature[1] & 0x80)
    t[pos++]='-' ;
  else
    t[pos++]='+';
  i=0;
  for(i;i<7;i++)     //получаем целую часть
  {
   if(iResult_int & 1<<i)
    temp+=1<<(i);
  } 
  
  itoa(temp,t);
  t[pos++]='.';
  i=3;
  temp=0;
  for(i;i>=0;i--)
  {
    if(iResult_float & 1<<i) //получаем дробную часть
    temp+=500/(1<<(3-i));
  }
     itoa(temp,t);
     t[pos++]=0xdf;
     t[pos++]='C';
     t[pos]='\n';
return t;
}


//Преобразует целое в строку

void itoa(int temp,volatile unsigned char* t)  
{
   if(temp/100)
  {   switch(temp/100)
   {
                 case 1: t[pos++]='1'; break;
	         case 2: t[pos++]='2'; break;
	    	 case 3: t[pos++]='3'; break;
	    	 case 4: t[pos++]='4'; break;
	    	 case 5: t[pos++]='5'; break;
	    	 case 6: t[pos++]='6'; break;
	    	 case 7: t[pos++]='7'; break;
	    	 case 8: t[pos++]='8'; break;
	    	 case 9: t[pos++]='9'; break;
    }
     temp%=100;
  }
  if(temp/10)
  {
  switch(temp/10)
   {
                 case 1: t[pos++]='1'; break;
	         case 2: t[pos++]='2'; break;
	    	 case 3: t[pos++]='3'; break;
	    	 case 4: t[pos++]='4'; break;
	    	 case 5: t[pos++]='5'; break;
	    	 case 6: t[pos++]='6'; break;
	    	 case 7: t[pos++]='7'; break;
	    	 case 8: t[pos++]='8'; break;
	    	 case 9: t[pos++]='9'; break;
    }
   temp%=10;
  }
 
  switch(temp)
   {
                 case 1: t[pos++]='1'; break;
	         case 2: t[pos++]='2'; break;
	    	 case 3: t[pos++]='3'; break;
	    	 case 4: t[pos++]='4'; break;
	    	 case 5: t[pos++]='5'; break;
	    	 case 6: t[pos++]='6'; break;
	    	 case 7: t[pos++]='7'; break;
	    	 case 8: t[pos++]='8'; break;
	    	 case 9: t[pos++]='9'; break;
    }
  
}


void read_rom(){
volatile unsigned char info[8];
int i=0;
int j=0;
disableInterrupts();
therm_reset(); 
therm_write_byte(THERM_CMD_READROM);
for(i;i<8;i++)
{
  info[i]=therm_read_byte();
}
enableInterrupts(); 

i=0;
LCD_CLEAR_DISPLAY();

for(j;j<8;j++)
{
for(i;i<8;i++)
{
if((info[j])& 1<<(7-i))
LCD_printchar('1');
else
LCD_printchar('0');
}
Delay_ms(3000);
LCD_CLEAR_DISPLAY();
i=0;
}
}

/*bool calc_CRC(unsigned char* e)
{
  u16 crc=0x131;
  u16 ret;
  u16 t;
  u8 i=9;
  t=*e>>52;
  while(i<52)
  {
  ret=t^crc;
  while(!(ret>>8))
  {
    ret<<1;
    t=*e<<++i;
    ret|= t&0x01;
    t=ret;
  }
    }
  return (ret==(*e&0xff))? TRUE : FALSE;
}*/