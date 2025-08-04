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
QueueHandle_t xQueue;

static void sendData(void *pvParameters);
static void receiveData(void *pvParameters);

int main( void ) 
{ 

    Driver_Init();
    xQueue = xQueueCreate( 5, sizeof( int32_t ) ); 
    if( xQueue != NULL ) 
    { xTaskCreate( sendData, "Sender1", 1000, ( void * ) 100, 1, NULL ); 
      xTaskCreate( sendData, "Sender2", 1000, ( void * ) 200, 2, NULL ); 
      xTaskCreate( receiveData, "Receiver", 1000, NULL, 3, NULL ); 
      vTaskStartScheduler(); 
    } 
    else 
    { 
    
    } 
    for( ;; ); 
} 

// 송신 태스크 함수
static void sendData(void *pvParams)
{
    int32_t valueToSend = (int32_t )pvParams;
    BaseType_t xQueueStatus;

    for (;;)
    {
        // 큐에 데이터 전송 (최대 300ms 대기)
        xQueueStatus = xQueueSendToBack(xQueue, &valueToSend, 0 );
        if (xQueueStatus != pdPASS)
        {
            printf("Fail to send %ld to queue\r\n", (long)valueToSend);
        }
        vTaskDelay(pdMS_TO_TICKS(200)); // 200ms 대기
    }
}



static void receiveData( void *pvParameters ) 
{ 
    /* Declare the variable that will hold the values received from the queue. */ 
    int32_t lReceivedValue; 
    BaseType_t xStatus; 
    const TickType_t xTicksToWait = pdMS_TO_TICKS( 300 ); 
 
    /* This task is also defined within an infinite loop. */ 
    for( ;; ) 
    { 
        /* This call should always find the queue empty because this task will 
        immediately remove any data that is written to the queue. */ 
        if( uxQueueMessagesWaiting( xQueue ) != 0 ) 
        { 
            printf("Queue should have been empty!\r\n");
        } 
 
        
        xStatus = xQueueReceive( xQueue, &lReceivedValue, portMAX_DELAY ); 
 
        if( xStatus == pdPASS ) 
        { 
            printf("Received value: %ld\r\n", (long)lReceivedValue);
        } 
        else 
        { 
            printf("Failed to receive from queue\r\n");
        } 
    } 
} 