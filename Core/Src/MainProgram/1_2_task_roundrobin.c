#include "main.h"
#include "driver_init.h"

// 전역 문자열로 선언 (메모리에 계속 유지됨)
const char* task1_msg = "Task1 Msg  !\r\n";
const char* task2_msg = "Task2 Msg  !\r\n";

 // HIGHLIGHT: range 25-47
 // ! TASK DECLARATION, 
void Task1(void *pvParameters) {
    const char* msg = (const char*) pvParameters;
    while (1) {
        // Do something
        HAL_UART_Transmit(&huart3, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);
        HAL_GPIO_TogglePin(GPIOB, LD1_Pin);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}


void Task2(void *pvParameters) {
    const char* msg = (const char*) pvParameters;

    while (1) {
        // Do something
        HAL_UART_Transmit(&huart3, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);
        HAL_GPIO_TogglePin(GPIOB, LD2_Pin);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}


int main(void)
{
  Driver_Init();
  TaskHandle_t xTask1Handle = NULL;
  TaskHandle_t xTask2Handle = NULL;
  xTaskCreate(Task1, "Task1", 256, (void*)task1_msg, 1, &xTask1Handle);
  xTaskCreate(Task2, "Task2", 256, (void*)task2_msg, 1, &xTask2Handle);
  vTaskStartScheduler();
  
  while (1)
  {
   
  }  
}





