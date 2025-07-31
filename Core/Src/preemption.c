/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : preemption.c
  * @brief          : Preemption example for FreeRTOS
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

// Task messages
const char* task1_msg = "Task1 HI\r\n";
const char* task2_msg = "Task2 LI\r\n";

void Task1(void *pvParameters);
void Task2(void *pvParameters);

void waitForStableSchedulerTick(uint32_t wait_ms) {
    TickType_t now = xTaskGetTickCount();
    TickType_t until = now + pdMS_TO_TICKS(wait_ms);
    while (xTaskGetTickCount() < until) {
        taskYIELD();  // 다른 Task에게 CPU 양보
    }
}
  

int main(void)
{
  Driver_Init();
  
  // System clock debugging info
  //char clock_info[100];
  //sprintf(clock_info, "[DEBUG] SystemCoreClock: %lu Hz, TickRate: %d Hz\r\n", 
         // SystemCoreClock, configTICK_RATE_HZ);
  //HAL_UART_Transmit(&huart3, (uint8_t *)clock_info, strlen(clock_info), HAL_MAX_DELAY);

  // HIGHLIGHT: range 65-69
  // ! Task 1 우선 순위 더 높음,
  
  xTaskCreate(Task1, "Task1", 512, (void*)task1_msg, 2, NULL);
  xTaskCreate(Task2, "Task2", 512, (void*)task2_msg, 1, NULL);

  vTaskStartScheduler();

  // This part of the code should never be reached
  while (1)
  {

  }
}


// Task1: Higher priority
void Task1(void *pvParameters) {
    vTaskDelay(pdMS_TO_TICKS(3000));
    while (1) {
        // LED ON + message output
        HAL_GPIO_WritePin(GPIOB, LD1_Pin, GPIO_PIN_SET);
        HAL_UART_Transmit(&huart3, (uint8_t *)pvParameters, strlen(pvParameters), HAL_MAX_DELAY);

        // Wait 1 second (LED stays ON)
        vTaskDelay(pdMS_TO_TICKS(1000)); 
        
        // LED OFF
        HAL_GPIO_WritePin(GPIOB, LD1_Pin, GPIO_PIN_RESET);
        
        // No additional delay - immediately loop back (1초마다 실행)
    }
}

// Task2: Lower priority
void Task2(void *pvParameters) {
    const char* msg = (const char*) pvParameters;
    while (1) {
        // This task runs continuously, toggling the Green LED (LD2)
        HAL_GPIO_TogglePin(GPIOB, LD2_Pin); 
        HAL_UART_Transmit(&huart3, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);
        
        // HIGHLIGHT: line 57
        // ! 1초마다 실행 
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}     