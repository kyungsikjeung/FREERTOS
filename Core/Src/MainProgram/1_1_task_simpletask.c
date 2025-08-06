#include "main.h"
#include "driver_init.h"

void ledLD1(void *inputData){
    while (1) {
        printf("Hello from ledLD1\r\n");
        HAL_GPIO_TogglePin(GPIOB, LD1_Pin);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
void ledLD2(void *inputData){
    while (1) {
        printf("Hello from ledLD2\r\n");
        HAL_GPIO_TogglePin(GPIOB, LD2_Pin);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

int main(){
    Driver_Init();
    
    xTaskCreate(ledLD1, "Starter", 256, NULL, 2, NULL);
    xTaskCreate(ledLD2, "Another", 256, NULL, 1, NULL);
    vTaskStartScheduler();
    while (1) {
    }

    return 0;

}


