#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include <stdio.h>

SemaphoreHandle_t xMutex;

void vTask1(void *pvParameters)
{
    while (1)
    {
        if (xSemaphoreTake(xMutex, portMAX_DELAY) == pdTRUE)
        {
            printf("🔵 Task1: Mutex 얻음\n");
            vTaskDelay(pdMS_TO_TICKS(1000));  // 공유 자원 사용
            printf("🔵 Task1: Mutex 반환\n");
            xSemaphoreGive(xMutex);
        }
        vTaskDelay(pdMS_TO_TICKS(100)); // 다음 루프까지 대기
    }
}

void vTask2(void *pvParameters)
{
    while (1)
    {
        if (xSemaphoreTake(xMutex, portMAX_DELAY) == pdTRUE)
        {
            printf("🟢 Task2: Mutex 얻음\n");
            vTaskDelay(pdMS_TO_TICKS(1000));  // 공유 자원 사용
            printf("🟢 Task2: Mutex 반환\n");
            xSemaphoreGive(xMutex);
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

int main(void)
{
    // 드라이버 초기화 등 필요한 초기화 코드가 있다면 여기에 작성

    // Mutex 생성
    xMutex = xSemaphoreCreateMutex();
    if (xMutex == NULL)
    {
        printf("❌ Mutex 생성 실패!\n");
        while (1);
    }

    // Task 생성
    xTaskCreate(vTask1, "Task1", 256, NULL, 2, NULL);
    xTaskCreate(vTask2, "Task2", 256, NULL, 2, NULL);

    // 스케줄러 시작
    vTaskStartScheduler();

    // 이 지점은 실행되지 않음
    while (1);
}
