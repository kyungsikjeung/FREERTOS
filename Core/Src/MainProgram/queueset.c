#include "driver_init.h"
#include "main.h"



static QueueHandle_t xQueue1 = NULL;
static QueueHandle_t xQueue2 = NULL;
static QueueSetHandle_t xQueueSet = NULL;

void vSenderTask1(void *pvParameters);
void vSenderTask2(void *pvParameters);
void vReceiverTask(void *pvParameters);

int main(){
    Driver_Init();

    xQueue1 = xQueueCreate(1, sizeof(char *));
    xQueue2 = xQueueCreate(1, sizeof(char *));
    xQueueSet = xQueueCreateSet(2);
        // 3. 큐를 큐셋에 추가
    xQueueAddToSet(xQueue1, xQueueSet);
    xQueueAddToSet(xQueue2, xQueueSet);

    xTaskCreate(vSenderTask1, "Sender1", 1000, NULL, 1, NULL);
    xTaskCreate(vSenderTask2, "Sender2", 1000, NULL, 1, NULL);

    xTaskCreate(vReceiverTask, "Receiver", 1000, NULL, 2, NULL);
    vTaskStartScheduler();
    while(1);
    return 0;
}

void vSenderTask1(void *pvParameters){
    const TickType_t xBlockTime = pdMS_TO_TICKS(100);
    const char * const pcMessage = "Message from vSenderTask1\r\n";
    for (;;)
    {
        vTaskDelay(xBlockTime); // 100ms 대기
        xQueueSend(xQueue1, &pcMessage, 0);
    }
}
void vSenderTask2(void *pvParameters){
    const TickType_t xBlockTime = pdMS_TO_TICKS(100);
    const char * const pcMessage  = "test hello world\r\n";
    for(;;){
        vTaskDelay(xBlockTime); // 100ms 대기
        xQueueSend(xQueue2, &pcMessage, 0);
    }
}

void vReceiverTask(void *pvParameters)
{
    QueueHandle_t xQueueThatContainsData;
    char *pcReceivedString;

    for (;;)
    {
        // 큐셋에서 데이터가 도착한 큐의 핸들 선택
        xQueueThatContainsData = (QueueHandle_t)xQueueSelectFromSet(xQueueSet, portMAX_DELAY);

        // 해당 큐에서 메시지 포인터 수신
        xQueueReceive(xQueueThatContainsData, &pcReceivedString, 0);

        // 문자열 출력
        printf("%s", pcReceivedString);
    }
}
