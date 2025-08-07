#include "adc_driver.h"
#include <stdio.h>

// 전역 ADC 드라이버 인스턴스
ADC_Driver_t g_adc_driver = {0};

// 내부 함수 프로토타입
static void ADC_Timer_Callback(TimerHandle_t xTimer);

/**
 * @brief ADC 드라이버 초기화
 * @param driver ADC 드라이버 구조체 포인터
 * @retval HAL_OK 또는 오류 코드
 */
HAL_StatusTypeDef ADC_Driver_Init(ADC_Driver_t *driver)
{
    HAL_StatusTypeDef status = HAL_OK;

    // 주변 장치 클럭 활성화
    __HAL_RCC_ADC1_CLK_ENABLE();
    __HAL_RCC_DMA2_CLK_ENABLE();

    // ADC 설정
    driver->hadc.Instance = ADC1;
    driver->hadc.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
    driver->hadc.Init.Resolution = ADC_RESOLUTION_12B;
    driver->hadc.Init.ScanConvMode = ENABLE;
    driver->hadc.Init.ContinuousConvMode = DISABLE;
    driver->hadc.Init.DiscontinuousConvMode = DISABLE;
    driver->hadc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    driver->hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    driver->hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    driver->hadc.Init.NbrOfConversion = ADC_CHANNEL_COUNT;

    status = HAL_ADC_Init(&driver->hadc);
    if (status != HAL_OK) return status;

    // DMA 설정
    driver->hdma.Instance = DMA2_Stream0;
    driver->hdma.Init.Channel = DMA_CHANNEL_0;
    driver->hdma.Init.Direction = DMA_PERIPH_TO_MEMORY;
    driver->hdma.Init.PeriphInc = DMA_PINC_DISABLE;
    driver->hdma.Init.MemInc = DMA_MINC_ENABLE;
    driver->hdma.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    driver->hdma.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    driver->hdma.Init.Mode = DMA_NORMAL;
    driver->hdma.Init.Priority = DMA_PRIORITY_HIGH;
    driver->hdma.Init.FIFOMode = DMA_FIFOMODE_DISABLE;


    
    status = HAL_DMA_Init(&driver->hdma);
    if (status != HAL_OK) return status;
    // DMA 와 ADC 연결
    __HAL_LINKDMA(&driver->hadc, DMA_Handle, driver->hdma);

    // 채널 설정
    ADC_ChannelConfTypeDef sConfig;
    sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
    uint8_t rank = 1;

#if USE_CH0
    sConfig.Channel = ADC_CHANNEL_0;
    sConfig.Rank = rank++;
    status = HAL_ADC_ConfigChannel(&driver->hadc, &sConfig);
    if (status != HAL_OK) return status;
#endif
#if USE_CH1
    sConfig.Channel = ADC_CHANNEL_1;
    sConfig.Rank = rank++;
    status = HAL_ADC_ConfigChannel(&driver->hadc, &sConfig);
    if (status != HAL_OK) return status;
#endif
#if USE_CH2
    sConfig.Channel = ADC_CHANNEL_2;
    sConfig.Rank = rank++;
    status = HAL_ADC_ConfigChannel(&driver->hadc, &sConfig);
    if (status != HAL_OK) return status;
#endif
#if USE_CH3
    sConfig.Channel = ADC_CHANNEL_3;
    sConfig.Rank = rank++;
    status = HAL_ADC_ConfigChannel(&driver->hadc, &sConfig);
    if (status != HAL_OK) return status;
#endif
#if USE_CH4
    sConfig.Channel = ADC_CHANNEL_4;
    sConfig.Rank = rank++;
    status = HAL_ADC_ConfigChannel(&driver->hadc, &sConfig);
    if (status != HAL_OK) return status;
#endif

    

    // ADC, DMA 인터럽트 우선순위 설정
    HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);
    HAL_NVIC_SetPriority(ADC_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(ADC_IRQn);

    // FreeRTOS 리소스 생성
    driver->data_queue = xQueueCreate(ADC_QUEUE_SIZE, sizeof(ADC_Data_t));
    driver->data_mutex = xSemaphoreCreateMutex();

    if (driver->data_queue == NULL || driver->data_mutex == NULL) {
        return HAL_ERROR;
    }

    // 샘플링 타이머 생성
    driver->sampling_timer = xTimerCreate(
        "ADC_Timer",
        pdMS_TO_TICKS(ADC_SAMPLING_PERIOD_MS),
        pdTRUE,
        driver, 
        ADC_Timer_Callback 
    );

    if (driver->sampling_timer == NULL) return HAL_ERROR;

    // ADC 태스크 생성
    BaseType_t result = xTaskCreate(
        ADC_Task,
        "ADC_Task",
        ADC_TASK_STACK_SIZE,
        driver,
        ADC_TASK_PRIORITY,
        &driver->adc_task_handle
    );

    if (result != pdPASS) return HAL_ERROR;

    //driver->conversion_complete = 0;
    return HAL_OK;
}

/**
 * @brief ADC 드라이버 시작 (타이머 시작)
 * @param driver ADC 드라이버 포인터
 * @retval HAL_OK 또는 HAL_ERROR
 */
HAL_StatusTypeDef ADC_Driver_Start(ADC_Driver_t *driver)
{
    if (driver->sampling_timer == NULL) return HAL_ERROR;
    if (xTimerStart(driver->sampling_timer, pdMS_TO_TICKS(100)) != pdPASS) return HAL_ERROR;
    return HAL_OK;
}

/**
 * @brief ADC 드라이버 정지
 * @param driver ADC 드라이버 포인터
 * @retval HAL_OK
 */
HAL_StatusTypeDef ADC_Driver_Stop(ADC_Driver_t *driver)
{
    if (driver->sampling_timer != NULL) {
        xTimerStop(driver->sampling_timer, pdMS_TO_TICKS(100));
    }
    HAL_ADC_Stop_DMA(&driver->hadc);
    return HAL_OK;
}

/**
 * @brief ADC 변환된 데이터 수신
 * @param driver ADC 드라이버 포인터
 * @param data 수신할 구조체 포인터
 * @param timeout 수신 대기 시간
 * @retval pdTRUE or pdFALSE
 */
BaseType_t ADC_Driver_GetData(ADC_Driver_t *driver, ADC_Data_t *data, TickType_t timeout)
{
    return xQueueReceive(driver->data_queue, data, timeout);
}

/**
 * @brief ADC 큐의 남은 메시지 개수 반환
 * @param driver ADC 드라이버 포인터
 * @retval 메시지 개수
 */
uint32_t ADC_Driver_GetQueueCount(ADC_Driver_t *driver)
{
    return uxQueueMessagesWaiting(driver->data_queue);
}

/**
 * @brief ADC 큐 초기화 (비우기)
 * @param driver ADC 드라이버 포인터
 */
void ADC_Driver_ClearQueue(ADC_Driver_t *driver)
{
    xQueueReset(driver->data_queue);
}



/**
 * @brief 샘플링 타이머 콜백 - ADC 변환 시작
 * @param xTimer FreeRTOS 타이머 핸들
 */
static void ADC_Timer_Callback(TimerHandle_t xTimer)
{
    ADC_Driver_t *driver = (ADC_Driver_t *)pvTimerGetTimerID(xTimer); 
    HAL_ADC_Start_DMA(&driver->hadc, (uint32_t *)driver->raw_buffer, ADC_CHANNEL_COUNT);
}

/**
 * @brief ADC 태스크 (변환 완료 시 데이터 처리)
 * @param pvParameters ADC_Driver_t 포인터
 */
void ADC_Task(void *pvParameters)
{
    ADC_Driver_t *driver = (ADC_Driver_t *)pvParameters;
    ADC_Data_t adc_data;

    for (;;) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        if (xSemaphoreTake(driver->data_mutex, pdMS_TO_TICKS(100)) == pdPASS) {
            adc_data.channel_count = ADC_CHANNEL_COUNT;
            adc_data.timestamp = xTaskGetTickCount();

            for (int i = 0; i < ADC_CHANNEL_COUNT; i++) {
                adc_data.channel_values[i] = driver->raw_buffer[i];
            }

            if (uxQueueSpacesAvailable(driver->data_queue) == 0) {
                ADC_Data_t dummy;
                xQueueReceive(driver->data_queue, &dummy, 0);
            }

            xQueueSend(driver->data_queue, &adc_data, 0);
            xSemaphoreGive(driver->data_mutex);
        }
    }
}

/**
 * @brief HAL 콜백 - ADC 변환 완료
 * @param hadc ADC 핸들
 */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    if (hadc->Instance == ADC1) {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        vTaskNotifyGiveFromISR(g_adc_driver.adc_task_handle, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

/**
 * @brief DMA 인터럽트 핸들러
 */
void DMA2_Stream0_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&g_adc_driver.hdma);
}

/**
 * @brief ADC 인터럽트 핸들러
 */
void ADC_IRQHandler(void)
{
    HAL_ADC_IRQHandler(&g_adc_driver.hadc);
}
