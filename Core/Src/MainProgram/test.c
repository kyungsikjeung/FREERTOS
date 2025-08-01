// FreeRTOS TCP/IP Event Handling Example - Full Code

#include "main.h"
#include "driver_init.h"
/* === Event Types === */
typedef enum {
    eNetworkDownEvent = 0,
    eNetworkRxEvent,
    eTCPAcceptEvent,
    eNoEvent
} eIPEvent_t;

/* === Event Structure === */
typedef struct {
    eIPEvent_t eEventType;
    void *pvData;
} IPStackEvent_t;

/* === Queue Handle === */
QueueHandle_t xNetworkEventQueue;

/* === Dummy Structs for Event Data === */
typedef struct {
    char data[100];
} NetworkBufferDescriptor_t;

typedef struct {
    int socket_id;
} FreeRTOS_Socket_t;

/* === Event Sender Functions === */
void vSendRxDataToTheTCPTask(NetworkBufferDescriptor_t *pxRxedData) {
    IPStackEvent_t xEventStruct;
    xEventStruct.eEventType = eNetworkRxEvent;
    xEventStruct.pvData = (void *)pxRxedData;
    xQueueSendToBack(xNetworkEventQueue, &xEventStruct, portMAX_DELAY);
}

void vSendAcceptRequestToTheTCPTask(FreeRTOS_Socket_t *xSocket) {
    IPStackEvent_t xEventStruct;
    xEventStruct.eEventType = eTCPAcceptEvent;
    xEventStruct.pvData = (void *)xSocket;
    xQueueSendToBack(xNetworkEventQueue, &xEventStruct, portMAX_DELAY);
}

void vSendNetworkDownEventToTheTCPTask() {
    IPStackEvent_t xEventStruct;
    xEventStruct.eEventType = eNetworkDownEvent;
    xEventStruct.pvData = NULL;
    xQueueSendToBack(xNetworkEventQueue, &xEventStruct, portMAX_DELAY);
}

/* === TCP/IP Task === */
void vTCPTask(void *pvParameters) {
    IPStackEvent_t xReceivedEvent;
    for (;;) {
        xReceivedEvent.eEventType = eNoEvent;
        if (xQueueReceive(xNetworkEventQueue, &xReceivedEvent, portMAX_DELAY) == pdPASS) {
            switch (xReceivedEvent.eEventType) {
                case eNetworkDownEvent:
                    char *msg = "Network down event received.\r\n";
                    HAL_UART_Transmit(&huart3, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);
                    
                    break;
                case eNetworkRxEvent:
                 char *msg;
                  snprintf(msg, sizeof(msg), "Received data: %s\r\n",
                           ((NetworkBufferDescriptor_t *)xReceivedEvent.pvData)->data);
                  HAL_UART_Transmit(&huart3, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);
                    break;
                case eTCPAcceptEvent:
                    printf("[TCP Task] Accepted socket id: %d\n",
                           ((FreeRTOS_Socket_t *)xReceivedEvent.pvData)->socket_id);
                    break;
                default:
                    printf("[TCP Task] Unknown event\n");
                    break;
            }
        }
    }
}

/* === Main === */
int main(void) {
    xNetworkEventQueue = xQueueCreate(10, sizeof(IPStackEvent_t));

    xTaskCreate(vTCPTask, "TCPTask", 512, NULL, 1, NULL);

    // Simulate events
    NetworkBufferDescriptor_t *rxBuffer = pvPortMalloc(sizeof(NetworkBufferDescriptor_t));
    strcpy(rxBuffer->data, "Hello from network!");
    vSendRxDataToTheTCPTask(rxBuffer);

    FreeRTOS_Socket_t *socket = pvPortMalloc(sizeof(FreeRTOS_Socket_t));
    socket->socket_id = 42;
    vSendAcceptRequestToTheTCPTask(socket);

    vSendNetworkDownEventToTheTCPTask();
    vTaskStartScheduler();

    while (1);
}
