#include "main.h"
#include "driver_init.h"


/* 
@objective : 인터럽트 발생 시, 현재 Task가 끝난 뒤에 실행되도록 지연된 함수 호출을 사용하여 인터럽트 처리 
@description : vPeriodicTask() 실행 중
→ HAL_NVIC_SetPendingIRQ()로 강제 인터럽트 발생
EXTI15_10_IRQHandler() 실행
→ xTimerPendFunctionCallFromISR() 호출
→ 요청만 큐에 넣고 실행은 안 함
→ portYIELD_FROM_ISR()로 컨텍스트 스위칭 요청
vPeriodicTask()는 vTaskDelay()로 Block 상태로 진입함
→ 스케줄러가 우선순위 높은 Task가 있나 확인
이제 우선순위가 더 높은 Timer Service Task가 Ready 상태이므로
→ Context Switch 발생 → Timer Task가 실행됨
Timer Task가 큐에 등록된 vDeferredHandlingFunction()을 그제서야 실행함
*/


void vPeriodicTask(void *pvParameters);
static void vDeferredHandlingFunction(void *pvParam1, uint32_t ulParam2);
static uint32_t ulParamCounter = 0;

void EXTI15_10_IRQHandler(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    // Timer Task 우선순위 ≤ 현재 Task - 현재 Task 끝난 뒤 실행됨 (지연됨)
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
    printf("인터럽트 처리 위임됨: Param = %lu\r\n", ulParam2);
}

/* 500ms 마다 인터럽트 소프트웨어 발생 */
void vPeriodicTask(void *pvParameters)
{
    while (1)
    {
        printf("Periodic Task 실행 중...\r\n");
        HAL_NVIC_SetPendingIRQ(EXTI15_10_IRQn); // 인터럽트 발생
        vTaskDelay(pdMS_TO_TICKS(500)); // 500ms 대기
    }
}

int main(void)
{
    Driver_Init();
    // 인터럽트 Enable
    HAL_NVIC_SetPriority(EXTI15_10_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn); 
    // 인터럽트 발생 Task 생성
    xTaskCreate(vPeriodicTask, "Periodic", 512, NULL, (UBaseType_t)(configTIMER_TASK_PRIORITY - 1), NULL);
    vTaskStartScheduler();
    while (1);
}
