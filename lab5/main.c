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
			
//6.1 TURNING ON AN LED
/*int main(void)
{
	RCC ->AHBENR = 0x000A0000;
	GPIOC -> MODER = 0x00050000;
	GPIOA -> MODER = 0x00000000;
	GPIOC -> ODR = 0x00000100;
	while(1);
} */


//6.2 USE BUTTON TO TOGGLE LED
/*int main(void)
{
	RCC ->AHBENR = 0x000A0000;
	GPIOC -> MODER = 0x00050000;
	//GPIOA -> MODER = 0x0;
	int a = 0;


	while(1) {
		a = GPIOA->IDR;
		a &= 0x1;
		if(a == 1) {
		GPIOC -> ODR = 0x00000200;
		}
		else {
			GPIOC -> ODR = 0x00000100;
		}
	}
}
 */
//6.3 USING TIMER FOR DEBOUNCING
/*
volatile int buttonPress = 0;
int buttonFlag = 0;
int buttonPrev = 0;
volatile int buttonCurrent = 0;
int main(void)
{
	//GPIO CONFIG
	RCC ->AHBENR = 0x000A0000;
	GPIOC -> MODER = 0x00050000;
	//GPIOA -> MODER = 0x00000000;
	//TIMER CONFIG
	RCC ->APB1ENR = 0x1;
	TIM2 ->PSC = 7999;
	TIM2 -> ARR = 4;
	TIM2 ->DIER = 0x1;
	TIM2 -> CR1 = 0x1;
	int a = 0;
	//NVIC CONFIG
	NVIC_EnableIRQ(15);
	while(1) {
		if(buttonFlag == 1) {
			buttonFlag = 0;
			a = GPIOC ->ODR;
			a &= 0x0000200;
			if(a == 0) {
				GPIOC -> ODR = 0x00000200;
			}
			else {
				GPIOC -> ODR = 0x0;
			}
			}
		}
	}


extern void TIM2_IRQHandler() {
	TIM2 -> SR = 0x0;
	buttonPrev = buttonCurrent;
	buttonCurrent = GPIOA->IDR;
	buttonCurrent &= 0x1;
	if((buttonCurrent == 0) && (buttonPrev ==1)) {
		buttonFlag = 1;
	}
}*/
volatile int buttonPress = 0;
volatile int buttonFlag = 0;
volatile int buttonPrev = 0;
volatile int buttonCurrent = 0;
volatile int counter = 0;
volatile int counterFlag = 0;
volatile int victoryFlag = 0;
volatile int defeatCounter = 0;
volatile int defeatFlag = 0;
int main(void)
{
	//GPIO CONFIG
	RCC ->AHBENR = 0x000A0000;
	GPIOC -> MODER = 0x00050000;
	//GPIOA -> MODER = 0x00000000;
	//TIMER CONFIG
	RCC ->APB1ENR = 0x1;
	TIM2 ->PSC = 7999;
	TIM2 -> ARR = 4;
	TIM2 ->DIER = 0x1;
	TIM2 -> CR1 = 0x1;
	int a = 0;
	//NVIC CONFIG
	NVIC_EnableIRQ(15);
	while(1) {
		counterStart();
		checkPress();
		if(victoryFlag == 1) {
			victory();
		}
		else {
			defeat();
		}
		resetLoop();
	}
}




void counterStart(void) {
	while(1) {
		if(buttonFlag == 1) {
			buttonFlag = 0;
			counterFlag = 1;
			break;
		}

	}
}

void checkPress(void) {
	while(1) {
		if(buttonFlag == 1) {
			buttonFlag = 0;
			if((counter > 900) && (counter < 1100)) {
				victoryFlag = 1;
			}
			else {
				victoryFlag = 0;
			}
			break;
		}
		if(counter > 2000) {
			victoryFlag = 0;
			break;
		}
	}
}

void victory(void) {
	GPIOC -> ODR |= 0x00000200;
}

void defeat(void) {
	GPIOC -> ODR |= 0x00000200;
	defeatFlag = 1;
	while(1) {
		if(defeatCounter == 200) {
			if(buttonFlag == 1) {
				break;
			}
			int b = 0x00000200;
			b = ~b;
			GPIOC -> ODR &= b;
			defeatCounter = 0;
			break;
		}
		}
	while(1) {
			if(defeatCounter == 200) {
				if(buttonFlag == 1) {
				break;
				}
				int b = 0x00000200;
				GPIOC -> ODR |= b;
				defeatCounter = 0;
				break;
			}
		}
	while(1) {
			if(defeatCounter == 200) {
				if(buttonFlag == 1) {
								break;
				}
				int b = 0x00000200;
				b = ~b;
				GPIOC -> ODR &= b;
				defeatCounter = 0;
				break;
			}
		}
}

void resetLoop(void) {
	while(1) {
		if(buttonFlag == 1) {
			GPIOC -> ODR = 0x00000000;
			buttonFlag = 0;
			counter = 0;
			counterFlag = 0;
			defeatFlag = 0;
			defeatCounter = 0;
			victoryFlag = 0;
			break;
		}
	}
}
extern void TIM2_IRQHandler() {
	TIM2 -> SR = 0x0;

	//Increment counter once flag is set
	if(counterFlag == 1) {
		counter = counter + 1;
	}
	if(defeatFlag == 1)
	{
		defeatCounter = defeatCounter + 1;
	}
	if(counter == 1000) {
		GPIOC -> ODR |= 0x00000100;
	}
	buttonPrev = buttonCurrent;
	buttonCurrent = GPIOA->IDR;
	buttonCurrent &= 0x1;
	if((buttonCurrent == 0) && (buttonPrev ==1)) {
		buttonFlag = 1;
	}
}
