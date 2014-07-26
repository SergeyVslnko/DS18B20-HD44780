/**
  ******************************************************************************
  * @file    Project/main.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    16-January-2012
  * @brief   Delay.c
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
#include "stm8s.h"
#include "delay.h"


/* Private defines -----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Global variables*/
//volatile u32 TimingDelay;
bool TimingDelay;
void Delay_mks(uint16_t nTime)
{
  
//  TimingDelay=TRUE;
  TIM3_TimeBaseInit(TIM3_PRESCALER_16,nTime);
  TIM3_Cmd(ENABLE); 
  while ((TIM3->SR1&TIM3_SR1_UIF) == 0);
  TIM3->SR1&=~TIM3_SR1_UIF;
}

void Delay_ms(uint16_t nTime)
{

 
 // TimingDelay=TRUE;
 // TIM3_PrescalerConfig(TIM3_PRESCALER_16384,TIM3_PSCRELOADMODE_UPDATE);
  TIM3_TimeBaseInit(TIM3_PRESCALER_16384,nTime);
  TIM3_Cmd(ENABLE); 
  while ((TIM3->SR1 & TIM3_SR1_UIF) == 0);
  TIM3->SR1&=~TIM3_SR1_UIF;
}
void Delay_1muss(void)
{
  nop(); nop(); nop(); nop();
  nop(); nop();

}

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
