# `main.c` 튜토리얼

이 문서는 `main.c` 예제 코드의 목적, 동작 방식 및 예상 결과에 대해 설명합니다.

## 1. Objective (목표)

이 예제의 주요 목표는 FreeRTOS를 사용하여 두 개의 독립적인 **태스크(Task)**를 생성하고, 각 태스크가 주기적으로 특정 동작을 수행하도록 하는 기본적인 멀티태스킹 환경을 구축하는 것입니다. 이를 통해 FreeRTOS의 태스크 생성 및 스케줄링의 기본 개념을 이해합니다.

## 2. Code Description (코드 설명)

- **`Task1(void *pvParameters)`**
  - `pvParameters`로 전달받은 문자열(`"Task1 Msg !\r\n"`)을 `msg` 포인터에 저장합니다.
  - `while(1)` 무한 루프 안에서 다음 동작을 반복합니다.
    - `HAL_UART_Transmit()`: UART3를 통해 `msg` 문자열을 전송합니다.
    - `HAL_GPIO_TogglePin()`: LD1 LED(녹색)의 상태를 토글(ON/OFF)합니다.
    - `vTaskDelay()`: 1000ms (1초) 동안 태스크를 대기 상태로 만듭니다. 이 시간 동안 다른 태스크가 실행될 수 있습니다.

- **`Task2(void *pvParameters)`**
  - `pvParameters`로 전달받은 문자열(`"Task2 Msg !\r\n"`)을 `msg` 포인터에 저장합니다.
  - `while(1)` 무한 루프 안에서 다음 동작을 반복합니다.
    - `HAL_UART_Transmit()`: UART3를 통해 `msg` 문자열을 전송합니다.
    - `HAL_GPIO_TogglePin()`: LD2 LED(파란색)의 상태를 토글(ON/OFF)합니다.
    - `vTaskDelay()`: 1000ms (1초) 동안 태스크를 대기 상태로 만듭니다.

- **`main(void)`**
  - `Driver_Init()`: MCU의 하드웨어(클럭, GPIO, UART 등)를 초기화합니다.
  - `xTaskCreate()`: 두 개의 태스크를 생성합니다.
    - **`Task1`**: `Task1` 함수를 실행하며, 스택 크기는 256, 우선순위는 1로 설정하고, `task1_msg` 문자열을 파라미터로 전달합니다.
    - **`Task2`**: `Task2` 함수를 실행하며, 스택 크기는 256, 우선순위는 1로 설정하고, `task2_msg` 문자열을 파라미터로 전달합니다.
  - `vTaskStartScheduler()`: FreeRTOS 스케줄러를 시작합니다. 이 함수가 호출되면, FreeRTOS가 CPU의 제어권을 가져가 생성된 태스크들을 스케줄링 정책에 따라 실행합니다. 이 함수 호출 이후의 코드는 실행되지 않습니다.

## 3. Expected Output (예상 결과)

코드가 실행되면, `Task1`과 `Task2`가 번갈아 가며 실행됩니다. 두 태스크의 우선순위가 같기 때문에, FreeRTOS의 라운드 로빈(Round-Robin) 스케줄링 방식에 따라 타임 슬라이스(Time Slice)를 나누어 사용하거나, 한 태스크가 `vTaskDelay()`로 대기 상태에 들어가면 다른 태스크가 실행되는 방식으로 동작합니다.

### 3.1. 시리얼 출력 (Serial Output)

시리얼 터미널에는 1초마다 `Task1`과 `Task2`의 메시지가 번갈아 출력됩니다.

```
Task1 Msg  !
Task2 Msg  !
Task1 Msg  !
Task2 Msg  !
...
```

### 3.2. LED 동작

- **LD1 (녹색 LED)**: 1초 주기로 깜빡입니다 (`Task1`에 의해 제어).
- **LD2 (파란색 LED)**: 1초 주기로 깜빡입니다 (`Task2`에 의해 제어).

두 LED가 동시에 깜빡이는 것처럼 보일 수 있으며, 이는 두 태스크가 거의 동시에 실행되기 때문입니다.

## 4. Task 동작 방식

1. **태스크 생성**: `main` 함수에서 `Task1`과 `Task2`가 동일한 우선순위로 생성됩니다.
2. **스케줄러 시작**: `vTaskStartScheduler()`가 호출되면, 스케줄러는 준비(Ready) 상태에 있는 태스크 중 가장 우선순위가 높은 태스크를 실행합니다. 여기서는 두 태스크의 우선순위가 같으므로, 먼저 생성된 `Task1`부터 실행될 수 있습니다.
3. **태스크 실행 및 대기**: `Task1`이 실행되어 메시지를 출력하고 LED를 토글한 후, `vTaskDelay(1000)`를 만나 1초 동안 대기(Blocked) 상태가 됩니다.
4. **컨텍스트 스위칭(Context Switching)**: `Task1`이 대기 상태에 들어가면, 스케줄러는 다른 준비 상태의 태스크인 `Task2`를 실행합니다. `Task2`도 마찬가지로 작업을 수행한 후 1초 대기 상태가 됩니다.
5. **반복**: 이 과정이 계속 반복되면서, 두 태스크는 1초마다 번갈아 실행되는 것처럼 보입니다. 이것이 FreeRTOS의 기본적인 협력적 멀티태스킹 동작 방식입니다.
