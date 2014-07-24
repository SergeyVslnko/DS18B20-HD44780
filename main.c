/**
  ******************************************************************************
  * @file    Project/main.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    16-January-2012
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * FOR MORE INFORMATION PLEASE READ CAREFULLY THE LICENSE AGREEMENT FILE
  * LOCATED IN THE ROOT DIRECTORY OF THIS FIRMWARE PACKAGE.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */


/* Includes ------------------------------------------------------------------*/

#include "main.h"
#include "HD44780.h"
#include "delay.h"
#include "1wire.h"
#include "stm8s.h"

#define THERM_LOW()         Wire_Port->ODR &= (u8)(~Wire_Pin)
#define THERM_HIGH()        Wire_Port->ODR |= (u8)Wire_Pin


/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
bool TimingDelay;
void main(void)
{
  /* Init CLK divider*/
  CLK->CKDIVR = 0x00; //to be programmed with value for Fmaster = Fcpu = Fhsi
 


  TIM3_PrescalerConfig(TIM3_PRESCALER_16384,TIM3_PSCRELOADMODE_UPDATE);
  TIM3_UpdateRequestConfig(TIM3_UPDATESOURCE_REGULAR);
  TIM3_SelectOnePulseMode(TIM3_OPMODE_SINGLE);
  TIM3_ITConfig(TIM3_IT_UPDATE, ENABLE);
  GPIO_Configuration();

    /* Configures clocks */

  enableInterrupts();
  
  
 

    LCD_PWRON();
    Delay_ms(100);
    LCD_INIT();
    Toggle();
    LCD_CLEAR_DISPLAY();
 
  LCD_printstring("Hello\n");

  TIM3_PrescalerConfig(TIM3_PRESCALER_16,TIM3_PSCRELOADMODE_UPDATE);
  TIM3_UpdateRequestConfig(TIM3_UPDATESOURCE_REGULAR);
  TIM3_SelectOnePulseMode(TIM3_OPMODE_SINGLE);
  TIM3_ITConfig(TIM3_IT_UPDATE, ENABLE);
  GPIO_Init(Wire_Port, Wire_Pin,GPIO_MODE_OUT_OD_HIZ_SLOW); //GPIO_MODE_OUT_OD_HIZ_FAST
 // GPIO_Init(Wire_Port, PullUp_Pin,GPIO_MODE_OUT_PP_LOW_FAST);
 volatile unsigned char t[11];
 therm_init_mode(THERM_MODE_12BIT);
  while(1)
  {
 /* if(therm_reset()==TRUE) 
    {
   LCD_CLEAR_DISPLAY();
   LCD_printstring(get_temperature(t));
    }
    else 
    {
      LCD_CLEAR_DISPLAY();
      LCD_printstring("offline\n");
    }
   Delay_ms(5000);
   LCD_CLEAR_DISPLAY();*/
    read_rom();
  }
   
}




/**
  ******************************************************************************
  * @brief Configures clocks
  * @par Parameters:
  * None
  * @retval void None
  * @par Required preconditions:
  * None
  ******************************************************************************
  */
void CLK_Configuration(void)
{
  /* Fmaster = 16MHz */
  CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
}

/**
  ******************************************************************************
  * @brief Configures GPIOs
  * @par Parameters:
  * None
  * @retval void None
  * @par Required preconditions:
  * None
  ******************************************************************************
  */
void GPIO_Configuration(void)
{
  /* GPIOD reset */
  GPIO_DeInit(GPIOD);
  /* GPIO reset */
  GPIO_DeInit(LCDPort);
  /* GPIO reset */
  GPIO_DeInit(LCDPwrPort);
  /* GPIO reset */
  GPIO_DeInit(LCDControlPort);
  /* Configure PD0 (LED1) as output push-pull low (led switched on) */
  GPIO_Init(GPIOD, GPIO_PIN_0, GPIO_MODE_OUT_PP_LOW_FAST);
  /* Configure LCDPort D0 output push-pull low LCD Bus*/
  GPIO_Init(LCDPort, GPIO_PIN_0, GPIO_MODE_OUT_PP_LOW_FAST);
  /* Configure LCDPort D1  output push-pull low LCD Bus*/
  GPIO_Init(LCDPort, GPIO_PIN_1, GPIO_MODE_OUT_PP_LOW_FAST);
  /* Configure LCDPort D2  output push-pull low LCD Bus*/
  GPIO_Init(LCDPort, GPIO_PIN_2, GPIO_MODE_OUT_PP_LOW_FAST);
  /* Configure LCDPort D3  output push-pull low LCD Bus*/
  GPIO_Init(LCDPort, GPIO_PIN_3, GPIO_MODE_OUT_PP_LOW_FAST);
  /* Configure LCDPort E output push-pull low LCD Enable Pin*/
  GPIO_Init(LCDControlPort, LCD_Enable, GPIO_MODE_OUT_PP_LOW_FAST);
  /* Configure LCDPort RS output push-pull low LCD RS Pin*/
  GPIO_Init(LCDControlPort, LCD_RS, GPIO_MODE_OUT_PP_LOW_FAST);
  /* Configure LCDPwrOnPort VDD output push-pull low LCD Power Supply*/
  GPIO_Init(LCDPwrPort, LCDPwrPin, GPIO_MODE_OUT_PP_LOW_FAST);
  /* Configure PC4 (push button) as input floating */
  GPIO_Init(BUTTON_PORT, BUTTON_PIN, GPIO_MODE_IN_PU_IT);
}

/**
  ******************************************************************************
  * @brief Toggle PD0 (Led LD1)
  * @par Parameters:
  * None
  * @retval void None
  * @par Required preconditions:
  * None
  ******************************************************************************
  */
void Toggle(void)
{
  GPIO_WriteReverse(GPIOD, GPIO_PIN_0);
}




#ifdef USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param file: pointer to the source file name
  * @param line: assert_param error line source number
  * @retval : None
  */
void assert_failed(u8* file, u32 line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {}
}
#endif

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
