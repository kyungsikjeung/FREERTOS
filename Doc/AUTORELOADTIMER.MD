# `autoreloadtimer.c` 튜토리얼

이 문서는 `autoreloadtimer.c` 예제 코드의 목적, 동작 방식 및 예상 결과에 대해 설명합니다.

## 1. Objective (목표)

이 예제의 주요 목표는 FreeRTOS의 **소프트웨어 타이머(Software Timer)**를 사용하여 특정 시간이 경과한 후 함수를 실행하는 방법을 시연하는 것입니다. 특히, **원-샷(One-shot) 타이머**와 **자동 리로드(Auto-reload) 타이머**의 차이점을 이해하고 활용하는 데 중점을 둡니다.

- **원-샷 타이머**: 설정된 시간이 한 번 만료되면 콜백 함수를 실행하고 중지됩니다.
- **자동 리로드 타이머**: 설정된 시간이 만료될 때마다 콜백 함수를 반복적으로 실행합니다.

## 2. Code Description (코드 설명)

- **`prvOneShotTimerCallback(TimerHandle_t xTimer)` (원-샷 타이머 콜백)**
  - 이 함수는 원-샷 타이머가 만료될 때 한 번만 호출됩니다.
  - 현재 시스템의 틱 카운트를 가져와 "One-shot timer callback executed at tick: [시간]" 형식의 메시지를 시리얼 포트로 출력합니다.
  - `ulCallCount` 변수를 1 증가시킵니다.

- **`prvAutoReloadTimerCallback(TimerHandle_t xTimer)` (자동 리로드 타이머 콜백)**
  - 이 함수는 자동 리로드 타이머가 만료될 때마다 주기적으로 호출됩니다.
  - 현재 시스템의 틱 카운트를 가져와 "Auto-reload timer callback executed at tick: [시간]" 형식의 메시지를 시리얼 포트로 출력합니다.
  - `ulCallCount` 변수를 1 증가시킵니다.

- **`main(void)`**
  - `Driver_Init()`를 호출하여 MCU의 하드웨어를 초기화합니다.
  - `xTimerCreate()` 함수를 사용하여 두 개의 타이머를 생성합니다.
    - **`xOneShotTimer`**: 7600ms 후에 만료되는 원-샷 타이머 (`pdFALSE`).
    - **`xAutoReloadTimer`**: 5000ms 주기로 반복되는 자동 리로드 타이머 (`pdTRUE`).
  - `xTimerStart()` 함수를 사용하여 두 타이머를 시작합니다.
  - `vTaskStartScheduler()`를 호출하여 FreeRTOS 스케줄러를 시작합니다. 타이머 콜백 함수는 타이머 서비스 태스크(Timer Service Task)의 컨텍스트에서 실행됩니다.

## 3. Expected Output (예상 결과)

코드가 실행되면 스케줄러가 시작되고 두 타이머가 활성화됩니다. 5000ms가 경과할 때마다 자동 리로드 타이머의 콜백이 실행되고, 7600ms가 경과했을 때 원-샷 타이머의 콜백이 한 번 실행됩니다.

### 3.1. 시리얼 출력 (Serial Output)

시리얼 터미널에서는 각 타이머의 콜백 함수가 실행될 때마다 메시지가 출력됩니다.

```
Auto-reload timer callback executed at tick: 5000
One-shot timer callback executed at tick: 7600
Auto-reload timer callback executed at tick: 10000
Auto-reload timer callback executed at tick: 15000
...
```

*(참고: 실제 틱 값은 시스템 부하 및 타이머 서비스 태스크의 우선순위에 따라 약간의 차이가 있을 수 있습니다.)*

## 4. Timer 동작 방식

1. **타이머 생성**: `main` 함수에서 원-샷 타이머와 자동 리로드 타이머가 각각 다른 주기로 생성됩니다.
2. **타이머 시작**: 두 타이머가 동시에 시작됩니다.
3. **콜백 실행**:
   - **5000ms 경과**: `xAutoReloadTimer`가 만료되어 `prvAutoReloadTimerCallback` 함수를 실행하고, 다시 5000ms 주기로 타이머를 재설정합니다.
   - **7600ms 경과**: `xOneShotTimer`가 만료되어 `prvOneShotTimerCallback` 함수를 한 번 실행하고, 타이머는 비활성 상태가 됩니다.
   - 이후 5000ms 마다 `prvAutoReloadTimerCallback` 함수만 계속 실행됩니다.

이 예제를 통해 정해진 시간에 특정 동작을 수행하거나, 주기적인 작업을 처리하는 방법을 배울 수 있습니다.
