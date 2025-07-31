/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : blockingqueue.c
  * @brief          : 테스크 2의 우선 순위를 높고 낮게 하여, 컨텍스트 스위칭 해주는 예제
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

#include "main.h"
#include "driver_init.h"


TaskHandle_t xTask1Handle = NULL;
TaskHandle_t xTask2Handle = NULL;


void Task1(void *pvParameters); // previous priority 2
void Task2(void *pvParameters); // previous priority 1



int main(){
  Driver_Init();
  BaseType_t result;
  result = xTaskCreate(Task2, "Task2", configMINIMAL_STACK_SIZE, NULL, 1,  &xTask2Handle);
  if(result != pdPASS) {
    HAL_UART_Transmit(&huart3, (uint8_t *)"Task2 creation failed\r\n", 24, HAL_MAX_DELAY);
  } else {
    HAL_UART_Transmit(&huart3, (uint8_t *)"Task2 created successfully\r\n", 28, HAL_MAX_DELAY);
  }

  result = xTaskCreate(Task1, "Task 1", configMINIMAL_STACK_SIZE, NULL, 2,  &xTask1Handle);
  if (result != pdPASS)
  {
      //vPrintString("Task 1 creation failed!\r\n");
  }
  vTaskStartScheduler();

  for(;;){

  }

  return 0;
}



void Task1(void *pvParameters)
{
    UBaseType_t uxPriority;

    // Get Task 1's priority
    uxPriority = uxTaskPriorityGet(NULL);

    for (;;)
    {

      // vPrintString("Task 1 is running\r\n");
        const char* msg = "Task 1 raise priority of Task2\r\n";
        HAL_UART_Transmit(&huart3, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);
        vTaskPrioritySet(xTask2Handle, uxPriority + 1);
    }
}

void Task2(void *pvParameters)
{
    UBaseType_t uxPriority;

    // Get Task 2's original priority
    uxPriority = uxTaskPriorityGet(NULL);

    for (;;)
    {
        const char* msg = "Task 2 lower task2 priority\r\n";
        HAL_UART_Transmit(&huart3, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);
      
        // Lower its own priority to below Task 1 again
        vTaskPrioritySet(NULL, uxPriority - 2);

        // Control returns to Task 1
    }
}


