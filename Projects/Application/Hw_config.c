/* Includes ------------------------------------------------------------------*/
#include "Hw_config.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
#if defined (DEBUG)
#ifdef __GNUC__
  /* With GCC/RAISONANCE, small PRINTF (option LD Linker->Libraries->Small PRINTF
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART */
  USART_SendData(USART1, (uint8_t) ch);

  /* Loop until transmit data register is empty */
  while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
  {}

  return ch;
}
#endif

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static __IO uint32_t uwTimingDelay;
RCC_ClocksTypeDef 	RCC_ClockFreq;

/*ADC variable*/
#define ADC_SampleNo 					100
#define ADC1_VERF_MV					3300	//3300
#define ADC_ChannelNo					14

static uint16_t							ADCOld=0;
static uint16_t							ADValue=0;
static uint16_t							ADCBuffer[ADC_SampleNo];	
static uint8_t							ADCCounter=0;
static __IO uint16_t 						RegularConvData_Tab[ADC_ChannelNo];

/*TIMER variable*/
uint32_t 	TimCounUpper = 0;
uint32_t 	TimCounLower = 5000;

/*assign variable*/

//Three variable allocate description method in KeilC as below
// use STLINKV2 to monitor below address data 
/*
	0x8001006 : 0x600D
	0x8001016 : 0x601E
	0x8001026 : 0x602F
*/

#define AssignedVar1Addr	0x8001006
#define AssignedVar1Value	0x600D
const uint16_t AssignedVar1 __attribute__((at(AssignedVar1Addr))) = AssignedVar1Value ;

const uint16_t AssignedVar2 __attribute__((at(0x8001016))) = 0x601E;
const uint16_t AssignedVar3 __attribute__((section(".ARM.__at_0x8001026"))) = 0x602F;


#define ForceBreak(void)		\
{	\
	PRINTF("\r\n");	\
	if ('x' == USART_ReceiveData(USART1))	\
		break;	\
	if ('X' == USART_ReceiveData(USART1))	\
		break;	\
}	\

/* Private functions ---------------------------------------------------------*/
//Two function allocate description method in KeilC as below
	
//monitor MAP file to check function address
void Allocate_Test2(void)
{
	uint16_t Temp = 0 ;

	Temp = AssignedVar1 ;

	PRINTF("Allocate_Test22222 function \r\n");
	PRINTF("Temp = 0x%8X\r\n" , Temp);

	#if 1
	PRINTF("AssignedVar1 = 0x%8X, 0x%8X \r\n" , AssignedVar1,&AssignedVar1);
	PRINTF("AssignedVar2 = 0x%8X, 0x%8X\r\n" , AssignedVar2,&AssignedVar2);
	PRINTF("AssignedVar3 = 0x%8X, 0x%8X \r\n" , AssignedVar3,&AssignedVar3);

	PRINTF("Test2 address =0x%8X \r\n" ,&Allocate_Test2);
	
	#endif
}

/*
	use a scatter file to place the named sections at specific locations.
	= FILE location : MDK-ARM\STM32F051\STM32F051.sct
	=> modify method TBD
*/
//monitor MAP file to check function address	
#pragma arm section code="FuncLocation1"
void Allocate_Test1(void)
{
	uint16_t Temp = 0 ;

	Temp = AssignedVar1 ;

	PRINTF("Allocate_Test1111111 function \r\n");	
	PRINTF("Temp = 0x%8X\r\n" , Temp);

	#if 1
	PRINTF("AssignedVar1 = 0x%8X, 0x%8X \r\n" , AssignedVar1,&AssignedVar1);
	PRINTF("AssignedVar2 = 0x%8X, 0x%8X\r\n" , AssignedVar2,&AssignedVar2);
	PRINTF("AssignedVar3 = 0x%8X, 0x%8X \r\n" , AssignedVar3,&AssignedVar3);

	PRINTF("Test1 address =0x%8X \r\n" ,&Allocate_Test1);	
	#endif
}
#pragma arm section
	
void LED_Config(void)	//for test purpose
{
  	GPIO_InitTypeDef    GPIO_InitStructure;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);		

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;	
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}

void LED_Test(void)	//for test purpose
{
	GPIOC->ODR ^= GPIO_Pin_8;
	Delay(100);
	
//	GPIO_WriteBit(GPIOC, GPIO_Pin_8, Bit_SET);
//	Delay(100);

//	GPIO_WriteBit(GPIOC, GPIO_Pin_8, Bit_RESET);
//	Delay(100);	
}
	
void WKUP_Set(void)	//PB15
{
	PRINTF("ENTER STANDBY mode \r\n");	

#if defined (WKUP_ON_DISCOVERY)	//use PA0 to test standby mode , WKUP1

//	while(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)!= RESET);
//	while(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0) == RESET)
//	{
//	}

	/* Disable wake-up source(Wake up pin) to guarantee free access to WUT level-OR input */ 
	PWR_WakeUpPinCmd(PWR_WakeUpPin_1, DISABLE);  

	/* Clear Standby flag */
	PWR_ClearFlag(PWR_FLAG_SB);

	/* Enable WKUP pin 1 */
	PWR_WakeUpPinCmd(PWR_WakeUpPin_1,ENABLE);

	/* Clear Power Wake-up (CWUF) flag */
	PWR_ClearFlag(PWR_FLAG_WU);	

	/* Request to enter STANDBY mode (Wake Up flag is cleared in PWR_EnterSTANDBYMode function) */
	PWR_EnterSTANDBYMode();		

		
#else	//PB15

//	while(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_15)!= RESET);
//	while(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_15) == RESET)
//	{
//	}	

	/* Disable wake-up source(Wake up pin) to guarantee free access to WUT level-OR input */ 
	PWR_WakeUpPinCmd(PWR_WakeUpPin_7, DISABLE);  

	/* Clear Standby flag */
	PWR_ClearFlag(PWR_FLAG_SB);

	/* Enable WKUP pin 7 */
	PWR_WakeUpPinCmd(PWR_WakeUpPin_7,ENABLE);

	/* Clear Power Wake-up (CWUF) flag */
	PWR_ClearFlag(PWR_FLAG_WU);	

	/* Request to enter STANDBY mode (Wake Up flag is cleared in PWR_EnterSTANDBYMode function) */
	PWR_EnterSTANDBYMode();		

#endif	
}

	
void WKUP_Config(void) //PB15
{
  	GPIO_InitTypeDef    GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;	
	GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* Connect Button EXTI Line to Button GPIO Pin */
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);

    /* Configure Button EXTI line */
    EXTI_InitStructure.EXTI_Line = EXTI_Line0;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    /* Enable and set Button EXTI Interrupt to the lowest priority */
    NVIC_InitStructure.NVIC_IRQChannel = EXTI0_1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPriority = 0x03;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

    NVIC_Init(&NVIC_InitStructure); 

	/* Enable PWR APB1 Clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

	/* Allow access to Backup */
	PWR_BackupAccessCmd(ENABLE);

	/* Reset RTC Domain */
	RCC_BackupResetCmd(ENABLE);
	RCC_BackupResetCmd(DISABLE);

}

void GPIO_Config_Input(void)
{
  	GPIO_InitTypeDef    GPIO_InitStructure;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOE, ENABLE);	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;	
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_Init(GPIOB, &GPIO_InitStructure);	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10;
	GPIO_Init(GPIOE, &GPIO_InitStructure);		
}

void GPIO_Config_Output(void)
{
  	GPIO_InitTypeDef    GPIO_InitStructure;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);		
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOD, ENABLE);	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOE, ENABLE);		
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOF, ENABLE);	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;	
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_Init(GPIOC, &GPIO_InitStructure);	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|	\
								GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
	GPIO_Init(GPIOD, &GPIO_InitStructure);	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|	\
								GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13;
	GPIO_Init(GPIOE, &GPIO_InitStructure);		

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_10;
	GPIO_Init(GPIOF, &GPIO_InitStructure);		
}	

BOOL GPIO_Input_GetState(GPIO_TypeDef* GPIOx,uint16_t PIN)
{
	return GPIO_ReadInputDataBit(GPIOx,PIN);
}

void GPIO_Output_SetState(GPIO_TypeDef* GPIOx,uint16_t PIN,BOOL Enable)
{
	GPIO_WriteBit(GPIOx, PIN, (BitAction)Enable);
}

void GPIO_EXTI_Config(void)	//PF6
{
	GPIO_InitTypeDef    GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	/* Enable GPIOC clock */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOF, ENABLE);

	/* Enable SYSCFG clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	/* Configure PC8 and PC13 pins as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOF, &GPIO_InitStructure);

	/* Connect EXTI8 Line to PC8 pin */
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOF, EXTI_PinSource6);

	/* Configure EXTI6 line */
	EXTI_InitStructure.EXTI_Line = EXTI_Line6;  
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	/* Enable and set EXTI4_15 Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_15_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 0x03;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

}

void TIM1_PWM_Config(void)	//PE14:100Hz
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	uint16_t TimerPeriod = 0;		
	uint16_t Channel4Pulse = 0;

	/* Enable GPIO clock */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOE, ENABLE);

	GPIO_StructInit(&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	GPIO_PinAFConfig(GPIOE, GPIO_PinSource14, GPIO_AF_0);

	/* TIM1 clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1 , ENABLE);

	/*************************************************************************
		TIMx_ARR = TIM_Period , Auto-Reload Register
		TIMx_CCR1 = TIM_Pulse , Capture Compare Register

		** Prescaler = (TIMxCLK / TIMx counter clock) - 1
		
		** Timer clock = PWM frequency * 2^PWM resolution

		The objective is to generate PWM signal at xx KHz:
		** TIMx Frequency = TIMx counter clock/(TIM_Period + 1)
			-> 1/ TIMx Frequency = (TIM_Period + 1) * (TIM_Prescaler + 1) / TIMxCLK
			-> TIMx Frequency =  TIMxCLK / ((TIM_Period + 1) * (TIM_Prescaler + 1))
				=> example : 
					Freq = 100 Hz = TIMxCLK / ((TIM_Period + 1) * (TIM_Prescaler + 1))
								  = 48000000 / (4000-1     + 1) * (120 -1        + 1))
			
		** TIMx_Period = (SystemCoreClock / TIMx Frequency) - 1 , if prescaler = 1-1

		** duty cycle = (TIM_Pulse/ TIM_Period + 1)* 100
		** ChannelxPulse = DutyCycle * (TIM1_Period - 1) / 100	
	*************************************************************************/

	TimerPeriod = 480 - 1;
	/* Compute CCR1 value to generate a duty cycle at 50% for channel 1 */
	Channel4Pulse = (uint16_t) (((uint32_t) 50 * (TimerPeriod - 1)) / 100);

	/* Time Base configuration */
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	TIM_TimeBaseStructure.TIM_Prescaler = 1000 -1 ;
	TIM_TimeBaseStructure.TIM_Period = TimerPeriod;	
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

	/* Channel 1, 2,3 and 4 Configuration in PWM mode */
	TIM_OCStructInit(&TIM_OCInitStructure);
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_Pulse = Channel4Pulse;
	TIM_OC4Init(TIM1, &TIM_OCInitStructure);

	/* TIM1 counter enable */
	TIM_Cmd(TIM1, ENABLE);

	/* TIM1 Main Output Enable */
	TIM_CtrlPWMOutputs(TIM1, ENABLE);
	
}

void TIM2_PWM_Config(void)	//PB10:100Hz
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	uint16_t TimerPeriod = 0;		
	uint16_t Channel3Pulse = 0;

	/* Enable GPIO clock */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);

	GPIO_StructInit(&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_2);

	/* TIM2 clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , ENABLE);

	/*************************************************************************
		TIMx_ARR = TIM_Period , Auto-Reload Register
		TIMx_CCR1 = TIM_Pulse , Capture Compare Register

		** Prescaler = (TIMxCLK / TIMx counter clock) - 1
		
		** Timer clock = PWM frequency * 2^PWM resolution

		The objective is to generate PWM signal at xx KHz:
		** TIMx Frequency = TIMx counter clock/(TIM_Period + 1)
			-> 1/ TIMx Frequency = (TIM_Period + 1) * (TIM_Prescaler + 1) / TIMxCLK
			-> TIMx Frequency =  TIMxCLK / ((TIM_Period + 1) * (TIM_Prescaler + 1))
				=> example : 
					Freq = 100 Hz = TIMxCLK / ((TIM_Period + 1) * (TIM_Prescaler + 1))
								  = 48000000 / (4000-1     + 1) * (120 -1        + 1))
			
		** TIMx_Period = (SystemCoreClock / TIMx Frequency) - 1 , if prescaler = 1-1

		** duty cycle = (TIM_Pulse/ TIM_Period + 1)* 100
		** ChannelxPulse = DutyCycle * (TIM1_Period - 1) / 100	
	*************************************************************************/

	TimerPeriod = 480 - 1;
	/* Compute CCR1 value to generate a duty cycle at 50% for channel 1 */
	Channel3Pulse = (uint16_t) (((uint32_t) 50 * (TimerPeriod - 1)) / 100);

	/* Time Base configuration */
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	TIM_TimeBaseStructure.TIM_Prescaler = 1000 -1 ;
	TIM_TimeBaseStructure.TIM_Period = TimerPeriod;	
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

	/* Channel 1, 2,3 and 4 Configuration in PWM mode */
	TIM_OCStructInit(&TIM_OCInitStructure);
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_Pulse = Channel3Pulse;
	TIM_OC3Init(TIM2, &TIM_OCInitStructure);

	/* TIM2 counter enable */
	TIM_Cmd(TIM2, ENABLE);

	/* TIM2 Main Output Enable */
	TIM_CtrlPWMOutputs(TIM2, ENABLE);
	
}

/*
	50% = 500/1000
	85% = 850/1000
*/
//uint16_t TIM3_DutyConvert(uint16_t Duty)
//{
//	return (uint16_t) (((uint32_t) Duty * (TimerPeriod - 1)) / 1000);
//}

void TIM3_PWM_Config(void)	//PB0:100Hz , PB1:100Hz
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	uint16_t TimerPeriod = 0;
	uint16_t Channel3Pulse = 0;
	uint16_t Channel4Pulse = 0;

	/* Enable GPIO clock */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);

	GPIO_StructInit(&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_PinAFConfig(GPIOB, GPIO_PinSource0, GPIO_AF_1);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource1, GPIO_AF_1);

	/* TIM3 clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3 , ENABLE);

	/*************************************************************************
		TIMx_ARR = TIM_Period , Auto-Reload Register
		TIMx_CCR1 = TIM_Pulse , Capture Compare Register

		** Prescaler = (TIMxCLK / TIMx counter clock) - 1
		
		** Timer clock = PWM frequency * 2^PWM resolution

		The objective is to generate PWM signal at xx KHz:
		** TIMx Frequency = TIMx counter clock/(TIM_Period + 1)
			-> 1/ TIMx Frequency = (TIM_Period + 1) * (TIM_Prescaler + 1) / TIMxCLK
			-> TIMx Frequency =  TIMxCLK / ((TIM_Period + 1) * (TIM_Prescaler + 1))
				=> example : 
					Freq = 100 Hz = TIMxCLK / ((TIM_Period + 1) * (TIM_Prescaler + 1))
								  = 48000000 / (4000-1     + 1) * (120 -1        + 1))
			
		** TIMx_Period = (SystemCoreClock / TIMx Frequency) - 1 , if prescaler = 1-1

		** duty cycle = (TIM_Pulse/ TIM_Period + 1)* 100
		** ChannelxPulse = DutyCycle * (TIM1_Period - 1) / 100	
	*************************************************************************/

	TimerPeriod = 480 - 1;
	/* Compute CCR1 value to generate a duty cycle at 50% for channel 1 */
	Channel3Pulse = (uint16_t) (((uint32_t) 50 * (TimerPeriod - 1)) / 100);
	/* Compute CCR2 value to generate a duty cycle at 80% for channel 2 */
	Channel4Pulse = (uint16_t) (((uint32_t) 80 * (TimerPeriod - 1)) / 100);
	
	/* Time Base configuration */
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	TIM_TimeBaseStructure.TIM_Prescaler = 1000 -1 ;
	TIM_TimeBaseStructure.TIM_Period = TimerPeriod;	
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

	/* Channel 1, 2,3 and 4 Configuration in PWM mode */
	TIM_OCStructInit(&TIM_OCInitStructure);
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_Pulse = Channel3Pulse;
	TIM_OC3Init(TIM3, &TIM_OCInitStructure);
	
	TIM_OCInitStructure.TIM_Pulse = Channel4Pulse;	
	TIM_OC4Init(TIM3, &TIM_OCInitStructure);

	/* TIM1 counter enable */
	TIM_Cmd(TIM3, ENABLE);

	/* TIM1 Main Output Enable */
	TIM_CtrlPWMOutputs(TIM3, ENABLE);

	#if 0
	/*default duty is zero*/
	TIM_SetCompare3(TIM3 , 0);	
	TIM_SetCompare4(TIM3 , 0);
	#endif

}

void TIM15_PWM_Config(void)	//PB14:89KHz
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	uint16_t TimerPeriod = 0;
	uint16_t Channel1Pulse = 0;

	/* Enable GPIO clock */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);

	GPIO_StructInit(&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_PinAFConfig(GPIOB, GPIO_PinSource14, GPIO_AF_1);

	/* TIM15 clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM15 , ENABLE);

	/*************************************************************************
		TIMx_ARR = TIM_Period , Auto-Reload Register
		TIMx_CCR1 = TIM_Pulse , Capture Compare Register

		** Prescaler = (TIMxCLK / TIMx counter clock) - 1
		
		** Timer clock = PWM frequency * 2^PWM resolution

		The objective is to generate PWM signal at xx KHz:
		** TIMx Frequency = TIMx counter clock/(TIM_Period + 1)
			-> 1/ TIMx Frequency = (TIM_Period + 1) * (TIM_Prescaler + 1) / TIMxCLK
			-> TIMx Frequency =  TIMxCLK / ((TIM_Period + 1) * (TIM_Prescaler + 1))
				=> example : 
					Freq = 100 Hz = TIMxCLK / ((TIM_Period + 1) * (TIM_Prescaler + 1))
								  = 48000000 / (4000-1     + 1) * (120 -1        + 1))
			
		** TIMx_Period = (SystemCoreClock / TIMx Frequency) - 1 , if prescaler = 1-1

		** duty cycle = (TIM_Pulse/ TIM_Period + 1)* 100
		** ChannelxPulse = DutyCycle * (TIM1_Period - 1) / 100	
	*************************************************************************/

	TimerPeriod = (SystemCoreClock / 89000 ) - 1;
	/* Compute CCR1 value to generate a duty cycle at 50% for channel 1 */
	Channel1Pulse = (uint16_t) (((uint32_t) 50 * (TimerPeriod - 1)) / 100);
	
	/* Time Base configuration */
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	TIM_TimeBaseStructure.TIM_Prescaler = 1 -1 ;
	TIM_TimeBaseStructure.TIM_Period = TimerPeriod;	
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInit(TIM15, &TIM_TimeBaseStructure);

	/* Channel 1, 2,3 and 4 Configuration in PWM mode */
	TIM_OCStructInit(&TIM_OCInitStructure);
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_Pulse = Channel1Pulse;
	TIM_OC1Init(TIM15, &TIM_OCInitStructure);

	/* TIM15 counter enable */
	TIM_Cmd(TIM15, ENABLE);

	/* TIM15 Main Output Enable */
	TIM_CtrlPWMOutputs(TIM15, ENABLE);

	#if 0
	/*default duty is zero*/
	TIM_SetCompare3(TIM3 , 0);	
	TIM_SetCompare4(TIM3 , 0);
	#endif

}

void TIM16_PWM_Config(uint32_t Freq)	//PB8	, range : 1Hz ~ 48M Hz
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	uint16_t TimerPeriod = 0;
	uint16_t Channel1Pulse = 0;

	//de-initial
	#if 1
	TIM_Cmd(TIM16, DISABLE);
	TIM_CtrlPWMOutputs(TIM16, DISABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, DISABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM16 , DISABLE);
	#endif
	
	/* Enable GPIO clock */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);

	GPIO_StructInit(&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_2);

	/* TIM16 clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM16 , ENABLE);

	/*************************************************************************
		TIMx_ARR = TIM_Period , Auto-Reload Register
		TIMx_CCR1 = TIM_Pulse , Capture Compare Register

		** Prescaler = (TIMxCLK / TIMx counter clock) - 1
		
		** Timer clock = PWM frequency * 2^PWM resolution

		The objective is to generate PWM signal at xx KHz:
		** TIMx Frequency = TIMx counter clock/(TIM_Period + 1)
			-> 1/ TIMx Frequency = (TIM_Period + 1) * (TIM_Prescaler + 1) / TIMxCLK
			-> TIMx Frequency =  TIMxCLK / ((TIM_Period + 1) * (TIM_Prescaler + 1))
				=> example : 
					Freq = 100 Hz = TIMxCLK / ((TIM_Period + 1) * (TIM_Prescaler + 1))
								  = 48000000 / (4000-1     + 1) * (120 -1        + 1))
			
		** TIMx_Period = (SystemCoreClock / TIMx Frequency) - 1 , if prescaler = 1-1

		** duty cycle = (TIM_Pulse/ TIM_Period + 1)* 100
		** ChannelxPulse = DutyCycle * (TIM1_Period - 1) / 100	
	*************************************************************************/

	if (Freq >=740)
		TimerPeriod = (SystemCoreClock / Freq ) -1;
	else
		TimerPeriod = ((SystemCoreClock/1000)/Freq) - 1; //(48000 / Freq ) -1;
	
	/* Compute CCR1 value to generate a duty cycle at 50% for channel 1 */
	Channel1Pulse = (uint16_t) (((uint32_t) 50 * (TimerPeriod - 1)) / 100);
	
	/* Time Base configuration */
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);

	if (Freq >=740)
		TIM_TimeBaseStructure.TIM_Prescaler = 1 - 1;
	else
		TIM_TimeBaseStructure.TIM_Prescaler = 1000 - 1;
	
	TIM_TimeBaseStructure.TIM_Period = TimerPeriod;	
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInit(TIM16, &TIM_TimeBaseStructure);

	/* Channel 1, 2,3 and 4 Configuration in PWM mode */
	TIM_OCStructInit(&TIM_OCInitStructure);
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_Pulse = Channel1Pulse;
	TIM_OC1Init(TIM16, &TIM_OCInitStructure);

	/* TIM16 counter enable */
	TIM_Cmd(TIM16, ENABLE);

	/* TIM16 Main Output Enable */
	TIM_CtrlPWMOutputs(TIM16, ENABLE);

	#if 0
	/*default duty is zero*/
	TIM_SetCompare3(TIM3 , 0);	
	TIM_SetCompare4(TIM3 , 0);
	#endif

}


/*
	TIMxCLK = PCLK1 = HCLK = SystemCoreClock
	TIMx counter clock = TIMxCLK /((Prescaler + 1)*(Period + 1))
	                = 48 MHz / ((7+1)*(11999+1))
	                = 500 Hz 
     ==> TIMx counter period = 2 ms
*/
void TIM6_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	
	/* TIMx clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);

	/* TIMx base configuration */
	TIM_TimeBaseStructure.TIM_Period = 11999;
	TIM_TimeBaseStructure.TIM_Prescaler = 7;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

	TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);
	
	/* TIM Interrupts enable */
	TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);

	/* TIMx enable counter */
	TIM_Cmd(TIM6, ENABLE);

	/* Enable the TIMx gloabal Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = TIM6_DAC_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void ADC_Average(void)
{
	uint32_t	ValueX=0;
	uint8_t 	i=0;

	if(ADCCounter<ADC_SampleNo)
	{
		ADCBuffer[ADCCounter]=ADCOld;
		ADCCounter++;
	}
	else	//when counter > ADC_SampleNo , reset counter
	{
		ADCCounter = 0;
	}

	for(i=0;i<ADC_SampleNo;i++)
	{
		ValueX+=ADCBuffer[i];
	}
	
	ValueX/=ADC_SampleNo;
	ADValue=(uint16_t)ValueX;
//	ADVoltage=((uint16_t)ValueX*ADC_VERF_MV)/0xFFF;

}	

void ADC_ConversionOutput(uint8_t Index)
{
	uint16_t ADC1ConvertedValue =0;
	uint16_t ADC1ConvertedVoltage=0;
	uint16_t ADCNew = 0 ;
//	uint32_t i=0;

    /* Test DMA1 TC flag */
    while((DMA_GetFlagStatus(DMA1_FLAG_TC1)) == RESET ); 
    
    /* Clear DMA TC flag */
    DMA_ClearFlag(DMA1_FLAG_TC1);
    
	ADCNew = RegularConvData_Tab[Index];		
	ADCOld=(ADCNew+ADCOld)/2;
	ADC_Average();

//	for (i=0;i<2;i++)
//		{
//			PRINTF("Value %d= %4d \r\n" ,i, RegularConvData_Tab[i]);
//		}

//	PRINTF("Value 0= %2d \r\n" , RegularConvData_Tab[0]);
//	PRINTF("Value 1= %2d \r\n" , RegularConvData_Tab[1]);
	
	ADC1ConvertedValue = ADValue;
//	PRINTF("ADC1ConvertedValue = %d \r\n" , ADC1ConvertedValue);
    
    /* Compute the voltage */
    ADC1ConvertedVoltage = (ADC1ConvertedValue *ADC1_VERF_MV)/0xFFF;
 	PRINTF("ADC1ConvertedVoltage = %d (mv)\r\n" ,ADC1ConvertedVoltage);   

}

/*
	ADC_IN0		PA0
	ADC_IN1		PA1
	ADC_IN2		PA2
	ADC_IN3		PA3
	ADC_IN4		PA4
	ADC_IN5		PA5
	ADC_IN6		PA6
	ADC_IN7		PA7
	ADC_IN10	PC0
	ADC_IN11	PC1
	ADC_IN12	PC2
	ADC_IN13	PC3
	ADC_IN14	PC4
	ADC_IN15	PC5

*/
void ADC_DMA_Config(void)	
{
	DMA_InitTypeDef   	DMA_InitStructure;
	ADC_InitTypeDef     ADC_InitStructure;
	GPIO_InitTypeDef    GPIO_InitStructure;
	
	/* ADC1 DeInit */  
	ADC_DeInit(ADC1);

	/* GPIOC Periph clock enable */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA|RCC_AHBPeriph_GPIOC, ENABLE);

	/* ADC1 Periph clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

	/* DMA1 clock enable */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1 , ENABLE);

	/* Configure ADC Channel11 and channel10 as analog input */

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|	\
								GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN ;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|	\
								GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN ;
	GPIO_Init(GPIOC, &GPIO_InitStructure);	

	/* DMA1 Channel1 Config */
	DMA_DeInit(DMA1_Channel1);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) &ADC1->DR ;//ADC1_DR_Address , 0x40012440
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)RegularConvData_Tab;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = ADC_ChannelNo;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);
	/* DMA1 Channel1 enable */
	DMA_Cmd(DMA1_Channel1, ENABLE);
  
	/* Initialize ADC structure */
	ADC_StructInit(&ADC_InitStructure);

	/* Configure the ADC1 in continuous mode withe a resolution equal to 12 bits  */
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE; 
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
//	ADC_InitStructure.ADC_ScanDirection = ADC_ScanDirection_Backward;
	ADC_InitStructure.ADC_ScanDirection = ADC_ScanDirection_Upward;	
	ADC_Init(ADC1, &ADC_InitStructure); 

	/* Convert the ADC1 Channel11 and channel10 with 55.5 Cycles as sampling time */ 
	ADC_ChannelConfig(ADC1, ADC_Channel_0 , ADC_SampleTime_239_5Cycles); 
	ADC_ChannelConfig(ADC1, ADC_Channel_1 , ADC_SampleTime_239_5Cycles); 
	ADC_ChannelConfig(ADC1, ADC_Channel_2 , ADC_SampleTime_239_5Cycles); 
	ADC_ChannelConfig(ADC1, ADC_Channel_3 , ADC_SampleTime_239_5Cycles); 	
	ADC_ChannelConfig(ADC1, ADC_Channel_4 , ADC_SampleTime_239_5Cycles); 
	ADC_ChannelConfig(ADC1, ADC_Channel_5 , ADC_SampleTime_239_5Cycles); 
	ADC_ChannelConfig(ADC1, ADC_Channel_6 , ADC_SampleTime_239_5Cycles); 
	ADC_ChannelConfig(ADC1, ADC_Channel_7 , ADC_SampleTime_239_5Cycles); 
	
	ADC_ChannelConfig(ADC1, ADC_Channel_10 , ADC_SampleTime_239_5Cycles); 
	ADC_ChannelConfig(ADC1, ADC_Channel_11 , ADC_SampleTime_239_5Cycles); 
	ADC_ChannelConfig(ADC1, ADC_Channel_12 , ADC_SampleTime_239_5Cycles); 
	ADC_ChannelConfig(ADC1, ADC_Channel_13 , ADC_SampleTime_239_5Cycles); 
	ADC_ChannelConfig(ADC1, ADC_Channel_14 , ADC_SampleTime_239_5Cycles); 
	ADC_ChannelConfig(ADC1, ADC_Channel_15 , ADC_SampleTime_239_5Cycles); 
	
	/* ADC Calibration */
	ADC_GetCalibrationFactor(ADC1);

	/* ADC DMA request in circular mode */
	ADC_DMARequestModeConfig(ADC1, ADC_DMAMode_Circular);

	/* ADC Calibration */
	ADC_GetCalibrationFactor(ADC1);

	/* Enable ADC_DMA */
	ADC_DMACmd(ADC1, ENABLE);  

	/* Enable the ADC peripheral */
	ADC_Cmd(ADC1, ENABLE);     

	/* Wait the ADRDY flag */
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_ADEN)); 
	
	/* ADC1 regular Software Start Conv */ 
	ADC_StartOfConversion(ADC1);
}


void USART1_Test(void)
{
	__IO uint8_t temp;
	
	if(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == SET)
	{
			temp = USART_ReceiveData(USART1);
			PRINTF("Press KEY : %c \n\r",temp);

			switch (temp)
			{

				case '1' :
					TimCounLower = TimCounLower - 100;
					PRINTF("Freq : %4d\r\n" ,5000 -TimCounLower);					
					TIM16_PWM_Config(5000 -TimCounLower);					
					break;

				case '2':
					TimCounUpper = TimCounUpper + 100;
					PRINTF("TimerCounter : %4d\r\n" ,TimCounUpper);		
					TIM16_PWM_Config(5000 + TimCounUpper);	
					
//					PRINTF("Pulse : %4d\r\n" , TIM_GetCapture1(TIM3));
					break;	
					
				case '3':
//					TIM_SetCompare1(TIM3 , TIM3_DutyConvert(750));//75%
					break;

				case '4':
//					PRINTF("Pulse : %4d\r\n" , TIM_GetCapture2(TIM3));
					break;						

				case '5':
//					TIM_SetCompare2(TIM3 , TIM3_DutyConvert(400));//40%
					break;	

				case '8':
					PRINTF("GPIOB_Input_GetState()  = %d\r\n",GPIO_Input_GetState(GPIOB,GPIO_Pin_12));

					break;

				case '9':
					GPIO_Output_SetState(GPIOD,GPIO_Pin_2 , TRUE);

					PRINTF("SET GPIOD_Output_SetState (PD2) to TRUE\r\n");
					break;

				case '0':
					GPIO_Output_SetState(GPIOD,GPIO_Pin_2 , FALSE);
					PRINTF("SET GPIOD_Output_SetState (PD2) to FALSE\r\n");					
					break;
					
				default : 
					PRINTF("INPUT CMD not support !\r\n");
					break;
			}

	}
}

void USART1_Config(void)	//TX : PA9 , RX : PA10
{
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Clock configuration ---------------------------------------------------*/
	/* Enable GPIO clock */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

	/* Enable USART clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	/* GPIO configuration ----------------------------------------------------*/
	GPIO_DeInit(GPIOA);

	/* Connect PXx to USARTx_Tx */
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_1);
	/* Connect PXx to USARTx_Rx */
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_1);

	/* Configure USARTx_Tx,USARTx_Rx as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* USART configuration ---------------------------------------------------*/
	USART_DeInit(USART1);

	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);

	/* Enable USARTy Receive and Transmit interrupts */
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); 
	//USART_ITConfig(USART1, USART_IT_TXE, ENABLE);

	/* The software must wait until TC=1. The TC flag remains cleared during all data
	transfers and it is set by hardware at the last frame’s end of transmission*/	
	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
	{}

	/* NVIC configuration */
	/* Enable the USARRx Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn; 
	NVIC_InitStructure.NVIC_IRQChannelPriority = 2; 
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
	NVIC_Init(&NVIC_InitStructure); 

	/* Enable the USARRx */
	USART_Cmd(USART1, ENABLE);
}

void SysTickTimer_Config(void)
{
	RCC_GetClocksFreq(&RCC_ClockFreq);
	
	#if 1 //debug
	PRINTF("===========================\r\n");
	PRINTF("SYSCLK_Frequency = %d \r\n" , 		RCC_ClockFreq.SYSCLK_Frequency);
	PRINTF("HCLK_Frequency = %d \r\n" , 		RCC_ClockFreq.HCLK_Frequency);
	PRINTF("PCLK_Frequency = %d \r\n" , 		RCC_ClockFreq.PCLK_Frequency);
	PRINTF("ADCCLK_Frequency= %d \r\n" , 	RCC_ClockFreq.ADCCLK_Frequency);
	PRINTF("CECCLK_Frequency = %d \r\n" , 	RCC_ClockFreq.CECCLK_Frequency);
	PRINTF("I2C1CLK_Frequency = %d \r\n" , 	RCC_ClockFreq.I2C1CLK_Frequency);
	PRINTF("USART1CLK_Frequency = %d \r\n" , 	RCC_ClockFreq.USART1CLK_Frequency); 
	#endif /*debug*/
	
	/* Setup SysTick Timer for 1ms interrupts  */
	if (SysTick_Config(SystemCoreClock / 1000))
	{
		/* Capture error */
		while (1);
	}
	
	/* Configure the SysTick handler priority */
	NVIC_SetPriority(SysTick_IRQn, 0x01);
}

/**
  * @brief  Inserts a delay time.
  * @param  nTime: specifies the delay time length, in milliseconds.
  * @retval None
  */
void Delay(__IO uint32_t uTime)
{ 
	uwTimingDelay = uTime;
	while(uwTimingDelay != 0);
}

//void Delay_s(__IO uint32_t mTime)
//{ 
//	uint32_t i;
//	for(i=0;i<mTime;i++)
//		Delay_ms(1000);
//}

/**
  * @brief  Decrements the TimingDelay variable.
  * @param  None
  * @retval None
  */
void TimingDelay_Decrement(void)
{
  if (uwTimingDelay != 0x00)
  { 
    uwTimingDelay--;
  }
}

//currently not use
/*

void SystemClkDelay(void)
{
	uint32_t i;
	i = 0xffff;
	while(i--);
}

void wtPutChar(uint8_t ccc)
{
	UART1_SendData8(ccc);
	// Loop until the end of transmission 
	while (UART1_GetFlagStatus(UART1_FLAG_TXE) == RESET);	
}

u16 GetAbsTime(u16 a,u16 b)
{
	u16 c;
	if(a>=b) c=(a-b);
	else c=65535-(b-a);	
	
	return c;
}
*/

void UART_SendByte(uint8_t Data)
{
	USART_SendData(USART1 , (unsigned char)Data);
	while (USART_GetFlagStatus(USART1 , USART_FLAG_TXE)==RESET);
	{
	}
}

void UART_SendString(uint8_t* Data,uint16_t len)
{
	uint16_t i=0;
	for(i=0;i<len;i++ )
	{
		UART_SendByte(Data[i]);
	}
}

void SystemClkDelay(uint32_t u32Delay)
{
	//uint32_t i;
	//i = 0xffff;
	while(u32Delay--);
}


