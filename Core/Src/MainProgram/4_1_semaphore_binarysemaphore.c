#include "main.h"
#include "driver_init.h"

SemaphoreHandle_t binSem;

void EXTI15_10_IRQHandler(void)
{
    if(__HAL_GPIO_EXTI_GET_IT(USER_Btn_Pin) != RESET){
        __HAL_GPIO_EXTI_CLEAR_IT(USER_Btn_Pin);
        xSemaphoreGiveFromISR(binSem, NULL);
        portYIELD_FROM_ISR(pdTRUE);
    }
}

void Interrupts_Init(){
    HAL_NVIC_SetPriority(EXTI15_10_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}

void EventProcessTask(){
    while(1){
        if(xSemaphoreTake(binSem, portMAX_DELAY) == pdTRUE){
            // Process the event triggered by the button press
            printf("Button Pressed!\n");
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

int main()
{
    Driver_Init();
    Interrupts_Init();
    binSem = xSemaphoreCreateBinary();
    // countsem = xSemaphoreCreateCounting(10, 0); - z카운트 세마포어 예시 , 10 - 최대 카운트, 0 - 초기 카운트
    if (binSem != NULL) {
        xTaskCreate(EventProcessTask, "EventProcessTask", 128, NULL, 1, NULL);
        vTaskStartScheduler();
    } else {
        
    }
    

    while (1)
    
    {
    }
    return 0;
}
