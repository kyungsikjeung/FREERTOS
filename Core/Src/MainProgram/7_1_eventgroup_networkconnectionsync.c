#include "main.h"
#include "driver_init.h"

/*  EventGroup 비트 정의 */
#define WIFI_CONNECTED_BIT    (1 << 0)
#define SERVER_CONNECTED_BIT  (1 << 1)

/*  EventGroup 핸들 */
EventGroupHandle_t xEventGroup;

/*  Wi-Fi 연결 Task */
void vWifiConnectTask(void *pvParameters) {
    vTaskDelay(pdMS_TO_TICKS(1000));  // Wi-Fi 연결에 1초 소요되는 시뮬레이션
    printf("[Tick %lu]  Wi-Fi 연결 완료\n", xTaskGetTickCount());
    xEventGroupSetBits(xEventGroup, WIFI_CONNECTED_BIT);
    vTaskDelete(NULL);
}

/*  서버 연결 Task */
void vServerConnectTask(void *pvParameters) {
    vTaskDelay(pdMS_TO_TICKS(1500));  // 서버 연결에 1.5초 소요되는 시뮬레이션
    printf("[Tick %lu]  서버 연결 완료\n", xTaskGetTickCount());
    xEventGroupSetBits(xEventGroup, SERVER_CONNECTED_BIT);
    vTaskDelete(NULL);
}

/*  메인 통신 Task: 두 연결이 완료되면 데이터 전송 */
void vDataTask(void *pvParameters) {
    printf("[Tick %lu]  데이터 전송을 위해 연결 대기 중...\n", xTaskGetTickCount());

    xEventGroupWaitBits(
        xEventGroup,
        WIFI_CONNECTED_BIT | SERVER_CONNECTED_BIT,
        pdTRUE,     // 비트 자동 클리어
        pdTRUE,     // 모든 비트가 Set될 때까지 대기 (AND 조건)
        portMAX_DELAY
    );

    printf("[Tick %lu]  모든 연결 완료됨. 데이터 전송 시작!\n", xTaskGetTickCount());

    while (1) {
        printf("[Tick %lu]  주기적 데이터 전송 중...\n", xTaskGetTickCount());
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

int main(void) {
    //  하드웨어 및 드라이버 초기화
    Driver_Init();

    //  EventGroup 생성
    xEventGroup = xEventGroupCreate();

    //  Task 생성
    xTaskCreate(vWifiConnectTask,    "WiFi",   1000, NULL, 1, NULL);
    xTaskCreate(vServerConnectTask,  "Server", 1000, NULL, 1, NULL);
    xTaskCreate(vDataTask,           "Data",   1000, NULL, 2, NULL);

    //  스케줄러 시작
    vTaskStartScheduler();

    //  실패 시 무한 루프
    while (1);
    return 0;
}
