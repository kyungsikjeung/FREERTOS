# `roundrobin.c` 튜토리얼

이 문서는 `roundrobin.c` 예제 코드의 목적, 동작 방식 및 예상 결과에 대해 설명합니다.

## 1. Objective (목표)

이 예제의 주요 목표는 FreeRTOS에서 동일한 우선순위를 가진 두 개의 태스크(Task)를 생성하여 **라운드 로빈(Round-Robin)** 스케줄링 방식을 시연하는 것입니다. 라운드 로빈 스케줄링에서는 동일 우선순위의 태스크들이 정해진 시간(Time Slice)만큼 번갈아 가며 실행됩니다. 이 예제에서는 두 개의 태스크가 1초 간격으로 번갈아 실행되며, 각각의 태스크는 LED를 토글하고 시리얼 포트로 메시지를 출력합니다.



## 2. Code Description (코드 설명)


- **`Task1(void *pvParameters)`**
  - `main` 함수에서 전달받은 문자열 (`"Task1 Msg  !\r\n"`)을 시리얼 포트(UART3)로 출력합니다.
  - 파란색 LED (LD1)를 토글(Toggle)합니다.
  - `vTaskDelay(pdMS_TO_TICKS(1000))`를 통해 1초 동안 대기 상태로 전환됩니다.

- **`Task2(void *pvParameters)`**
  - `main` 함수에서 전달받은 문자열 (`"Task2 Msg  !\r\n"`)을 시리얼 포트(UART3)로 출력합니다.
  - 초록색 LED (LD2)를 토글합니다.
  - `vTaskDelay(pdMS_TO_TICKS(1000))`를 통해 1초 동안 대기 상태로 전환됩니다.

- **`main(void)`**
  - `Driver_Init()`를 호출하여 MCU의 하드웨어를 초기화합니다.
  - `xTaskCreate` 함수를 사용하여 `Task1`과 `Task2`를 생성합니다. 두 태스크 모두 동일한 우선순위 `1`로 설정됩니다.
  - `vTaskStartScheduler()`를 호출하여 FreeRTOS 스케줄러를 시작합니다. 이 함수가 호출된 이후부터는 스케줄러가 태스크들을 관리합니다.

## 3. Expected Output (예상 결과)

코드가 실행되면 다음과 같은 결과를 확인할 수 있습니다.

### 3.1. 시리얼 출력 (Serial Output)

시리얼 터미널(`terminal.ps1` 또는 다른 터미널 프로그램)을 통해 1초마다 두 태스크의 메시지가 번갈아 출력됩니다.

```
Task1 Msg  !
Task2 Msg  !
Task1 Msg  !
Task2 Msg  !
...



