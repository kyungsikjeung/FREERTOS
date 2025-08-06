#include "main.h"
#include "driver_init.h"


QueueHandle_t xQueue;

/* Source 종류 */
typedef enum {
    SOURCE_HMI,
    SOURCE_CAN,
    SOURCE_ANOTHER
} DataSource_t;

/* 전송할 데이터 구조체 */
typedef struct{
    DataSource_t source;
    uint32_t uxValue;
}sendData_t;

/* 전송할 데이터 예시 */
sendData_t dataToSend[3] = {
    {SOURCE_HMI, 100},
    {SOURCE_CAN, 200},
    {SOURCE_ANOTHER, 300}
};


static void sendTask1(void *pvParameters) {
    sendData_t sendDataPointer = *(sendData_t* )pvParameters; // {SOURCE_HMI, 100} - copy from dataToSend
    sendDataPointer.uxValue = 111; // update Value, 실 코드에서는 실제 HMI에서 받은 값으로 업데이트
    while (1) {
        xQueueSendToBack(xQueue, &sendDataPointer, portMAX_DELAY);
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

static void sendTask2(void *pvParameters) {
    sendData_t sendDataPointer = *(sendData_t* )pvParameters; // {SOURCE_CAN, 200} - copy from dataToSend
    sendDataPointer.uxValue = 222; // update Value  
    while (1) {
        xQueueSendToBack(xQueue, &sendDataPointer, portMAX_DELAY);
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

static void sendTask3(void *pvParameters) {
    sendData_t sendDataPointer = *(sendData_t* )pvParameters; // {SOURCE_ANOTHER, 300} - copy from dataToSend
    sendDataPointer.uxValue = 333; // update Value
    while (1) {
        xQueueSendToBack(xQueue, &sendDataPointer, portMAX_DELAY);
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

static void receiveTask(void *pvParameters) {
    sendData_t receivedData;
    while (1) {
        if(uxQueueMessagesWaiting(xQueue) != 3) {
            const char *msg = "Queue should have been empty!\r\n";
            HAL_UART_Transmit(&huart3, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);
        }
        if (xQueueReceive(xQueue, &receivedData, portMAX_DELAY) == pdPASS) {
            printf("Received from source %d: value = %lu\r\n", receivedData.source, receivedData.uxValue);
            /* Data Processing */
            // switch (receivedData.source) {
            //     case SOURCE_HMI:
            //         // Process HMI data
            //         break;
            //     case SOURCE_CAN:
            //         // Process CAN data
            //         break;
            //     case SOURCE_ANOTHER:
            //         // Process another data
            //         break;
            //     default:
            //         break;
            // }
        }else{
            const char *msg = "Failed to receive data from queue!\r\n";
            HAL_UART_Transmit(&huart3, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);
        }
    }
}

int main(){
    Driver_Init();
    xQueue = xQueueCreate(4, sizeof(sendData_t));
    xTaskCreate(sendTask1,"SOURCE_HMI", 128, &dataToSend[0], 1, NULL);
    xTaskCreate(sendTask2,"SOURCE_CAN", 128,&dataToSend[1], 1, NULL);
    xTaskCreate(sendTask3,"SOURCE_ANOTHER", 128,&dataToSend[2], 1, NULL);
    xTaskCreate(receiveTask, "Task1", 128, NULL, 4, NULL);
    vTaskStartScheduler();   
    return 0;
}






