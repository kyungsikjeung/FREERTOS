QueueHandle_t xQueue = xQueueCreate(10, sizeof(int));
int valueToSend = 123;
xQueueSend(xQueue, &valueToSend, portMAX_DELAY);

int receivedValue;
if (xQueueReceive(xQueue, &receivedValue, portMAX_DELAY)) {
    // Received from queue
}


void Task1(void *pvParameters) {
    while (1) {
        // Do something
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

xTaskCreate(Task1, "Task1", 128, NULL, 1, NULL);

QueueHandle_t xQueue;

static void SenderTask(void *pvParameters) {
    int valueToSend = 123;
    while (1) {
        xQueueSendToBack(xQueue, &valueToSend, portMAX_DELAY);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

static void ReceiverTask(void *pvParameters) {
    int receivedValue;
    while (1) {
        if (xQueueReceive(xQueue, &receivedValue, portMAX_DELAY) == pdPASS) {
            // Handle received data
        }
    }
}

void InitQueueAndTasks(void) {
    xQueue = xQueueCreate(10, sizeof(int));
    if (xQueue != NULL) {
        xTaskCreate(SenderTask, "Sender", 128, NULL, 1, NULL);
        xTaskCreate(ReceiverTask, "Receiver", 128, NULL, 1, NULL);
    }
}


xTaskCreate(Task2, "Task2", 128, NULL, 1, NULL);



void *pvPortMalloc( size_t xWantedSize )



void vPortFree( void *pv )