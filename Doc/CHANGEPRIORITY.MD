# `changepriority.c` 튜토리얼

이 문서는 `changepriority.c` 예제 코드의 목적, 동작 방식 및 예상 결과에 대해 설명합니다.

## 1. Objective (목표)

이 예제의 주요 목표는 FreeRTOS에서 태스크가 실행 중에 **자신 또는 다른 태스크의 우선순위를 동적으로 변경**하는 방법을 시연하는 것입니다. 이를 통해 프로그램의 요구사항에 따라 실시간으로 태스크의 중요도를 조절하여 시스템의 동작을 제어할 수 있습니다.

이 예제에서는 두 개의 태스크가 서로의 우선순위를 변경하며 CPU 제어권을 주고받는 과정을 보여줍니다.

## 2. Code Description (코드 설명)

- **`Task1(void *pvParameters)` (초기 우선순위: 2)**
  - `Task1`은 `Task2`보다 높은 우선순위로 시작합니다.
  - 실행되면, 시리얼 포트로 `"Task 1 raise priority of Task2\r\n"` 메시지를 출력합니다.
  - `vTaskPrioritySet()` 함수를 호출하여 `Task2`의 우선순위를 자신(`Task1`)의 우선순위보다 1 높은 값(즉, 3)으로 변경합니다.
  - `Task2`의 우선순위가 더 높아졌기 때문에, 이 코드가 실행된 직후 스케줄러는 `Task1`을 중단하고 `Task2`를 실행합니다.

- **`Task2(void *pvParameters)` (초기 우선순위: 1)**
  - `Task2`는 `Task1`보다 낮은 우선순위로 시작합니다.
  - `Task1`에 의해 우선순위가 높아져 실행되면, 시리얼 포트로 `"Task 2 lower task2 priority\r\n"` 메시지를 출력합니다.
  - `vTaskPrioritySet()` 함수를 호출하여 **자기 자신(NULL 핸들)**의 우선순위를 원래의 우선순위(1)보다 2 낮은 값(즉, -1, 실제로는 0이 됨)으로 변경합니다.
  - 자신의 우선순위를 `Task1`보다 낮췄기 때문에, 스케줄러는 다시 `Task1`을 실행합니다.

- **`main(void)`**
  - `Driver_Init()`를 호출하여 MCU의 하드웨어를 초기화합니다.
  - `xTaskCreate` 함수를 사용하여 `Task2`를 우선순위 `1`로, `Task1`을 우선순위 `2`로 생성합니다.
  - `vTaskStartScheduler()`를 호출하여 FreeRTOS 스케줄러를 시작합니다.

## 3. Expected Output (예상 결과)

코드가 실행되면 두 태스크가 서로의 우선순위를 변경하면서 제어권을 주고받는 과정이 반복됩니다.

### 3.1. 시리얼 출력 (Serial Output)

시리얼 터미널에서는 두 태스크의 메시지가 끊임없이 번갈아 출력됩니다.

1.  **시작:** `Task1`이 우선순위가 높으므로 먼저 실행되어 `Task 1 raise priority of Task2` 메시지를 출력합니다.
2.  `Task1`이 `Task2`의 우선순위를 높이면, 즉시 `Task2`가 실행되어 `Task 2 lower task2 priority` 메시지를 출력합니다.
3.  `Task2`가 자신의 우선순위를 낮추면, 다시 `Task1`이 실행되어 1번 과정이 반복됩니다.

```
Task2 created successfully
Task 1 raise priority of Task2
Task 2 lower task2 priority
Task 1 raise priority of Task2
Task 2 lower task2 priority
Task 1 raise priority of Task2
Task 2 lower task2 priority
...
```

이 예제는 `vTaskPrioritySet()` 함수를 사용하여 어떻게 태스크의 우선순위를 동적으로 제어하고, 이를 통해 프로그램의 실행 흐름을 바꿀 수 있는지를 명확하게 보여줍니다.
