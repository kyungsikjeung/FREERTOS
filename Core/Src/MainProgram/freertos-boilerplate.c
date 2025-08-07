#include "main.h"
#include "driver_init.h"

int main(void) {
    // Hardware Initialization
    Driver_Init();

    vTaskStartScheduler();

    // Loop forever if scheduler fails
    while(1);
    return 0;
}