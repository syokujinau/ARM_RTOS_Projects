/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    2018/08/02
  * @brief   Project3 - Task Notification APIs
  ******************************************************************************
*/


#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "task.h"
#include <string.h>
#include <stdint.h>
#include <stdio.h>
			

// task handle for task notification
TaskHandle_t xTaskHandle1 = NULL;
TaskHandle_t xTaskHandle2 = NULL;

// prototypes
static void prevSetupUart(void);
static void preSetupGPIO(void);
static void prvSetupHardware(void);
void printMsg(char *msg);
void rtos_delay(uint32_t delay_in_ms);

// UART
char user_msg[250] = {0};
// task handler prototypes
void led_task_handler(void *params);
void button_task_handler(void *params);

// Global variable for flags
#define FALSE        0
#define TRUE         1
#define NOT_PRESSED  FALSE
#define PRESSED      TRUE
uint8_t button_status_flag = NOT_PRESSED;

int main(void)
{
	// SEGGER
	DWT->CTRL |= (1 << 0);
	// 1. Reset the RCC clock config to the default reset state.
	// HSI On, PLL Off, system clock = cpu clock = 16MHz
	RCC_DeInit();

	// 2. update the SystemCoreClock variable
	SystemCoreClockUpdate();

	// 3. set up peripherals
	prvSetupHardware();

	// UART
	sprintf(user_msg, "Task Notify APIs\r\n");
	printMsg(user_msg);

	// SEGGER
	SEGGER_SYSVIEW_Conf();
	SEGGER_SYSVIEW_Start();

	// 4. Create LED and Button Task
	xTaskCreate(
			led_task_handler,
			"LED task",
			500, // 500 words
			NULL,
			2,
			&xTaskHandle1
	);

	xTaskCreate(
			button_task_handler,
			"button task",
			500,
			NULL,
			2,
			&xTaskHandle2
	);

	// 5. Start Scheduler
	vTaskStartScheduler();


	for(;;);
}

void led_task_handler(void *params) {
    /* led_task should turn on the LED if flag is SET and vice versa. */
	uint32_t current_notification_value = 0;

	while(1) {

		// wait until the led_task receive any notification event from button_task

		if( xTaskNotifyWait(0, 0, &current_notification_value, portMAX_DELAY) == pdTRUE ) {
		    // received the notification. should toggle the led
		    GPIO_ToggleBits(GPIOA, GPIO_Pin_5);
		    sprintf(user_msg, "The notification is received, count = %ld \r\n", current_notification_value);
		    printMsg(user_msg);
		}

	}

}

void button_task_handler(void *params) {
	/* button_task should continuously poll the button status, if the button is pressed, it should update the flag variable. */

	while(1) {

		if( !GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13) ) {
			// PA13 = 0, button is pressed

			// delay time for debouncing, otherwise the notification will be sent many times.
			rtos_delay(100); // wait for 100 ticks

			// Should notify
			xTaskNotify(
					xTaskHandle1,
					0x0, // don't send any value
					eIncrement
			);
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

static void preSetupGPIO(void) {

	// 1. Enable AHB1
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE); // for EXTI configuration

	// 2. Initialize GPIO
	GPIO_InitTypeDef led_init,
					 btn_init;

	// led init structure
	led_init.GPIO_Mode  = GPIO_Mode_OUT;
	led_init.GPIO_OType = GPIO_OType_PP;
	led_init.GPIO_Pin   = GPIO_Pin_5;
	led_init.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	led_init.GPIO_Speed = GPIO_Medium_Speed;

	GPIO_Init(GPIOA, &led_init);

	// button init structure
	btn_init.GPIO_Mode  = GPIO_Mode_IN;
	btn_init.GPIO_Pin   = GPIO_Pin_13;
	btn_init.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	btn_init.GPIO_Speed = GPIO_Medium_Speed;

	GPIO_Init(GPIOC, &btn_init);
}

static void prvSetupHardware(void) {
	// setup Button and LED
	preSetupGPIO();
	// setup UART2
	prevSetupUart();
}

void printMsg(char *msg) {

	for(uint32_t i = 0; i < strlen(msg); i++) {

		while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) != SET); // waiting if transmit data register empty flag doesn't set.
		USART_SendData(USART2, msg[i]);

	}

	while(USART_GetFlagStatus(USART2, USART_FLAG_TC) != SET);

}



void rtos_delay(uint32_t delay_in_ms) {

	uint32_t current_tick_count = xTaskGetTickCount();

	uint32_t delay_in_ticks = (delay_in_ms * configTICK_RATE_HZ) / 1000;

	while( xTaskGetTickCount() < (current_tick_count + delay_in_ticks));

}




