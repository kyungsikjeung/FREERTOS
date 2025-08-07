#include "main.h"
#include "driver_init.h"
#include "adc_driver.h"

// 태스크 함수 프로토타입
/**
 * @brief ADC 실시간 모니터링 태스크
 * @param pvParameters 태스크에 전달할 파라미터 (사용하지 않음)
 */
void vADC_MonitorTask(void *pvParameters);

/**
 * @brief ADC 데이터 분석 및 처리 태스크
 * @param pvParameters 태스크에 전달할 파라미터 (사용하지 않음)
 */
void vADC_ProcessTask(void *pvParameters);

/**
 * @brief 메인 함수 - 시스템 초기화 및 FreeRTOS 시작
 * 
 * @return int (도달하지 않음)
 */
int main(void)
{
    // 하드웨어 초기화
    Driver_Init();

    // ADC 드라이버 초기화
    if (ADC_Driver_Init(&g_adc_driver) != HAL_OK) {
        printf(" ADC 드라이버 초기화 실패\r\n");
        while(1);
    }

    // ADC 모니터 태스크 생성
    xTaskCreate(
        vADC_MonitorTask,
        "ADC_Monitor",
        256,
        NULL,
        3,
        NULL
    );

    // ADC 데이터 처리 태스크 생성
    xTaskCreate(
        vADC_ProcessTask,
        "ADC_Process",
        256,
        NULL,
        2,
        NULL
    );

    // ADC 샘플링 시작
    if (ADC_Driver_Start(&g_adc_driver) != HAL_OK) {
        printf(" ADC 샘플링 시작 실패\r\n");
        while(1);
    }

    // 스케줄러 시작
    vTaskStartScheduler();

    // 여기에 도달하면 안됨
    while (1);
}

/**
 * @brief ADC 모니터링 태스크
 * 
 * ADC 드라이버로부터 데이터를 주기적으로 수신하여 UART로 출력한다.
 * 1초마다 큐 상태와 ADC 채널 값을 출력한다.
 * 
 * @param pvParameters 사용되지 않음
 */
void vADC_MonitorTask(void *pvParameters)
{
    ADC_Data_t adc_data;
    TickType_t last_wake_time = xTaskGetTickCount();

    printf("ADC 모니터링 태스크 시작\r\n");

    for (;;) {
        // 500ms 타임아웃으로 데이터 수신
        if (ADC_Driver_GetData(&g_adc_driver, &adc_data, pdMS_TO_TICKS(500)) == pdPASS) {
            
            printf(" [%lu] ADC 값: ", adc_data.timestamp);
            #ifdef LOG_ADC_VALUES
            for (int i = 0; i < ADC_CHANNEL_COUNT; i++) {
                printf("CH%d:%4u ", i, adc_data.channel_values[i]);
            }
            #endif

            // 큐 상태 표시
            uint32_t queue_count = ADC_Driver_GetQueueCount(&g_adc_driver);
            printf("(큐: %lu/%d)\r\n", queue_count, ADC_QUEUE_SIZE);
            
        } else {
            printf("  ADC 데이터 타임아웃\r\n");
        }

        // 1초 주기로 실행
        vTaskDelayUntil(&last_wake_time, pdMS_TO_TICKS(1000));
    }
}

/**
 * @brief ADC 데이터 처리 태스크
 * 
 * ADC 데이터를 수신하고 각 채널에 대해 통계값(평균, 최대, 최소)을 계산한다.
 * 100개의 샘플마다 통계를 UART로 출력한다.
 * 
 * @param pvParameters 사용되지 않음
 */
void vADC_ProcessTask(void *pvParameters)
{
    ADC_Data_t adc_data;
    uint32_t sample_count = 0;
    uint32_t sum[ADC_CHANNEL_COUNT] = {0};
    uint16_t max_values[ADC_CHANNEL_COUNT] = {0};
    uint16_t min_values[ADC_CHANNEL_COUNT] = {4095, 4095, 4095};

    printf("ADC 데이터 처리 태스크 시작\r\n");

    for (;;) {
        // 데이터 수신 (무한 대기)
        if (ADC_Driver_GetData(&g_adc_driver, &adc_data, portMAX_DELAY) == pdPASS) {
            sample_count++;

            // 채널별 통계 계산
            for (int i = 0; i < ADC_CHANNEL_COUNT; i++) {
                uint16_t value = adc_data.channel_values[i];

                sum[i] += value;
                if (value > max_values[i]) max_values[i] = value;
                if (value < min_values[i]) min_values[i] = value;
            }

            // 100개 샘플마다 통계 출력
            if (sample_count % 100 == 0) {
                printf("\r\n100샘플 통계 (샘플#%lu):\r\n", sample_count);

                for (int i = 0; i < ADC_CHANNEL_COUNT; i++) {
                    uint16_t avg = sum[i] / 100;

                    printf("  CH%d - 평균:%4u, 최대:%4u, 최소:%4u\r\n", 
                           i, avg, max_values[i], min_values[i]);

                    // 통계 리셋
                    sum[i] = 0;
                    max_values[i] = 0;
                    min_values[i] = 4095;
                }

                printf("\r\n");
            }
        }
    }
}
