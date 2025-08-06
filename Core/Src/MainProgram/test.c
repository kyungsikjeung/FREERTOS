#include "main.h"
#include "driver_init.h"
#include "string.h"     // for memcpy, memcmp

/* 공유 배열 (Race Condition 실험 대상) */
volatile uint8_t shared[4] = {0};

/* 기준값 */
const uint8_t expected1[4] = {0xAA, 0xAA, 0xAA, 0xAA};
const uint8_t expected2[4] = {0x55, 0x55, 0x55, 0x55};

/* Writer Task (의도적 커럽션 유도) */
void vTask1(void *pvParameters)
{
    while (1)
    {

        
        memcpy((void *)shared, expected1, 4);  // 비원자적 복사
        taskYIELD(); 
        
        // 빠른 루프로 충돌 가능성 증가
    }
}

/* Reader Task (중간 상태까지 포함해서 감지) */
void vTask2(void *pvParameters)
{
    uint8_t temp[4];  // 로컬 복사 버퍼

    while (1)
    {
        memcpy(temp, (const void *)shared, 4);  // 일관성 있는 순간 값 확보
        // 만약 복사된 temp값이 {0x11, 0x22, 0x33, 0x44} 혹은 {0x44, 0x33, 0x22, 0x11}; 둘중 하나가 아니라면

        if (memcmp(temp, expected1, 4) != 0 && memcmp(temp, expected2, 4) != 0){
            printf("❌ FAULT DETECTED: shared = {%02X %02X %02X %02X}\n",
                   temp[0], temp[1], temp[2], temp[3]);
        }
        else
        {
            printf("🟢 shared OK: {%02X %02X %02X %02X}\n",
                   temp[0], temp[1], temp[2], temp[3]);
        }
        taskYIELD(); 
    }
}

int main(void)
{
    Driver_Init();  // UART 등 주변 장치 초기화

    // Task 생성
    xTaskCreate(vTask1, "Writer", 256, NULL, 1, NULL);
    xTaskCreate(vTask2, "Reader", 256, NULL, 1, NULL);

    // FreeRTOS 스케줄러 시작
    vTaskStartScheduler();

    // 도달하지 않음
    while (1);
}
