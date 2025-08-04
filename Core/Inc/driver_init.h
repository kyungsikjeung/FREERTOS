#ifndef __DRIVER_INIT_H__
#define __DRIVER_INIT_H__

#include "stm32f4xx_hal.h"
#include "string.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "timers.h"
#include "event_groups.h"
#include <string.h>
#include <stdio.h>



#ifdef __cplusplus
extern "C" {
#endif


extern UART_HandleTypeDef huart3;
extern PCD_HandleTypeDef hpcd_USB_OTG_FS;

/* Exported function prototypes ---------------------------------------------*/
void Driver_Init(void);


#ifdef __cplusplus
}
#endif

#endif /* __DRIVER_INIT_H__ */
