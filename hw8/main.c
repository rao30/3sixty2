
#include "stm32f0xx.h"
#include "stm32f0308_discovery.h"
extern uint32_t wavetab[];

volatile int counter = 0;
volatile int wave = 0;
int main(void)
{
    if ((RCC->CFGR & RCC_CFGR_SWS) == RCC_CFGR_SWS_PLL) /* (1) */
    {
        RCC->CFGR &= (uint32_t) (~RCC_CFGR_SW); /* (2) */
        while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI) /* (3) */
        {
            /* For robust implementation, add here time-out management */
        }
    }
    RCC->CR &= (uint32_t)(~RCC_CR_PLLON);/* (4) */
    while((RCC->CR & RCC_CR_PLLRDY) != 0) /* (5) */
    {
        /* For robust implementation, add here time-out management */
    }
    RCC->CFGR = (RCC->CFGR & (~RCC_CFGR_PLLMUL)) | (RCC_CFGR_PLLMUL12); /* (6) */
    RCC->CR |= RCC_CR_PLLON; /* (7) */
    while((RCC->CR & RCC_CR_PLLRDY) == 0) /* (8) */
    {
        /* For robust implementation, add here time-out management */
    }
    RCC->CFGR |= (uint32_t) (RCC_CFGR_SW_PLL); /* (9) */
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL) /* (10) */
    {
        /* For robust implementation, add here time-out management */
    }
    RCC ->AHBENR = 0x000E0000;
    GPIOC -> MODER |= 0x00050000;
    GPIOA -> MODER |= 0x00AAF000;  //PWM pins 8,9,10,11 and Analog input pins 6,7
    GPIOA ->AFR[1] =  0x00002222;

    GPIOA->MODER |= 0x300;
    GPIOA ->AFR[0] |=  0x00050000;
    //TIMER2 CONFIG


    //NVIC CONFIG


    //PWM SETUP
    RCC ->APB2ENR |= 0x00000800;
    TIM1->PSC = 0; /* (1) */
    TIM1->ARR = 1024; /* (2) */
    TIM1->CR1 =0x20; /* (7) */
    TIM1->CCR1 = 0; /* (3) */
    TIM1->CCMR1 =0x60;
    TIM1->CCER =0x1;
    TIM1->BDTR =0x8000; /* (6) */
    TIM1->DIER |= 0x1;
    NVIC->ISER[0]=0x2000;
    NVIC->ICPR[0]=0x2000;
    //DAC SETUP
    TIM1->CR1 |=0x1; /* (7) */
    RCC->APB1ENR |= 0x20000000;
    DAC->CR &= 0xFFFFFFFD;
    DAC->CR |= 0x01;


    while(1) {

    }
}

extern void TIM1_BRK_UP_TRG_COM_IRQHandler() {
    TIM1 -> SR = 0x0;
    wave = wavetab[counter];
    //PWM
    TIM1->CCR1 = wave;
    //DAC
    DAC->DHR12R1 = wave;
    //Increment counter once flag is set
    counter = counter+1;
    if(counter == 124) {
        counter = 0;
    }
}
