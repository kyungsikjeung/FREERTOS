/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : blockingqueue.c
  * @brief          : 두 개의 송신 Task가 각각 100과 200을 주기적으로 큐에 전송하고, 
  *                   수신 Task가 해당 큐로부터 데이터를 수신하여 UART로 출력하는 예제.
  *                   수신 Task는 가장 높은 우선순위를 가지며, 큐에 데이터가 존재하는지
  *                   상태를 확인한 후 데이터를 즉시 처리함.
  * @details         :수신자가 모든 데이터를 순차적으로 수신하고, 큐가 가득 차면 송신자는 대기하거나 실패
  ******************************************************************************
  */
/* USER CODE END Header */
#include "main.h"
#include "driver_init.h"

typedef struct {
    uint32_t value;
    TickType_t timestamp;
} Example_t;

QueueHandle_t xMailbox;

void vUpdaterTask(void *pvParams) {
    Example_t dataToSend;
    uint32_t counter = 0;

    for (;;) {
        dataToSend.value = counter++;
        // HIGHLIGHT: range 29-30
        dataToSend.timestamp = xTaskGetTickCount();
        xQueueOverwrite(xMailbox, &dataToSend);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void vReaderTask(void *pvParams) {
    Example_t receivedData;
    TickType_t lastTimestamp = 0;

    for (;;) {
        // HIGHLIGHT: range 42-43
        if (xQueuePeek(xMailbox, &receivedData, portMAX_DELAY) == pdPASS) {
            if ((TickType_t)(receivedData.timestamp - lastTimestamp) > 0) {
                printf("Received value: %lu (Tick: %lu)\r\n",
                       (unsigned long)receivedData.value,
                       (unsigned long)receivedData.timestamp);
                lastTimestamp = receivedData.timestamp;
            }
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

int main(void) {
    xMailbox = xQueueCreate(1, sizeof(Example_t));  // 길이 1: 메일박스

    if (xMailbox != NULL) {
        xTaskCreate(vUpdaterTask, "Updater", 1000, NULL, 2, NULL);
        xTaskCreate(vReaderTask,  "Reader",  1000, NULL, 1, NULL);
        vTaskStartScheduler();
    }

    for (;;);  // 실행 안됨
}
