#include "main.h"
#include "driver_init.h"
#include <string.h>
#include <stdio.h>

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
    
    // 큐가 가득 찰 경우를 대비해 타임아웃 설정
    if (xQueueSendToBack(xNetworkEventQueue, &xEventStruct, pdMS_TO_TICKS(100)) == pdPASS) {
        const char msg[] = "[SEND] Rx Event Sent\r\n";
        HAL_UART_Transmit(&huart3, (uint8_t *)msg, strlen(msg), 1000);
    } else {
        const char errorMsg[] = "[ERROR] Failed to send Rx Event\r\n";
        HAL_UART_Transmit(&huart3, (uint8_t *)errorMsg, strlen(errorMsg), 1000);
        vPortFree(pxRxedData); // 실패 시 메모리 해제
    }
}

void vSendAcceptRequestToTheTCPTask(FreeRTOS_Socket_t *xSocket) {
    IPStackEvent_t xEventStruct;
    xEventStruct.eEventType = eTCPAcceptEvent;
    xEventStruct.pvData = (void *)xSocket;
    
    if (xQueueSendToBack(xNetworkEventQueue, &xEventStruct, pdMS_TO_TICKS(100)) == pdPASS) {
        const char msg[] = "[SEND] Accept Event Sent\r\n";
        HAL_UART_Transmit(&huart3, (uint8_t *)msg, strlen(msg), 1000);
    } else {
        const char errorMsg[] = "[ERROR] Failed to send Accept Event\r\n";
        HAL_UART_Transmit(&huart3, (uint8_t *)errorMsg, strlen(errorMsg), 1000);
        vPortFree(xSocket); // 실패 시 메모리 해제
    }
}

void vSendNetworkDownEventToTheTCPTask() {
    IPStackEvent_t xEventStruct;
    xEventStruct.eEventType = eNetworkDownEvent;
    xEventStruct.pvData = NULL;
    
    if (xQueueSendToBack(xNetworkEventQueue, &xEventStruct, pdMS_TO_TICKS(100)) == pdPASS) {
        const char msg[] = "[SEND] NetworkDown Event Sent\r\n";
        HAL_UART_Transmit(&huart3, (uint8_t *)msg, strlen(msg), 1000);
    } else {
        const char errorMsg[] = "[ERROR] Failed to send NetworkDown Event\r\n";
        HAL_UART_Transmit(&huart3, (uint8_t *)errorMsg, strlen(errorMsg), 1000);
    }
}

/* === TCP/IP Task === */
void vTCPTask(void *pvParameters) {
    IPStackEvent_t xReceivedEvent;
    char msgBuffer[128];

    const char startMsg[] = "[TCP Task] Task started\r\n";
    HAL_UART_Transmit(&huart3, (uint8_t *)startMsg, strlen(startMsg), 1000);

    for (;;) {
        // 큐 상태 확인 (디버깅용)
        UBaseType_t waiting = uxQueueMessagesWaiting(xNetworkEventQueue);
        snprintf(msgBuffer, sizeof(msgBuffer), "[TCP Task] Queue waiting: %lu\r\n", waiting);
        HAL_UART_Transmit(&huart3, (uint8_t *)msgBuffer, strlen(msgBuffer), 1000);

        // 이벤트 수신 대기 (1초 타임아웃)
        if (xQueueReceive(xNetworkEventQueue, &xReceivedEvent, pdMS_TO_TICKS(1000)) == pdPASS) {
            snprintf(msgBuffer, sizeof(msgBuffer), "[TCP Task] Event received: %d\r\n", xReceivedEvent.eEventType);
            HAL_UART_Transmit(&huart3, (uint8_t *)msgBuffer, strlen(msgBuffer), 1000);

            // 이벤트 타입별 처리
            switch (xReceivedEvent.eEventType) {
                case eNetworkDownEvent: {
                    const char msg[] = "Network down event received.\r\n";
                    HAL_UART_Transmit(&huart3, (uint8_t *)msg, strlen(msg), 1000);
                    // pvData가 NULL이므로 해제할 필요 없음
                    break;
                }
                case eNetworkRxEvent: {
                    NetworkBufferDescriptor_t *pxBuffer = (NetworkBufferDescriptor_t *)xReceivedEvent.pvData;
                    if (pxBuffer != NULL) {
                        snprintf(msgBuffer, sizeof(msgBuffer), "Received data: %s\r\n", pxBuffer->data);
                        HAL_UART_Transmit(&huart3, (uint8_t *)msgBuffer, strlen(msgBuffer), 1000);
                        vPortFree(pxBuffer); // ⭐️ 메모리 해제 필수
                    }
                    break;
                }
                case eTCPAcceptEvent: {
                    FreeRTOS_Socket_t *pxSocket = (FreeRTOS_Socket_t *)xReceivedEvent.pvData;
                    if (pxSocket != NULL) {
                        snprintf(msgBuffer, sizeof(msgBuffer), "Accepted socket id: %d\r\n", pxSocket->socket_id);
                        HAL_UART_Transmit(&huart3, (uint8_t *)msgBuffer, strlen(msgBuffer), 1000);
                        vPortFree(pxSocket); // ⭐️ 메모리 해제 필수
                    }
                    break;
                }
                default: {
                    const char msg[] = "Unknown event received.\r\n";
                    HAL_UART_Transmit(&huart3, (uint8_t *)msg, strlen(msg), 1000);
                    // 혹시 pvData가 있다면 타입을 알 수 없으므로 해제하지 않음
                    break;
                }
            }
        } else {
            // 타임아웃 발생 (1초간 이벤트 없음) - 정상적인 상황
        }
    }
}

/* === Simulator Task (Sends Events) === */
void vEventSimulatorTask(void *pvParameters) {
    // 충분한 초기화 시간 대기
    vTaskDelay(pdMS_TO_TICKS(2000));

    const char msg[] = "[SIM] Sending simulated events...\r\n";
    HAL_UART_Transmit(&huart3, (uint8_t *)msg, strlen(msg), 1000);

    // ⭐️ 1. 네트워크 RX 이벤트 전송
    NetworkBufferDescriptor_t *rxBuffer = pvPortMalloc(sizeof(NetworkBufferDescriptor_t));
    if (rxBuffer != NULL) {
        strcpy(rxBuffer->data, "Hello from network!");
        vSendRxDataToTheTCPTask(rxBuffer);
    }

    // 이벤트 간 간격 추가 (순서 보장)
    vTaskDelay(pdMS_TO_TICKS(100));

    // ⭐️ 2. TCP Accept 이벤트 전송
    FreeRTOS_Socket_t *socket = pvPortMalloc(sizeof(FreeRTOS_Socket_t));
    if (socket != NULL) {
        socket->socket_id = 42;
        vSendAcceptRequestToTheTCPTask(socket);
    }

    // 이벤트 간 간격 추가 (순서 보장)
    vTaskDelay(pdMS_TO_TICKS(100));

    // ⭐️ 3. 네트워크 다운 이벤트 전송
    vSendNetworkDownEventToTheTCPTask();

    // 모든 이벤트 전송 완료
    const char completeMsg[] = "[SIM] All events sent, task terminating\r\n";
    HAL_UART_Transmit(&huart3, (uint8_t *)completeMsg, strlen(completeMsg), 1000);

    // 태스크 종료
    vTaskDelete(NULL);
}

int main(void) {
    // 1단계: 기본 하드웨어 초기화
    Driver_Init();

    // 2단계: 시작 메시지
    const char startMsg[] = "[INFO] Event Handling Queue Demo Started\r\n";
    HAL_UART_Transmit(&huart3, (uint8_t *)startMsg, strlen(startMsg), 1000);

    // 3단계: 메시지 전송할 큐 생성
    xNetworkEventQueue = xQueueCreate(10, sizeof(IPStackEvent_t));
    configASSERT(xNetworkEventQueue != NULL);

    // 4단계: 태스크 생성
    // ⭐️ TCP Task를 더 높은 우선순위로 설정하여 이벤트 처리 우선
    BaseType_t result1 = xTaskCreate(vTCPTask, "TCPTask", 1024, NULL, 4, NULL);  // 우선순위 4
    BaseType_t result2 = xTaskCreate(vEventSimulatorTask, "SimTask", 1024, NULL, 2, NULL);  // 우선순위 2

    configASSERT(result1 == pdPASS);
    configASSERT(result2 == pdPASS);

    // 5단계: 스케줄러 시작 알림
    const char schedMsg[] = "[INFO] Starting FreeRTOS Scheduler...\r\n";
    HAL_UART_Transmit(&huart3, (uint8_t *)schedMsg, strlen(schedMsg), 1000);

    // 6단계: 스케줄러 시작
    vTaskStartScheduler();

    // 여기까지 도달하면 안됨 (스케줄러 시작 실패)
    const char errorMsg[] = "[ERROR] Scheduler failed to start!\r\n";
    HAL_UART_Transmit(&huart3, (uint8_t *)errorMsg, strlen(errorMsg), 1000);
    
    while(1) {
        // 무한 루프 (에러 상태)
    }
}