#include "main.h"
#include "driver_init.h"

/* 타이머 핸들 */
static TimerHandle_t xOneShotTimer = NULL;
static TimerHandle_t xAutoReloadTimer = NULL;

/* 문자열 출력 함수 (예: UART 대신 printf 사용) */
void vPrintStringAndNumber(const char *msg, TickType_t tick) {
    printf("%s at tick %lu\n", msg, (unsigned long)tick);
}

/* 타이머 콜백 함수: 두 타이머에서 공통 사용 */
static void prvTimerCallback(TimerHandle_t xTimer)
{
    TickType_t xTimeNow;
    uint32_t ulExecutionCount;

    // 1. 현재 ID 값(실행 횟수)을 가져와서 1 증가
    ulExecutionCount = (uint32_t) pvTimerGetTimerID(xTimer);
    ulExecutionCount++;

    // 2. 새로운 실행 횟수로 ID 업데이트
    vTimerSetTimerID(xTimer, (void *) ulExecutionCount);

    // 3. 현재 tick 획득
    xTimeNow = xTaskGetTickCount();

    // 4. 어떤 타이머인지 구분 후 메시지 출력
    if (xTimer == xOneShotTimer)
    {
        vPrintStringAndNumber("One-shot timer callback executing", xTimeNow);
    }
    else
    {
        vPrintStringAndNumber("Auto-reload timer callback executing", xTimeNow);

        // 5. 오토리로드 타이머는 5번 실행 후 정지
        if (ulExecutionCount == 5)
        {
            xTimerStop(xTimer, 0);  // 블로킹 없이 정지
        }
    }
}

/* 시작 태스크: 타이머 생성 및 시작 */
void vStarterTask(void *pvParameters)
{
    // 1. One-shot 타이머 생성 (1초 후 단 1회 실행)
    xOneShotTimer = xTimerCreate(
        "OneShot",                // 이름
        pdMS_TO_TICKS(1000),      // 기간: 1000ms
        pdFALSE,                  // 자동 재시작 여부 (false = one-shot)
        (void *)0,                // 초기 실행 횟수
        prvTimerCallback          // 콜백 함수
    );

    // 2. Auto-reload 타이머 생성 (0.5초 간격 반복 실행)
    xAutoReloadTimer = xTimerCreate(
        "AutoReload",             // 이름
        pdMS_TO_TICKS(500),       // 기간: 500ms
        pdTRUE,                   // 자동 재시작 여부
        (void *)0,                // 초기 실행 횟수
        prvTimerCallback          // 콜백 함수
    );

    // 3. 타이머 시작
    xTimerStart(xOneShotTimer, 0);
    xTimerStart(xAutoReloadTimer, 0);

    // 4. 자신은 종료
    vTaskDelete(NULL);
}

/* 메인 함수 */
int main(void)
{
    Driver_Init();
    // 시작 태스크 생성
    xTaskCreate(vStarterTask, "Starter", configMINIMAL_STACK_SIZE + 100, NULL, 1, NULL);

    // 스케줄러 시작
    vTaskStartScheduler();

    // 실행 도달 불가
    for (;;);
}