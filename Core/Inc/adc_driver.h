#ifndef __ADC_DRIVER_H__
#define __ADC_DRIVER_H__

#include "stm32f4xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "timers.h"

/// @name ADC 채널 사용 설정
/// @{
#define USE_CH0    1  /**< CH0 사용 여부 */
#define USE_CH1    1  /**< CH1 사용 여부 */
#define USE_CH2    1  /**< CH2 사용 여부 */
#define USE_CH3    0  /**< CH3 사용 여부 */
#define USE_CH4    0  /**< CH4 사용 여부 */
/// @}

/// @brief 사용 채널 수 자동 계산
#define ADC_CHANNEL_COUNT (USE_CH0 + USE_CH1 + USE_CH2 + USE_CH3 + USE_CH4)

/// @name ADC 설정 값
/// @{
#define ADC_SAMPLING_PERIOD_MS    100    /**< 샘플링 주기 (밀리초 단위) */
#define ADC_QUEUE_SIZE            10     /**< ADC 데이터 큐 크기 */
#define ADC_TASK_STACK_SIZE       256    /**< ADC 태스크 스택 크기 */
#define ADC_TASK_PRIORITY         2      /**< ADC 태스크 우선순위 */
/// @}

/**
 * @brief ADC로 측정된 데이터를 담는 구조체
 */
typedef struct {
    uint16_t channel_values[ADC_CHANNEL_COUNT]; /**< 각 채널의 ADC 값 배열 */
    uint32_t timestamp;                         /**< 타임스탬프 (ms 기준) */
    uint8_t channel_count;                      /**< 사용 중인 채널 수 */
} ADC_Data_t;

/**
 * @brief ADC 드라이버 관리 구조체
 */
typedef struct {
    ADC_HandleTypeDef hadc;             /**< HAL ADC 핸들 */
    DMA_HandleTypeDef hdma;             /**< HAL DMA 핸들 */
    uint16_t raw_buffer[ADC_CHANNEL_COUNT]; /**< DMA 수신용 버퍼 */
    TimerHandle_t sampling_timer;       /**< 주기적 샘플링용 FreeRTOS 타이머 */
    QueueHandle_t data_queue;           /**< ADC 결과 전송용 큐 */
    SemaphoreHandle_t data_mutex;       /**< 데이터 보호용 뮤텍스 */
    //volatile uint8_t conversion_complete; /**< 변환 완료 플래그 (ISR에서 사용) */
    TaskHandle_t adc_task_handle;       /**< ADC 처리용 태스크 핸들 */
    uint8_t channel_count;              /**< 사용 중인 ADC 채널 수 */
} ADC_Driver_t;

/// @brief 전역 ADC 드라이버 인스턴스
extern ADC_Driver_t g_adc_driver;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief ADC 드라이버 초기화
 * @param driver ADC 드라이버 구조체 포인터
 * @return HAL_OK 또는 HAL_ERROR
 */
HAL_StatusTypeDef ADC_Driver_Init(ADC_Driver_t *driver);

/**
 * @brief ADC 측정 시작 (DMA 방식)
 * @param driver ADC 드라이버 구조체 포인터
 * @return HAL_OK 또는 HAL_ERROR
 */
HAL_StatusTypeDef ADC_Driver_Start(ADC_Driver_t *driver);

/**
 * @brief ADC 측정 중지
 * @param driver ADC 드라이버 구조체 포인터
 * @return HAL_OK 또는 HAL_ERROR
 */
HAL_StatusTypeDef ADC_Driver_Stop(ADC_Driver_t *driver);

/**
 * @brief ADC 큐에서 데이터 수신
 * @param driver ADC 드라이버 구조체 포인터
 * @param data 수신한 ADC 데이터 구조체 포인터
 * @param timeout 큐 대기 시간 (Tick 단위)
 * @return pdTRUE(성공) 또는 pdFALSE(타임아웃)
 */
BaseType_t ADC_Driver_GetData(ADC_Driver_t *driver, ADC_Data_t *data, TickType_t timeout);

/**
 * @brief ADC 큐에 쌓인 데이터 개수 반환
 * @param driver ADC 드라이버 구조체 포인터
 * @return 큐 내 항목 개수
 */
uint32_t ADC_Driver_GetQueueCount(ADC_Driver_t *driver);

/**
 * @brief ADC 큐를 비움
 * @param driver ADC 드라이버 구조체 포인터
 */
void ADC_Driver_ClearQueue(ADC_Driver_t *driver);

/**
 * @brief ADC 측정을 수행하는 FreeRTOS 태스크
 * @param pvParameters 태스크 파라미터 (사용 안함)
 */
void ADC_Task(void *pvParameters);

/**
 * @brief DMA 인터럽트 핸들러 (DMA2_Stream0용)
 */
void DMA2_Stream0_IRQHandler(void);

/**
 * @brief ADC 인터럽트 핸들러 (옵션)
 */
void ADC_IRQHandler(void);

#ifdef __cplusplus
}
#endif

#endif /* __ADC_DRIVER_H__ */
