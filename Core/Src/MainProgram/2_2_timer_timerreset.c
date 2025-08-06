#include "main.h"
#include "driver_init.h"
#include <stdbool.h>
#include <stdio.h>

/* 타이머 핸들 */
TimerHandle_t oneShotTimerHandle = NULL;

/* 버튼 눌림 상태 */
volatile bool isButtonPushed = false;

/* 백라이트 OFF 콜백 (5초 후 호출됨) */
void resetFlag(TimerHandle_t xTimer) {
    printf("reset Flag\r\n");
    isButtonPushed = false;

    HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);
}

/* 백라이트 ON 처리 */
void turnOnBackLight() {
    HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_SET);
}

/* 버튼 감지 태스크 */
void detectKeyInput(void *pvParameters) {
    BaseType_t timerActive;
    GPIO_PinState prevState = GPIO_PIN_RESET;

    while (1) {
        GPIO_PinState pinState = HAL_GPIO_ReadPin(USER_Btn_GPIO_Port, USER_Btn_Pin);

        // Rising edge 감지: 이전 상태는 LOW, 현재는 HIGH
        if (pinState == GPIO_PIN_SET && prevState == GPIO_PIN_RESET) {
            // 버튼이 눌렸고 이전엔 안 눌렸음 → 이벤트 발생
            turnOnBackLight();
            isButtonPushed = true;

            // 타이머 상태 확인 후 리셋 또는 시작
            timerActive = xTimerIsTimerActive(oneShotTimerHandle);
            if (timerActive == pdFALSE) {
                xTimerStart(oneShotTimerHandle, 0);
            } else {
                xTimerReset(oneShotTimerHandle, 0);
            }
        }

        prevState = pinState;
        vTaskDelay(pdMS_TO_TICKS(50));  // 디바운싱 및 폴링 간격
    }
}

/* 메인 함수 */
int main(void) {
    // 기본 초기화
    Driver_Init();

    // 5초짜리 one-shot 타이머 생성
    oneShotTimerHandle = xTimerCreate("OneShotTimer",
                                      pdMS_TO_TICKS(5000),
                                      pdFALSE,   // One-shot
                                      NULL,
                                      resetFlag);
    if (oneShotTimerHandle != NULL) {
        xTimerStart(oneShotTimerHandle, 0);  // 초기에는 바로 시작 (OFF 위한 준비)
    }

    // 버튼 감지 태스크 생성
    xTaskCreate(detectKeyInput, "DetectKeyInput", 256, NULL, 1, NULL);

    // FreeRTOS 스케줄러 시작
    vTaskStartScheduler();

    // 실행 도달 불가
    while (1) {}

    return 0;
}
