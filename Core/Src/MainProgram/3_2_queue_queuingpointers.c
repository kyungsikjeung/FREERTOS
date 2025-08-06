#include "main.h"
#include "driver_init.h"

QueueHandle_t xQueue;

void SenderTask(void *pvParameters) {
    char* pcToSendStirng;
    size_t sendMaxLength = 50;
    BaseType_t sendIndex = 0;
    while (1) {
        pcToSendStirng  = (char *)pvPortMalloc(50);
        snprintf(pcToSendStirng,sendMaxLength,"Hello from SenderTask %d\r\n", sendIndex++);
        HAL_UART_Transmit(&huart3, (uint8_t *)pcToSendStirng, strlen(pcToSendStirng), HAL_MAX_DELAY);
        xQueueSendToBack(xQueue, &pcToSendStirng, portMAX_DELAY);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}


void ReceiverTask(void *pvParameters) {
    char * pcReceivingString;
    while (1) {
        if (xQueueReceive(xQueue, &pcReceivingString, portMAX_DELAY) == pdPASS) {
            HAL_UART_Transmit(&huart3, (uint8_t *)pcReceivingString, strlen(pcReceivingString), HAL_MAX_DELAY);
            vPortFree(pcReceivingString); // Free the buffer after use
        } else {
            const char *errMsg = "Failed to receive from queue\r\n";
            HAL_UART_Transmit(&huart3, (uint8_t *)errMsg, strlen(errMsg), HAL_MAX_DELAY);
        }
    }
}

int main(int argc, char const *argv[])
{
    Driver_Init();
    /* code */
    xQueue = xQueueCreate(5, sizeof(char*));
    xTaskCreate(SenderTask, "Sender", 128, NULL, 1, NULL);
    xTaskCreate(ReceiverTask, "Receiver", 128, NULL, 2, NULL);
   
    vTaskStartScheduler();
   
    while(1);
    return 0;
}




