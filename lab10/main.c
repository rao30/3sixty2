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
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* USER CODE BEGIN Includes */
#include <string.h>
#include <stdio.h>
#include <math.h>
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
char buffer[100];
int len;
char result_buffer[100];
char pause_buffer[50];
char Rx_indx, Rx_data[2], Rx_Buffer[100], Transfer_cplt;
char cal_en[] = "\nTimer Mode Enabled.\n";
char time[] = "\nEnter Time: ";
char op[] = "Specify an operator(+,-,*,/,c); c to clear: ";
char comms[] = "\n*****Command List*****\nRun: run the timer\nReset: reset the timer to the set value\nSet: specify a new timer value";
char running[] = "\nRunning, type 'Pause' to pause :";
char timeout[] = "Timeout.";
volatile int tx_flag;
int valid_op = 0;

volatile int total = 0;
volatile int operand = 0;
int zero_error = 0;

volatile int tout = 0;
volatile int time_val = 0;
volatile int reset = 0;
volatile int set = 1;
volatile int run = 0;
volatile int pause = 0;
volatile int led_flag = 0;
volatile int led_counter = 0;
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
	int operator = 0;
	/* USER CODE END 1 */

	/* MCU Configuration----------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */
	HAL_TIM_Base_MspInit(&s_TimerInstance);
	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_USART1_UART_Init();
	MX_SPI1_Init();
	MX_TIM1_Init();
	/* USER CODE BEGIN 2 */
	HAL_TIM_Base_Start_IT(&s_TimerInstance);
	__HAL_UART_ENABLE_IT(&huart1, UART_IT_TC);
	__HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);
	//while(HAL_UART_GetState(&huart1) != HAL_UART_STATE_READY);
	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	/*char a[] = {0x31,0x32};
	int k = 5;
	TransmitChar('0'+k);
	ResetPos();
	TransmitChar('0'+k+1); */
	HAL_Delay(15);
	tx_flag = 1;
	HAL_UART_Transmit_IT(&huart1, cal_en, sizeof(cal_en));
	while(tx_flag);
	while (1)
	{
		if(set == 1) {
			total = Time_wait();
			time_val = total;
			timer_pause();
			commands();
			//pause_wait();
		}
		else if(pause == 1) {
			commands();
		}
		else if(run == 1) {
			pause_wait();
		}
		else if(reset == 1) {
			timer_pause();
			commands();
		}
		else if(tout == 1) {
			led_flag = 1;
			led_counter = 0;
			print_timeout();
			commands();
		}
	}
}
/* USER CODE END WHILE */

/* USER CODE BEGIN 3 */
int Time_wait() {
	tx_flag = 1;
	Transfer_cplt = 0;
	HAL_UART_Transmit_IT(&huart1, time, sizeof(time));
	while(tx_flag);
	while(!Transfer_cplt);
	return atoi(Rx_Buffer);
}

void print_result() {
	tx_flag = 1;
	sprintf(result_buffer, "Result: %d\n", total);
	//HAL_UART_Transmit_IT(&huart1, itoa(total), sizeof(itoa(total)));
	HAL_UART_Transmit_IT(&huart1, result_buffer, sizeof(result_buffer));
	while(tx_flag);

}
void print_timeout() {
	tx_flag = 1;
	HAL_UART_Transmit_IT(&huart1, "\nTimeout. ", sizeof("\nTimeout. "));
	while(tx_flag);
}
void timer_pause() {
	ResetPos();
	TransmitNum(total);
	tx_flag = 1;
	sprintf(pause_buffer, "Timer paused. Timer Value: %d\n", total);
	HAL_UART_Transmit_IT(&huart1, pause_buffer, sizeof(pause_buffer));
	while(tx_flag);
}

void commands() {
	comm_list();
	comm_wait();
}
void comm_list() {
	tx_flag = 1;
	HAL_UART_Transmit_IT(&huart1, comms, sizeof(comms));
	while(tx_flag);
}

void comm_wait() {
	tx_flag = 1;
	Transfer_cplt = 0;
	HAL_UART_Transmit_IT(&huart1, "\nCommand: ", sizeof("\nCommand: "));
	while(tx_flag);
	while(!Transfer_cplt);
	comm_check(&Rx_Buffer);
}
void comm_check(char *buff) {
	if(strcmp(buff, "Run") == 0) {
		run = 1;
		reset = 0;
		pause = 0;
		set = 0;
		/*tx_flag = 1;
		HAL_UART_Transmit_IT(&huart1, running, sizeof(running));
		while(tx_flag); */
	}
	else if(strcmp(buff, "Reset") == 0) {
		reset = 1;
		run = 0;
		pause = 0;
		set = 0;
		tout = 0;
		total = time_val;
	}
	else if(strcmp(buff, "Set") == 0) {
		reset = 0;
		run = 0;
		pause = 0;
		set = 1;
		tout = 0;
	}
}
void pause_wait() {
	Transfer_cplt = 0;
	//while((!tout) & (!Transfer_cplt));
	tx_flag = 1;
	HAL_UART_Transmit_IT(&huart1, running, sizeof(running));
	while(tx_flag);
	//while(!Transfer_cplt);
	while((!tout) & (!Transfer_cplt));
	if(Transfer_cplt == 1) {
		pause_check(&Rx_Buffer);
	}
	else if(tout == 1) {
		reset = 0;
		run = 0;
		pause = 0;
		set = 0;
		Transfer_cplt = 1;
	}
}
void pause_check(char *buff) {
	if(strcmp(buff, "Pause") == 0) {
		reset = 0;
		run = 0;
		pause = 1;
		set = 0;
		timer_pause();
	}
}
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {

	if(run == 1 & !pause) {
		if(total == 0) {
			tout = 1;
			run = 0;
		}
		else {
			total = total - 1;
			ResetPos();
			//TransmitChar(('0'+total));
			TransmitNum(total);
		}
	}
		if(led_flag == 1) {
			led_counter++;
		}
		if(led_counter >= 4) {
			led_flag = 0;
		}
		if(led_flag == 1 & (led_counter < 4)) {
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8,GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9,GPIO_PIN_SET);
		}
		else {
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8,GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9,GPIO_PIN_RESET);
		}
}

void TransmitChar(char ch) {
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi1, &ch, sizeof(ch), 10);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
	//ResetPos();
	//HAL_Delay(1);
	/* USER CODE END 3 */

}

void ResetPos() {
	char a = 0xFE;
	TransmitChar(a);
	a = 0x51;
	TransmitChar(a);
	a = 0;
	TransmitChar(a);
}

void TransmitNum(int val) {
	int digit = val;
	int count = 0;
	while(val > 9) {
		count = 0;
		while (digit > 9) {
			digit = digit/10;
			count++;
		}
		TransmitChar('0'+digit);
		for(int i = 0; i < count; i++) {
			digit = digit*10;
		}
		val = val - digit;
		digit = val;
	}
	TransmitChar('0'+val);
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{

	RCC_OscInitTypeDef RCC_OscInitStruct;
	RCC_ClkInitTypeDef RCC_ClkInitStruct;
	RCC_PeriphCLKInitTypeDef PeriphClkInit;

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
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV64;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV16;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
	{
		_Error_Handler(__FILE__, __LINE__);
	}

	PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1;
	PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_SYSCLK;
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
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

//Interrupt callback routine

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
