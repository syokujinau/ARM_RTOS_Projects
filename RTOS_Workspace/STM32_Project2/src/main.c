/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    2018/08/02
  * @brief   Project2 - using external interrupt of button (PC13) and led task
  ******************************************************************************
*/


#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "task.h"
#include <string.h>
#include <stdint.h>
			

static void prevSetupUart(void);
static void preSetupGPIO(void);
static void prvSetupHardware(void);
void printMsg(char *msg);

// task handler prototypes
void led_task_handler(void *params);
//void button_task_handler(void *params);

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

	// SEGGER
	SEGGER_SYSVIEW_Conf();
	SEGGER_SYSVIEW_Start();

	// 4. Create LED and Button Task
	xTaskCreate(
			led_task_handler,
			"LED task",
			configMINIMAL_STACK_SIZE,
			NULL,
			1,
			NULL
	);

//	xTaskCreate(
//			button_task_handler,
//			"button task",
//			configMINIMAL_STACK_SIZE,
//			NULL,
//			1,
//			NULL
//	);

	// 5. Start Scheduler
	vTaskStartScheduler();


	for(;;);
}

void led_task_handler(void *params) {
    /* led_task should turn on the LED if flag is SET and vice versa. */

	while(1) {

		if(button_status_flag == PRESSED) {

			GPIO_WriteBit(GPIOA, GPIO_Pin_5, Bit_SET);

		} else {

			GPIO_WriteBit(GPIOA, GPIO_Pin_5, Bit_RESET);

		}

	}

}

//void button_task_handler(void *params) {
//	/* button_task should continuously poll the button status, if the button is pressed, it should update the flag variable. */
//
//	while(1) {
//
//		if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13)){
//
//			button_status_flag = PRESSED;
//
//		} else {
//
//			button_status_flag = NOT_PRESSED;
//
//		}
//
//	}
//
//}

void button_handler (void *params) {

	button_status_flag ^= 1; // toggle the flag

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

	// 3. interrupt configuration for the button (PC13)
	//// 3-1. system configuration for exti line (SYSCFG settings)
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource13);
	//// 3-2. EXTI line configuration 13, falling edge, interrupt mode
	EXTI_InitTypeDef exti_init;
	exti_init.EXTI_Line    = EXTI_Line13;
	exti_init.EXTI_Mode    = EXTI_Mode_Interrupt;
	exti_init.EXTI_Trigger = EXTI_Trigger_Falling;
	exti_init.EXTI_LineCmd = ENABLE;

	EXTI_Init(&exti_init);

	// 4. NVIC settings (IRQ settings for the selected EXTI line 13)
	// [Reference] search "Vector table for STM32F411xC/E" in https://www.st.com/content/ccc/resource/technical/document/reference_manual/9b/53/39/1c/f7/01/4a/79/DM00119316.pdf/files/DM00119316.pdf/jcr:content/translations/en.DM00119316.pdf
	// Need to mention IRQ number of EXTI13 for NVIC
	NVIC_SetPriority(EXTI15_10_IRQn, 5); // 0 ~ 15, 0 is highest, I choose 5 here.
	NVIC_EnableIRQ(EXTI15_10_IRQn);
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

}

void EXTI15_10_IRQHandler(void) {
	traceISR_ENTER(); // SEGGER
	// 1. clear the interrupt pending bit of the EXTI line 13
	// Since the pending bit is set that is an indication to the NVIC that the interrupt is happened.
	EXTI_ClearITPendingBit(EXTI_Line13);

	// 2. call our handler
	button_handler(NULL);
	traceISR_EXIT(); // SEGGER
}
