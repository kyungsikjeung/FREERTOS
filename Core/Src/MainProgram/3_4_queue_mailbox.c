
/**
  ******************************************************************************
  * @file           : mailboxqueue.c
  * @brief          : 두 개의 송신 Task가 각각 100과 200을 주기적으로 메일박스 큐에 덮어쓰며 전송하고, 
  *                   수신 Task는 해당 큐로부터 최신 데이터를 읽어 UART로 출력하는 예제.
  *                   큐는 길이 1로 생성되며, 항상 최신 상태값만 유지한다.
  * @details        : 기존 큐와 달리, 데이터가 가득차면 전송 실패, FAIL 과 달리 xQueueOverwrite()를 통해 데이터를 최신으로 덮어쓴다.
  *                   수신 Task는 xQueuePeek()을 통해 데이터를 큐에서 제거하지 않고 읽기만 한다.
  *                   수신자는 마지막으로 수신한 timestamp와 비교하여 중복 처리 없이 최신 데이터만 출력한다.
  ******************************************************************************
  */

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
        // HIGHLIGHT: range 30-32
        dataToSend.timestamp = xTaskGetTickCount();
        xQueueOverwrite(xMailbox, &dataToSend);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void vReaderTask(void *pvParams) {
    Example_t receivedData;
    TickType_t lastTimestamp = 0;

    for (;;) {
        // HIGHLIGHT: range 43-44
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
    Driver_Init();
    xMailbox = xQueueCreate(1, sizeof(Example_t));  // 길이 1: 메일박스

    if (xMailbox != NULL) {
        xTaskCreate(vUpdaterTask, "Updater", 1000, NULL, 2, NULL);
        xTaskCreate(vReaderTask,  "Reader",  1000, NULL, 1, NULL);
        vTaskStartScheduler();
    }
    for (;;);  // 실행 안됨
}
