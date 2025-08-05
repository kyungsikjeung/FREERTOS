#include "main.h"
#include "driver_init.h"

void vPeriodicTask(void *pvParameters);
static void vDeferredHandlingFunction(void *pvParam1, uint32_t ulParam2);
static uint32_t ulParamCounter = 0;

void EXTI15_10_IRQHandler(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    xTimerPendFunctionCallFromISR(
        vDeferredHandlingFunction, // 실행할 함수
        NULL,                      // 파라미터 1 (사용 안 함)
        ulParamCounter++,          // 파라미터 2 (카운터 증가)
        &xHigherPriorityTaskWoken  // 우선순위 전환 요청
    );

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_13); // 예: 버튼 인터럽트 클리어
}

static void vDeferredHandlingFunction(void *pvParam1, uint32_t ulParam2)
{
    printf("🟢 인터럽트 처리 위임됨: Param = %lu\r\n", ulParam2);
}

void vPeriodicTask(void *pvParameters)
{
    
    while (1)
    {
        printf("🔁 Periodic Task 실행 중...\r\n");
        HAL_NVIC_SetPendingIRQ(EXTI15_10_IRQn); // 인터럽트 발생
        vTaskDelay(pdMS_TO_TICKS(500)); // 500ms 대기
    }
}

int main(void)
{
    Driver_Init();
    HAL_NVIC_SetPriority(EXTI15_10_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);  // 💡 반드시 있어야 함!    

    const UBaseType_t ulPeriodicTaskPriority = configTIMER_TASK_PRIORITY - 1;

    xTaskCreate(vPeriodicTask, "Periodic", 512, NULL, ulPeriodicTaskPriority, NULL);

    vTaskStartScheduler();

    while (1);
}
