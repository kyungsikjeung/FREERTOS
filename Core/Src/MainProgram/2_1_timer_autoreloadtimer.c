#include "driver_init.h"
#include "main.h"

static void prvOneShotTimerCallback(TimerHandle_t xTimer);
static void prvAutoReloadTimerCallback(TimerHandle_t xTimer);

static uint32_t ulCallCount = 0;

int main() {
    Driver_Init();

    BaseType_t xTimer1Started, xTimer2Started;
    TimerHandle_t xOneShotTimer;
    TimerHandle_t xAutoReloadTimer;
    
    xOneShotTimer = xTimerCreate("OneShotTimer", pdMS_TO_TICKS(7600), pdFALSE, (void *)0, prvOneShotTimerCallback);
    xAutoReloadTimer = xTimerCreate("AutoReloadTimer", pdMS_TO_TICKS(5000), pdTRUE, (void *)0, prvAutoReloadTimerCallback);

    if ((xOneShotTimer != NULL) && (xAutoReloadTimer != NULL)) {
        xTimer1Started = xTimerStart(xOneShotTimer, 0);
        xTimer2Started = xTimerStart(xAutoReloadTimer, 0);

        if ((xTimer1Started == pdPASS) && (xTimer2Started == pdPASS)) {
            vTaskStartScheduler();
        }
    }

    while (1);  // Should never reach here
}

/* One-shot 타이머 콜백 */
static void prvOneShotTimerCallback(TimerHandle_t xTimer) {
    TickType_t xTimeNow = xTaskGetTickCount();
    printf("One-shot timer callback executed at tick: %lu\r\n", (unsigned long)xTimeNow);
    ulCallCount++;
}

/* Auto-reload 타이머 콜백 */
static void prvAutoReloadTimerCallback(TimerHandle_t xTimer) {
    TickType_t xTimeNow = xTaskGetTickCount();
    printf("Auto-reload timer callback executed at tick: %lu\r\n", (unsigned long)xTimeNow);
    ulCallCount++;
}
