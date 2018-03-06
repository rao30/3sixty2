/**
 ******************************************************************************
 * @file    Part2
 * @author  Ac6
 * @version V1.0
 * @date    01-December-2013
 * @brief   Default main function.
 ******************************************************************************
 */


#include "stm32f0xx.h"
#include "stm32f0308_discovery.h"

volatile int buttonPress = 0;
volatile int buttonFlag = 0;
volatile int buttonPrev = 0;
volatile int buttonCurrent = 0;
volatile int counter = 0;
volatile int counterFlag = 0;
volatile int flag_50ms = 0;
int main(void)
{
	int adcVal = 0;
	RCC ->AHBENR = 0x000E0000;
	GPIOC -> MODER |= 0x00050000;
	GPIOA -> MODER |= 0x00AAF000;  //PWM pins 8,9,10,11 and Analog input pins 6,7
	GPIOB -> MODER = 0x55555555;
	GPIOA ->AFR[1] =  0x00002222;
	//TIMER2 CONFIG
	RCC ->APB1ENR = 0x1;
	TIM2 ->PSC = 7999;
	TIM2 -> ARR = 4;
	TIM2 ->DIER = 0x1;
	TIM2 -> CR1 = 0x1;

	//NVIC CONFIG
	NVIC_EnableIRQ(15);
	//ADC CONFIG
	RCC ->APB2ENR |= RCC_APB2ENR_ADC1EN;
	RCC->CR2 |= RCC_CR2_HSI14ON;
	while((RCC->CR2 & RCC_CR2_HSI14RDY) == 0);
	ADC1->CR |= ADC_CR_ADEN;
	while((ADC1->ISR & ADC_ISR_ADRDY) == 0);

	//PWM SETUP
	RCC ->APB2ENR |= 0x00000800;
	TIM1->PSC = 0; /* (1) */
	TIM1->ARR = 0x00001000; /* (2) */
	TIM1->CCR1 = 0; /* (3) */
	TIM1->CCMR1 |= TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1PE; /* (4) */
	TIM1->CCER |= TIM_CCER_CC1E; /* (5) */
	TIM1->BDTR |= TIM_BDTR_MOE; /* (6) */
	TIM1->CR1 |= TIM_CR1_CMS_0 | TIM_CR1_CEN; /* (7) */
	TIM1->EGR |= TIM_EGR_UG; /* (8) */

	while(1) {
		if(flag_50ms == 1) {
			flag_50ms = 0;
			adcEnable(6);
			adcVal = adcRead();
			TIM1->CCR1 = adcVal;
		}
	}

}


void adcEnable(int channel) {
	// Wait for ADC to be ready
	while((ADC1->ISR & ADC_ISR_ADRDY) == 0);
	// Ensure ADCStart = 0
	while((ADC1->CR & ADC_CR_ADSTART) == 1);
	ADC1->CHSELR = 0;
	ADC1->CHSELR |= 1 << channel;
}


int adcRead(void) {
int adcValue = 0;
// Wait for ADC to be ready
while((ADC1->ISR & ADC_ISR_ADRDY) == 0);
// Start the ADC (ADCStart = 1)
ADC1->CR |= ADC_CR_ADSTART;
// Wait for end of conversion
while((ADC1->ISR & ADC_ISR_EOC) == 0);
adcValue = ADC1->DR;
return adcValue;
}


extern void TIM2_IRQHandler() {
	TIM2 -> SR = 0x0;
	//Increment counter once flag is set
	counter = counter+1;
	if(counter == 10) {
		counter = 0;
		flag_50ms = 1;
	}
	buttonPrev = buttonCurrent;
	buttonCurrent = GPIOA->IDR;
	buttonCurrent &= 0x1;
	if((buttonCurrent == 0) && (buttonPrev ==1)) {
		buttonFlag = 1;
	}
}
