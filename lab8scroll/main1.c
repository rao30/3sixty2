/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 ** This notice applies to any and all portions of this file
 * that are not between comment pairs USER CODE BEGIN and
 * USER CODE END. Other portions of this file, whether
 * inserted by the user or by software development tools
 * are owned by their respective copyright owners.
 *
 * COPYRIGHT(c) 2018 STMicroelectronics
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of STMicroelectronics nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f0xx_hal.h"
#include "tim.h"
#include "gpio.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
volatile int count = 0;
volatile int buttonPress = 0;
volatile int buttonFlag = 0;
//Variables for the scrolling display
volatile int scrollToggleb1 = 0;
volatile int buttonFlagb1 = 0;
volatile int buttonPrevb1 = 0;
volatile int buttonCurrentb1 = 0;
volatile int countb1;
volatile int scrollSum;
volatile int reverseSum;
//END scrolling display
volatile int buttonPrev[12];
volatile int buttonCurrent[12];
volatile int digit1 = 0;
volatile int digit10 = 0;
volatile int colLoop = 0;
volatile int sum = 0;
volatile int digit1prev = 0;
volatile int digit10prev = 0;
volatile int digit1S = 0;
volatile int digit10S = 0;
int toggle0display = 0;
int padding = 0;
static TIM_HandleTypeDef s_TimerInstance = {
		.Instance = TIM1
};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 *
 * @retval None
 */
int main(void)
{
	/* USER CODE BEGIN 1 */

	/* USER CODE END 1 */

	/* MCU Configuration----------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */
	HAL_TIM_Base_MspInit(&s_TimerInstance);
	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_TIM1_Init();
	/* USER CODE BEGIN 2 */
	HAL_TIM_Base_Start_IT(&s_TimerInstance);
	reset_num();
	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1){
		/* USER CODE END WHILE */
		keypadHandler();
		buttonHandler();
		/* USER CODE BEGIN 3 */

	}
	/* USER CODE END 3 */

}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{

	RCC_OscInitTypeDef RCC_OscInitStruct;
	RCC_ClkInitTypeDef RCC_ClkInitStruct;

	/**Initializes the CPU, AHB and APB busses clocks
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = 16;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL12;
	RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		_Error_Handler(__FILE__, __LINE__);
	}

	/**Initializes the CPU, AHB and APB busses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
			|RCC_CLOCKTYPE_PCLK1;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
	{
		_Error_Handler(__FILE__, __LINE__);
	}

	/**Configure the Systick interrupt time
	 */
	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

	/**Configure the Systick
	 */
	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

	/* SysTick_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* USER CODE BEGIN 4 */
writeDigit(int digit, int pos) {
	if(pos == 1) {
		write1(digit);
	}
	else if(pos == 10) {
		write10(digit);
	}
}

//Writes to the 1's digit led
void write1(int digit) {
	HAL_GPIO_WritePin(GPIOA, 0x0F00, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOC, 0x0380, GPIO_PIN_RESET);
	switch(digit) {
	case 0:
		HAL_GPIO_WritePin(GPIOA, 0x0F00, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOC, 0x0300, GPIO_PIN_SET);
		break;
	case 1:
		HAL_GPIO_WritePin(GPIOA,0x0600, GPIO_PIN_SET);
		break;
	case 2:
		HAL_GPIO_WritePin(GPIOA, 0x0D00, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOC, 0x0280, GPIO_PIN_SET);
		break;
	case 3:
		HAL_GPIO_WritePin(GPIOA, 0x0F00, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOC, 0x0080, GPIO_PIN_SET);
		break;
	case 4:
		HAL_GPIO_WritePin(GPIOA, 0x0600, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOC, 0x0180, GPIO_PIN_SET);
		break;
	case 5:
		HAL_GPIO_WritePin(GPIOA, 0x0B00, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOC, 0x0180, GPIO_PIN_SET);
		break;
	case 6:
		HAL_GPIO_WritePin(GPIOA, 0x0B00, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOC, 0x0380, GPIO_PIN_SET);
		break;
	case 7:
		HAL_GPIO_WritePin(GPIOA, 0x0E00, GPIO_PIN_SET);
		break;
	case 8:
		HAL_GPIO_WritePin(GPIOA, 0x0F00, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOC, 0x0380, GPIO_PIN_SET);
		break;
	case 9:
		HAL_GPIO_WritePin(GPIOA, 0x0E00, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOC, 0x0180, GPIO_PIN_SET);
		break;
	}
}

//Writes to the 10's digit LED
void write10(int digit) {
	HAL_GPIO_WritePin(GPIOB, 0x0C07, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOC, 0x0030, GPIO_PIN_RESET);
	switch(digit) {
	case 0:
		HAL_GPIO_WritePin(GPIOB, 0x0C07, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOC, 0x0020, GPIO_PIN_SET);
		break;
	case 1:
		HAL_GPIO_WritePin(GPIOB, 0x0404, GPIO_PIN_SET);
		break;
	case 2:
		HAL_GPIO_WritePin(GPIOB, 0x0C03, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOC, 0x0010, GPIO_PIN_SET);
		break;
	case 3:
		HAL_GPIO_WritePin(GPIOB, 0x0C06, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOC, 0x0010, GPIO_PIN_SET);
		break;
	case 4:
		HAL_GPIO_WritePin(GPIOB, 0x0404, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOC, 0x0030, GPIO_PIN_SET);
		break;
	case 5:
		HAL_GPIO_WritePin(GPIOB, 0x0806, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOC, 0x0030, GPIO_PIN_SET);
		break;
	case 6:
		HAL_GPIO_WritePin(GPIOB, 0x0807, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOC, 0x0030, GPIO_PIN_SET);
		break;
	case 7:
		HAL_GPIO_WritePin(GPIOB, 0x0C04, GPIO_PIN_SET);
		break;
	case 8:
		HAL_GPIO_WritePin(GPIOB, 0x0C07, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOC, 0x0030, GPIO_PIN_SET);
		break;
	case 9:
		HAL_GPIO_WritePin(GPIOB, 0x0C04, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOC, 0x0030, GPIO_PIN_SET);;
		break;
	}
}


//Runs through each pin and does the switch de-bouncing for a particular column
void matrixDebounce(int col) {
	colLoop++;
	switch (col) {
	case 0:
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, GPIO_PIN_SET);
		btnDebounce(0);
		btnDebounce(3);
		btnDebounce(6);
		btnDebounce(9);
		break;
	case 1:
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, GPIO_PIN_SET);
		btnDebounce(1);
		btnDebounce(4);
		btnDebounce(7);
		btnDebounce(10);
		break;
	case 2:
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, GPIO_PIN_RESET);
		btnDebounce(2);
		btnDebounce(5);
		btnDebounce(8);
		btnDebounce(11);
		break;
	}
}


//Checks which button was pressed and activates the particular button press flag
btnDebounce(int row){
	buttonPrev[row] = buttonCurrent[row];
	if (row == 0 | row == 1 | row == 2) {
		buttonCurrent[row] = HAL_GPIO_ReadPin (GPIOB, GPIO_PIN_7);
	}
	else if(row == 3 | row == 4 | row == 5) {
		buttonCurrent[row] = HAL_GPIO_ReadPin (GPIOB, GPIO_PIN_6);
	}
	else if(row == 6 | row == 7 | row == 8) {
		buttonCurrent[row] = HAL_GPIO_ReadPin (GPIOB, GPIO_PIN_5);
	}
	else if(row == 9 | row == 10 | row == 11) {
		buttonCurrent[row] = HAL_GPIO_ReadPin (GPIOB, GPIO_PIN_4);
	}
	if((buttonCurrent[row] == 0) && (buttonPrev[row] ==1)) {
		buttonFlag = 1;
		digit1prev = digit1;
		//Assigns a value to the first digit depending on which button was pressed
		digit1 = row+1;
	}
}
extern void TIM2_IRQHandler()
{
	HAL_TIM_IRQHandler(&s_TimerInstance);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	/*(write1(count);
	write10(count);
	count++;
	if(count == 10) {
		count = 0;
	} */
	countb1 = countb1+1;
	if(colLoop > 2) {
		colLoop = 0;
	}
	if(scrollToggleb1 == 0) {
		matrixDebounce(colLoop);
	}
	scrollCheck();
	if(scrollToggleb1 == 1 & countb1 == 50) {
		countb1 = 0;
		scroller();
	}

}

void scroller() {
	if(padding == 0) {
		if((sum == reverseSum) & (sum == 0)) {
			digit10S = digit1S;
			digit1S = reverseSum%10;
			padding = 1;
		}
		else if(scrollSum != 0) {
			digit10S = digit1S;
			digit1S = scrollSum%10;
			scrollSum = scrollSum/10;
			if(scrollSum == 0) {
				padding = 1;
				scrollSum = reverseSum;
			}
		}
	}
	else{
		digit10S = digit1S;
		digit1S  = -1;
		padding++;
		if(padding == 3) {
			padding = 0;
		}

	}
	writeDigit(digit1S, 1);
	writeDigit(digit10S,10);
}
void reset_num() {
	digit1 = 0;
	digit10 = 0;
	sum = 0;
	write1(0);
	write10(0);
}

//Decides what to do depending on which button was pressed and sets the value on the LED accordingly
//Deals with the 0 button press, add functionality and reset.
void keypadHandler() {
	if(buttonFlag == 1 ) {
		buttonFlag = 0;

		if(digit1 == 11) {
			digit1 = 0;
			digit10 = digit1prev;
		}
		else if(digit1 == 10) {
			summer();
		}
		else if(digit1 == 12) {
			reset_num();
		}
		else {
			digit10 = digit1prev;
		}

		writeDigit(digit1, 1);
		writeDigit(digit10, 10);

	}
	else if(scrollToggleb1 == 0) {
		writeDigit(digit1, 1);
		writeDigit(digit10, 10);
	}
}

//Deals with the auto scroll feautre to display the sum output.
void buttonHandler() {
	if(buttonFlagb1 == 1) {
		buttonFlagb1 = 0;
		scrollToggleb1 = !scrollToggleb1;
		if(scrollToggleb1 == 1) {
			scrollSum = reverseNum(sum);
			reverseSum = scrollSum;
			digit1S = -1;
			digit10S = -1;
			writeDigit(digit1S, 1);
			writeDigit(digit10S,10);
		}
		else {
			keypadHandler();
		}
		countb1 = 0;
	}
}

int reverseNum(int num) {
	int revNum = 0;
	while(num>0) {
		revNum = revNum*10 + num%10;
		num = num/10;
	}
	return(revNum);
}

//Returns the value of the 2 digits on display.
int digitVal () {
	int value = digit1prev + digit10*10;
	return value;
}

//Keeps a running total of the digits added up
void summer() {
	sum = sum + digitVal();
	digit1 = sum%10;
	digit1prev = digit1;
	int temp = sum/10;
	digit10 = temp%10;
}

//Checks to see if the scroll button was pressed and sets the button flag.
void scrollCheck() {
	buttonPrevb1 = buttonCurrentb1;
	buttonCurrentb1 = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0);
	if((buttonCurrentb1 == 0) && (buttonPrevb1 ==1)) {
		buttonFlagb1 = 1;
	}
}
/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @param  file: The file name as string.
 * @param  line: The line in file as a number.
 * @retval None
 */
void _Error_Handler(char *file, int line)
{
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	while(1)
	{
	}
	/* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t* file, uint32_t line)
{ 
	/* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	/* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/**
 * @}
 */

/**
 * @}
 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
