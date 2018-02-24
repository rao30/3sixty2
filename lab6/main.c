/**
 ******************************************************************************
 * @file    main.c
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
volatile int tenthFlag = 0;
volatile int run = 1;
static int thresh1 = 0x327;
static int thresh2 = 0x64E;
static int thresh3 = 0x975;
static int thresh4 = 0xC9C;
static int thresh5 = 0xFB4;
//static int ADC_READY = 0x80000000;

int main(void)
{
	//GPIO CONFIG
	int adcVal = 0;
	int ledB = 0;
	int ledBprev = 0;
	RCC ->AHBENR = 0x000E0000;
	GPIOC -> MODER |= 0x00050000;
	GPIOA -> MODER |= 0x0000F000;
	GPIOB -> MODER = 0x55555555;
	//TIMER CONFIG
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

	//ADC1 ->CR |= 0x80000000;
	//ADC calibration

	//while((ADC1->ISR & ADC_ISR_RDY) == 0);

	//adcEnable(7);
	//GPIOB->ODR = 0xF;
	while(1) {
		checkPress();
		if((run == 1) && (tenthFlag == 1)) {
			tenthFlag = 0;
			adcEnable(6);
			adcVal = adcRead();
			ledB = checkLed(adcVal);
			ledB = lightB1(ledB);
			ledBprev = GPIOB->ODR;
			GPIOB ->ODR = ledB;
			adcEnable(7);
			adcVal = adcRead();
			ledB = checkLed(adcVal);
			ledB = lightB2(ledB);
			GPIOB ->ODR = ledB;
		/*	if(adcVal > 0x7E1) {
				GPIOB->ODR |= 0x0000FFFF;
		}
			else {
				GPIOB ->ODR &= ~(0x0000FFFF);
			} */
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

void checkPress(void) {
		if(buttonFlag == 1) {
			buttonFlag = 0;
			run = run^1; //TOggle the run/ pause functionality
			GPIOC->ODR = GPIOC->ODR^0x00000100;
		}
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

int ledOn1(int pin) {
	int a = 0xE018;
	int b = 0x1;
	a = GPIOB->ODR & a;
	b = b << pin;
	a = a|b;
	return a;
}
int ledOn2(int pin) {
	int a = 0x1C06;
	int b = 0x1;
	a = GPIOB->ODR & a;
	b = b << pin;
	a = a|b;
	return a;
}

int lightB1(int ledNum) {
	switch(ledNum) {

	case 0:
		return GPIOB->ODR&~(0x1C06);
		break;
	case 1:
		return ledOn1(12);
		break;
	case 2:
		return ledOn1(11);
		break;
	case 3:
		return ledOn1(10);
		break;
	case 4:
		return ledOn1(2);
		break;

	case 5:
		return ledOn1(1);
		break;

	default:
		return 0x0;
		break;
	}
}

int lightB2(int ledNum) {
	switch(ledNum) {

	case 0:
		return GPIOB->ODR&~(0xE018);
		break;
	case 1:
		return ledOn2(13);
		break;
	case 2:
		return ledOn2(14);
		break;
	case 3:
		return ledOn2(15);
		break;
	case 4:
		return ledOn2(3);
		break;

	case 5:
		return ledOn2(4);
		break;

	default:
		return 0x0;
		break;
	}
}
int checkLed(int adcVal) {
	int ledNum = 0;
	if(adcVal < thresh1) {
		ledNum = 0;
		return ledNum;
	}
	else if(adcVal <thresh2) {
		ledNum = 1;
		return ledNum;
	}
	else if(adcVal <thresh3) {
			ledNum = 2;
			return ledNum;
		}
	else if(adcVal <thresh4) {
			ledNum = 3;
			return ledNum;
		}
	else if(adcVal <thresh5) {
			ledNum = 4;
			return ledNum;
		}
	else {
		ledNum = 5;
		return ledNum;
	}
}
extern void TIM2_IRQHandler() {
	TIM2 -> SR = 0x0;
	//Increment counter once flag is set
	if(run == 1) {
	counter = counter+1;
	if(counter == 20) {
		counter = 0;
		tenthFlag = 1;
	}
	}
	buttonPrev = buttonCurrent;
	buttonCurrent = GPIOA->IDR;
	buttonCurrent &= 0x1;
	if((buttonCurrent == 0) && (buttonPrev ==1)) {
		buttonFlag = 1;
	}
}
