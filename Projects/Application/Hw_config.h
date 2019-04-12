/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __HW_CONFIG_H
#define __HW_CONFIG_H

/* Platform config -----------------------------------------------------------*/

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx.h"
#include <string.h>
// #include "i2c_device.h"

/* Define config -------------------------------------------------------------*/
#define TRUE			1
#define FALSE           0
#define ON				1
#define OFF				0
#define IN				1
#define OUT				0
typedef unsigned char   BOOL;

#define DEBUG

#if defined (DEBUG)
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

/* Macro ---------------------------------------------------------------------*/
/*
#define UartTxPutChar(x)		\
{	\
     UART1_SendData8(x);	\
     while (UART1_GetFlagStatus(UART1_FLAG_TXE) == RESET);	\
}*/

/* Exported types ------------------------------------------------------------*/
void Allocate_Test1(void);

void Allocate_Test2(void) __attribute__((section(".ARM.__at_0x8002000")));

void LED_Config(void);
void LED_Test(void);

void WKUP_Set(void);
void WKUP_Config(void);

void GPIO_Config_Input(void);
void GPIO_Config_Output(void);
BOOL GPIO_Input_GetState(GPIO_TypeDef* GPIOx,uint16_t PIN);
void GPIO_Output_SetState(GPIO_TypeDef* GPIOx,uint16_t PIN,BOOL Enable);
void GPIO_EXTI_Config(void);

void TIM1_PWM_Config(void);
void TIM2_PWM_Config(void);
void TIM3_PWM_Config(void);
void TIM15_PWM_Config(void);
void TIM16_PWM_Config(uint32_t Freq);

void TIM6_Config(void);

void ADC_ConversionOutput(uint8_t Index);
void ADC_DMA_Config(void);

void USART1_Test(void);
void USART1_Config(void);     

void SysTickTimer_Config(void);
void UART_SendByte(uint8_t Data);
void UART_SendString(uint8_t* Data,uint16_t len);
void SystemClkDelay(uint32_t u32Delay);
void Delay(__IO uint32_t uTime);
void TimingDelay_Decrement(void);
/* Exported constants --------------------------------------------------------*/

#endif  /* __HW_CONFIG_H */

