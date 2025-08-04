#include "main.h"
#include "driver_init.h"

TimerHandle_t xTimer;


void customFunction(TimerHandle_t xTimer) {
    
    static int cnt = 0;
    if(cnt != 5){
        printf("1 Sec\r\n");
    }
    else if( cnt == 5 ){
        printf("2 Sec\r\n");
        xTimerChangePeriod(xTimer, pdMS_TO_TICKS(2000), 0); // 주기 변경
    }
    cnt++;

}


void vInitTask(void *pvParameters) {
    xTimer = xTimerCreate("Timer1", pdMS_TO_TICKS(1000), 1, 0, customFunction);
    if (xTimer != NULL) {
        xTimerStart(xTimer, 0); // 타이머 시작
    }
    vTaskDelete(NULL); //  초기화 이후 자신의 테스크 삭제
}


int main(){
    Driver_Init();
    xTaskCreate(vInitTask, "Init", 256, NULL, 1, NULL);
    vTaskStartScheduler();

    return 0;
}