/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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

// 전역 문자열로 선언 (메모리에 계속 유지됨)
const char* task1_msg = "Task1 Msg  !\r\n";
const char* task2_msg = "Task2 Msg  !\r\n";

 // HIGHLIGHT: range 25-47
 // ! TASK DECLARATION, 
void Task1(void *pvParameters) {
    const char* msg = (const char*) pvParameters;
    while (1) {
        // Do something
        HAL_UART_Transmit(&huart3, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);
        HAL_GPIO_TogglePin(GPIOB, LD1_Pin);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}


void Task2(void *pvParameters) {
    const char* msg = (const char*) pvParameters;

    while (1) {
        // Do something
        HAL_UART_Transmit(&huart3, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);
        HAL_GPIO_TogglePin(GPIOB, LD2_Pin);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}


int main(void)
{
  Driver_Init();
  
  // TaskHandle_t을 선언해서 태스크 생성 결과를 확인할 수 있습니다
  TaskHandle_t xTask1Handle = NULL;
  TaskHandle_t xTask2Handle = NULL;
  
  // xTaskCreate(함수포인터, "태스크이름", 스택크기, 파라미터, 우선순위, 태스크핸들)
  xTaskCreate(Task1, "Task1", 256, (void*)task1_msg, 1, &xTask1Handle);
  xTaskCreate(Task2, "Task2", 256, (void*)task2_msg, 1, &xTask2Handle);

  // 스케줄러 시작
  vTaskStartScheduler();
  
  // 여기 코드는 실행되지 않아야 함 (스케줄러가 제어권을 가져감)
  while (1)
  {
   
  }  
}





