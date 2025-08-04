# `mailboxqueue.c` 튜토리얼

이 문서는 `mailboxqueue.c` 예제 코드의 목적, 동작 방식 및 예상 결과에 대해 설명합니다.

## 1. Objective (목표)

이 예제의 주요 목표는 FreeRTOS의 **큐(Queue)**를 **메일박스(Mailbox)** 형태로 사용하여, 항상 최신의 데이터 상태를 유지하고 공유하는 방법을 시연하는 것입니다. 메일박스는 길이가 1인 큐로, 새로운 데이터가 들어오면 이전 데이터를 덮어쓰는 특징을 가집니다.

이 예제에서는 하나의 **업데이터(Updater) 태스크**가 주기적으로 값을 갱신하여 메일박스에 쓰고, 다른 하나의 **리더(Reader) 태스크**는 메일박스에서 최신 값을 읽어와 처리하는 구조를 보여줍니다.

## 2. Code Description (코드 설명)

- **`Example_t` (데이터 구조체)**
  - `value`: 전송할 데이터 값 (카운터 값).
  - `timestamp`: 데이터가 생성된 시점의 틱 카운트. 이를 통해 수신 측에서 데이터의 최신 여부를 판별할 수 있습니다.

- **`vUpdaterTask(void *pvParameters)` (업데이터 태스크)**
  - 500ms마다 `counter` 값을 1씩 증가시킵니다.
  - `Example_t` 구조체에 현재 카운터 값과 시스템 틱 카운트(`timestamp`)를 저장합니다.
  - `xQueueOverwrite()` 함수를 사용하여 메일박스(큐)에 데이터를 씁니다. 큐가 이미 차 있어도(길이가 1이므로 항상 차 있음) 블로킹되지 않고, 기존 데이터를 **덮어씁니다**.

- **`vReaderTask(void *pvParameters)` (리더 태스크)**
  - `xQueuePeek()` 함수를 사용하여 메일박스에서 데이터를 읽습니다. `xQueueReceive()`와 달리, `xQueuePeek()`는 큐에서 데이터를 **제거하지 않고** 읽기만 합니다.
  - 데이터 수신에 성공하면, 수신한 데이터의 `timestamp`와 이전에 처리했던 `lastTimestamp`를 비교합니다.
  - `timestamp`가 더 최신인 경우에만 새로운 데이터로 간주하고, 시리얼 포트로 값을 출력한 후 `lastTimestamp`를 갱신합니다. 이를 통해 중복 데이터 처리를 방지합니다.
  - 100ms 주기로 메일박스를 확인합니다.

- **`main(void)`**
  - `Driver_Init()`를 호출하여 MCU의 하드웨어를 초기화합니다.
  - `xQueueCreate()`를 사용하여 `Example_t` 타입의 데이터를 저장할 수 있는 **길이 1**의 큐(메일박스)를 생성합니다.
  - `xTaskCreate` 함수를 사용하여 `Updater`와 `Reader` 태스크를 생성합니다.
    - `Updater` 우선순위: 2
    - `Reader` 우선순위: 1
  - `vTaskStartScheduler()`를 호출하여 FreeRTOS 스케줄러를 시작합니다.

## 3. Expected Output (예상 결과)

코드가 실행되면 `Updater` 태스크는 500ms마다 메일박스의 값을 새로운 카운터 값으로 덮어씁니다. `Reader` 태스크는 100ms마다 메일박스를 확인하여, `Updater`가 값을 변경했을 때만 새로운 데이터를 출력합니다. `Reader`의 확인 주기가 `Updater`의 갱신 주기보다 짧기 때문에, `Updater`가 값을 갱신할 때마다 `Reader`는 거의 즉시 이를 감지하고 출력하게 됩니다.

### 3.1. 시리얼 출력 (Serial Output)

시리얼 터미널에는 `Reader` 태스크가 새로 수신한 값과 해당 데이터가 생성된 시점의 틱 카운트가 출력됩니다. 약 500ms 간격으로 새로운 값이 출력되는 것을 확인할 수 있습니다.

```
Received value: 0 (Tick: [tick_value_0])
Received value: 1 (Tick: [tick_value_1])
Received value: 2 (Tick: [tick_value_2])
Received value: 3 (Tick: [tick_value_3])
...
```

## 4. Mailbox 동작 방식

1. **초기 상태**: `Updater` 태스크가 먼저 실행되어 초기값(0)을 메일박스에 씁니다. `Reader` 태스크는 메일박스에 데이터가 들어오기를 기다립니다.
2. **데이터 읽기**: `Reader`가 `xQueuePeek()`으로 데이터를 읽고 출력합니다. 데이터는 큐에서 제거되지 않습니다.
3. **데이터 덮어쓰기**: 500ms 후, `Updater`가 새로운 값(1)으로 메일박스의 기존 데이터를 **덮어씁니다**.
4. **최신 데이터 확인**: `Reader`는 100ms마다 메일박스를 확인하다가, `timestamp`가 변경된 것을 감지하고 새로운 값을 출력합니다.

이처럼 메일박스는 시스템의 현재 상태(예: 센서 값, 설정 값)를 여러 태스크에 공유할 때 유용합니다. 모든 중간 과정이 아닌, **가장 최신의 상태**만이 중요할 때 효과적인 통신 방법입니다.
