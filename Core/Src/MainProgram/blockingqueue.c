/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : blockingqueue.c
  * @brief          : 두개의 Task에서 각각 100과 200을 전송, 수신 테스크에서 큐로 데이터 처리 
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
            const char *errMsg = "Fail Send\r\n";
            HAL_UART_Transmit(&huart3, (uint8_t *)errMsg, strlen(errMsg), HAL_MAX_DELAY);
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
           // const char *msg = "Queue should have been empty!\r\n";
           // HAL_UART_Transmit( &huart3, ( uint8_t * ) msg, strlen( msg ), HAL_MAX_DELAY );
            
        } 
 
        
        xStatus = xQueueReceive( xQueue, &lReceivedValue, portMAX_DELAY ); 
 
        if( xStatus == pdPASS ) 
        { 
          
            char msg[64];
            sprintf( msg, "Receive d = %ld\r\n", ( long ) lReceivedValue );
            HAL_UART_Transmit( &huart3, ( uint8_t * ) msg, strlen( msg ), HAL_MAX_DELAY );
        } 
        else 
        { 
          
            const char* failMsg = "Failed to receive from queue\r\n";
            HAL_UART_Transmit( &huart3, ( uint8_t * ) failMsg, strlen( failMsg ), HAL_MAX_DELAY );
        } 
    } 
} 