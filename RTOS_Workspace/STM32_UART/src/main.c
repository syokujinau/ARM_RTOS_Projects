/**
  ******************************************************************************
  * @file    main.c
  * @author  YC Lin
  * @version V1.0
  * @date    2018/07/25
  * @brief   STM32 UART
  ******************************************************************************
*/

// semihosting
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "task.h"

TaskHandle_t xTaskHandle1 = NULL;
TaskHandle_t xTaskHandle2 = NULL;

void vTask1_handler(void *params);
void vTask2_handler(void *params);

#ifdef USE_SEMIHOSTING
// used for semihosting
extern void initialise_monitor_handles();
#endif

//
static void prvSetupHardware(void);
static void prevSetupUart(void);

//
char msg[100] = "Hello world!";

void printMsg(char *msg);

char user_msg[250] = {0};

// Flag
#define TRUE            1
#define FALSE           0
#define AVAILABLE       TRUE
#define NON_AVAILABLE   FALSE
uint8_t UART_ACCESS_KEY = AVAILABLE;


int main(void)
{

#ifdef USE_SEMIHOSTING
	initialise_monitor_handles();
	printf("Hello semihosting UART GOGOGO\n");
#endif

	// Enable CYCCNT in DWT_CTRL for SEGGER
	DWT->CTRL |= (1 << 0);

	// 1. Reset the RCC clock config to the default reset state.
	// HSI On, PLL Off, system clock = cpu clock = 16MHz
	RCC_DeInit();

	// 2. update the SystemCoreClock variable
	SystemCoreClockUpdate();

	// set up peripherals
	prvSetupHardware();

	sprintf(user_msg, "This is hello world application starting\r\n");
	printMsg(user_msg);


	// SEGGER start Recording
	SEGGER_SYSVIEW_Conf();
	SEGGER_SYSVIEW_Start();

	// 3. lets create 2 tasks, task-1 and task-2
	xTaskCreate(
				vTask1_handler,
				"Task-1",
				configMINIMAL_STACK_SIZE,
				NULL,
				2,
				&xTaskHandle1

	);

	xTaskCreate(
				vTask2_handler,
				"Task-2",
				configMINIMAL_STACK_SIZE,
				NULL,
				2,
				&xTaskHandle2
	);

	// 4. Start the scheduler
	vTaskStartScheduler();

	// will never reach here because vTaskStartScheduler doesn't return
	for(;;);
}


void vTask1_handler(void *params) {

	while(1){

		if(UART_ACCESS_KEY == AVAILABLE) {

			UART_ACCESS_KEY = NON_AVAILABLE;
			printMsg("Hello, I'm task-1\r\n");
			UART_ACCESS_KEY = AVAILABLE;
			taskYIELD();

		}

	}

}


void vTask2_handler(void *params) {

	while(1){

		if(UART_ACCESS_KEY == AVAILABLE) {

			UART_ACCESS_KEY = NON_AVAILABLE;
			printMsg("Hello, I'm task-2\r\n");
			UART_ACCESS_KEY = AVAILABLE;
			taskYIELD();

		}

	}

}

static void prevSetupUart(void) {
	// peripheral related initialization

	// 1. Enable the UART2 and GPIOA Peripheral clock
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	// 2.
	// PA2: UART2_TX
	// PA3: UART2_RX
	// Alternate function configuration of MCU pins to behave as UART2 Tx, Rx
	GPIO_InitTypeDef gpio_uart_pins;

	memset(&gpio_uart_pins, 0, sizeof(gpio_uart_pins)); // clear the structure
	gpio_uart_pins.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
	gpio_uart_pins.GPIO_Mode = GPIO_Mode_AF;
	gpio_uart_pins.GPIO_PuPd = GPIO_PuPd_UP; // for idle state

	GPIO_Init(GPIOA, &gpio_uart_pins);

	// 3. AF mode settings for the pins
	GPIO_PinAFConfig(
			GPIOA,            // PA2
			GPIO_PinSource2,  // PA2
			GPIO_AF_USART2
	);
	GPIO_PinAFConfig(
			GPIOA,            // PA3
			GPIO_PinSource3,  // PA3
			GPIO_AF_USART2
	);

	// 4. UART parameter initializations
	USART_InitTypeDef uart2_init;

	memset(&uart2_init, 0, sizeof(uart2_init)); // clear the structure
	uart2_init.USART_BaudRate = 115200;
	uart2_init.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	uart2_init.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	uart2_init.USART_Parity = USART_Parity_No;
	uart2_init.USART_StopBits = USART_StopBits_1;
	uart2_init.USART_WordLength = USART_WordLength_8b;

	USART_Init(
			USART2,
			&uart2_init
	);

	// 5. Enable the UART2 peripheral
	USART_Cmd(USART2, ENABLE);
}

static void prvSetupHardware(void) {
	// setup UART2
	prevSetupUart();
}

void printMsg(char *msg) {

	for(uint32_t i = 0; i < strlen(msg); i++) {

		while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) != SET); // waiting if transmit data register empty flag doesn't set.
		USART_SendData(USART2, msg[i]);

	}

}

