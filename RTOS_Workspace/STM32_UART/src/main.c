/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   STM32 UART
  ******************************************************************************
*/

// semihosting
#include <stdio.h>
#include <stdint.h>

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

static void prvSetupHardware(void);

int main(void)
{

#ifdef USE_SEMIHOSTING
	initialise_monitor_handles();
	printf("Hello semihosting UART GOGOGO\n");
#endif

	// 1. Reset the RCC clock config to the default reset state.
	// HSI On, PLL Off, system clock = cpu clock = 16MHz
	RCC_DeInit();

	// 2. update the SystemCoreClock variable
	SystemCoreClockUpdate();


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

	}

}


void vTask2_handler(void *params) {

	while(1){

	}

}

static void prvSetupHardware(void) {
	// peripheral related initialization

}
