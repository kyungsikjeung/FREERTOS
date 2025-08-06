/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : changepriority.c
  * @brief          : Task1 기준으로 Task2 우선순위를 올리고 내리는 예제 (컨텍스트 스위칭)
  ******************************************************************************
  */
/* USER CODE END Header */

#include "main.h"
#include "driver_init.h"

// 전역 태스크 핸들
TaskHandle_t xTask1Handle = NULL;
TaskHandle_t xTask2Handle = NULL;

// 태스크 선언
void Task1(void *pvParameters);
void Task2(void *pvParameters);

int main() {
    Driver_Init();
    BaseType_t result;

    // Task2 생성 (우선순위 1)
    result = xTaskCreate(Task2, "Task2", configMINIMAL_STACK_SIZE, NULL, 1, &xTask2Handle);
    if(result != pdPASS) {
        HAL_UART_Transmit(&huart3, (uint8_t *)"Task2 생성 실패\r\n", strlen("Task2 생성 실패\r\n"), HAL_MAX_DELAY);
    } else {
        HAL_UART_Transmit(&huart3, (uint8_t *)"Task2 생성 성공\r\n", strlen("Task2 생성 성공\r\n"), HAL_MAX_DELAY);
    }

    // Task1 생성 (우선순위 2)
    result = xTaskCreate(Task1, "Task1", configMINIMAL_STACK_SIZE, NULL, 2, &xTask1Handle);
    if(result != pdPASS) {
        HAL_UART_Transmit(&huart3, (uint8_t *)"Task1 생성 실패\r\n", strlen("Task1 생성 실패\r\n"), HAL_MAX_DELAY);
    }

    // 스케줄러 시작
    vTaskStartScheduler();

    while (1) {
        // 코드 도달하지 않음
    }

    return 0;
}

// Task1: Task2의 우선순위를 높임
void Task1(void *pvParameters) {
    UBaseType_t uxPriority = uxTaskPriorityGet(NULL); // 현재 Task1 우선순위

    for (;;) {
        const char* msg = "Task1: Task2의 우선순위를 높입니다.\r\n";
        HAL_UART_Transmit(&huart3, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);

        // Task2 우선순위 = Task1보다 높게 설정
        vTaskPrioritySet(xTask2Handle, uxPriority + 1);

        // Task1은 여기서 선점당함
    }
}

// Task2: 자신의 우선순위를 다시 낮춤
void Task2(void *pvParameters) {
    UBaseType_t uxPriority = uxTaskPriorityGet(NULL); // 현재 Task2 우선순위

    for (;;) {
        const char* msg = "Task2: 자신의 우선순위를 다시 낮춥니다.\r\n";
        HAL_UART_Transmit(&huart3, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);

        // 우선순위 다시 낮춰서 Task1에 CPU 양보
        vTaskPrioritySet(NULL, uxPriority - 2);
    }
}
