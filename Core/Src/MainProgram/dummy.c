#include "main.h"
#include "driver_init.h"


void EXTI15_10_IRQnHandler(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    // 소프트웨어 인터럽트 핸들러 호출
    xTimerPendFunctionCallFromISR(
        vDeferredHandlingFunction, // 실행할 함수
        NULL,                      // 파라미터 1 (사용 안 함)
        ulParamCounter++,          // 파라미터 2 (카운터 증가)
        &xHigherPriorityTaskWoken  // 우선순위 전환 요청
    );

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_13); // 예: 버튼 인터럽트 클리어
}



void vPeriodicTask(void *pvParameters){
    while(1){
        HAL_NVIC_SetPendingIRQ(EXTI15_10_IRQn); // 소프트웨어 인터럽트 발생
        vTaskDelay(pdMS_TO_TICKS(500)); // 1초 대기
    }   
}



int main(void) {
    // Hardware Initialization
    Driver_Init();

    xTaskCreate( vPeriodicTask, "Task", configMINIMAL_STACK_SIZE + 1024, NULL, 1, NULL);
    vTaskStartScheduler();

    // Loop forever if scheduler fails
    while(1);
    return 0;
}