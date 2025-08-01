#include "main.h"
#include "driver_init.h"


QueueHandle_t xQueue;

// HIGHLIGHT: range 7-33
typedef enum {
    SOURCE_HMI,
    SOURCE_CAN,
    SOURCE_ANOTHER
} DataSource_t;





typedef struct{
    DataSource_t source;
    uint32_t uxValue;
}sendData_t;


sendData_t dataToSend[3] = {{SOURCE_HMI, 100},
                            {SOURCE_CAN, 200},
                            {SOURCE_ANOTHER, 300}};










static void sendTask1(void *pvParameters) {
     sendData_t sendDataPointer = *(sendData_t* )pvParameters;
     sendDataPointer.uxValue = 111;
    while (1) {
        xQueueSendToBack(xQueue, &sendDataPointer, portMAX_DELAY);
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

static void sendTask2(void *pvParameters) {
     sendData_t sendDataPointer = *(sendData_t* )pvParameters;
     sendDataPointer.uxValue = 222;
    while (1) {
        xQueueSendToBack(xQueue, &sendDataPointer, portMAX_DELAY);
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

static void sendTask3(void *pvParameters) {
     sendData_t sendDataPointer = *(sendData_t* )pvParameters;
     sendDataPointer.uxValue = 333;
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
            // Handle received data, print to UART
            char msg[64];
            sprintf(msg, "Received from source %d: value = %lu\r\n", receivedData.source, receivedData.uxValue);
            HAL_UART_Transmit(&huart3, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);
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






