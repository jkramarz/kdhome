#include <public.h>
#include <hardware.h>
#include <settings.h>
#include <delay.h>
#include <myprintf.h>
#include <ow.h>

#include "utils.h"
#include "ir.h"
#include "io.h"
#include "temp.h"
#include "providers.h"

#ifdef ETHERNET
#include "ethernet.h"
#endif
#ifdef ESP
#include "esp.h"
#endif

volatile uint32_t ticks = 0;
int dodump = 0;

void myputchar(int c)
{
	USART1->DR = c;
	while (!(USART1->SR & USART_SR_TC));
}

void main()
{
	SCB->VTOR = 0x08000000;
	
	// set clock source as HSI / 2 * (PLL) 8
	// RCC->CFGR |= RCC_CFGR_PLLMULL8 | RCC_CFGR_PLLXTPRE_HSE_Div2 /*| RCC_CFGR_PLLSRC*/;
	// RCC->CR |= RCC_CR_PLLON;
	// while (!(RCC->CR & RCC_CR_PLLRDY));
	// RCC->CFGR |= RCC_CFGR_SW_PLL;
	// while (!(RCC->CFGR & RCC_CFGR_SWS_PLL));
	
	// enable peripherals
	RCC->APB1ENR = RCC_APB1ENR_TIM2EN;
	RCC->APB2ENR = RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_IOPCEN | RCC_APB2ENR_ADC1EN | RCC_APB2ENR_USART1EN | RCC_APB2ENR_AFIOEN;
	
	AFIO->MAPR = AFIO_MAPR_SWJ_CFG_JTAGDISABLE;
	
	// configure USART
	IO_ALT_PUSH_PULL(UART_TX);
	USART1->BRR = USART_BRR(230400);
	USART1->CR1 = USART_CR1_UE | USART_CR1_TE | USART_CR1_RE | USART_CR1_RXNEIE;
	ENABLE_INTERRUPT(USART1_IRQn);
	
	// enable systick
	SysTick->LOAD = SysTick->VAL = (F_CPU / 1000) / 8;
	SysTick->CTRL = /*SysTick_CTRL_CLKSOURCE |*/ SysTick_CTRL_ENABLE | SysTick_CTRL_TICKINT;
	
	_delay_init();
	_delay_ms(1000);

	myprintf("starting...\r\n");
	
	IO_PUSH_PULL(LED);
	IO_HIGH(LED);
	
	// for(;;)
	// {
	// IO_TOGGLE(LED);
	// _delay_ms(100);
	// }
	
	IO_INPUT(IN1);
	IO_INPUT(IN2);
	IO_INPUT(IN3);
	IO_INPUT(IN4);
	IO_INPUT(IN5);
	IO_INPUT(IN6);
	IO_INPUT(IN7);
	IO_INPUT(IN8);
	
	IO_PUSH_PULL(OUT1);
	IO_PUSH_PULL(OUT2);
	IO_PUSH_PULL(OUT3);
	IO_PUSH_PULL(OUT4);
	IO_PUSH_PULL(OUT5);
	IO_PUSH_PULL(OUT6);
	IO_PUSH_PULL(OUT7);
	IO_PUSH_PULL(OUT8);
	
	OW_UART_init();
	tempInit();
	ioInit();
	irInit();
#ifdef ETHERNET
	ethInit();
#endif
#ifdef ESP
	espInit();
#endif
	
	uint8_t b;
	uint32_t lastCheck = 0;
	for (;;)
	{
		ioProcess();
		
		tempProcess();
		
		irProcess();
		provTmr();
#ifdef ETHERNET
		ethProcess();
#endif
#ifdef ESP
		espProcess();
#endif
	}
}

void USART1_Handler()
{
	if (USART1->SR & USART_SR_RXNE)
	{
		uint8_t d = USART1->DR;
		if (d == 0x7f)
		{
			*((unsigned long*)0x0E000ED0C) = 0x05FA0004;
			while (1);
		}
	}
}
void USART2_Handler()
{
#ifdef ESP
	espUsartHandler();
#endif
}

void SysTick_Handler()
{
	ticks++;
}
void _errorloop()
{
	while (1)
	{
		IO_TOGGLE(LED);
		_delay_ms(100);
	}
}
