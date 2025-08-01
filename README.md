# Nucleo-F429ZI FreeRTOS 테스트 프로젝트

이 프로젝트는 Nucleo-F429ZI 보드에서 FreeRTOS를 사용하여 다양한 기능을 테스트하기 위한 예제입니다. VSCode와 CMake를 사용하여 개발 환경을 구축하고, `terminal.ps1` 스크립트를 통해 시리얼 로그를 확인할 수 있습니다.

## 1. MCU 정보
*   **보드:** Nucleo-F429ZI
*   **MCU:** STM32F429ZIT6

## 2. 개발 환경 설정
### 2.1. 필수 설치 프로그램
프로젝트를 빌드하고 디버깅하기 위해 다음 프로그램들을 설치해야 합니다.
참조링크: https://youtu.be/aLD9zggmop4?si=lFmVP9OnK20WT8dq



## 3. 빌드 및 실행
### 3.1. 빌드할 소스 파일 변경

`CMakeLists.txt` 파일의 `MAIN_FILE` 변수 값을 수정하여 빌드할 메인 소스 파일을 변경할 수 있습니다. 예를 들어, `test_led.c` 파일을 빌드하고 싶다면 다음과 같이 수정합니다.

```cmake
# CMakeLists.txt
# Set the main file to use (default: main)
if(NOT DEFINED MAIN_FILE)
    set(MAIN_FILE "test_led") # "main" -> "test_led"
endif()
```

기본값은 `main.c` 입니다.

### 3.2. 빌드 및 디버깅

VSCode에서 `F5` 키를 누르면 자동으로 빌드 및 디버깅이 시작됩니다. `CMake Tools` 확장 프로그램이 설치되어 있어야 하며, `launch.json` 파일에 디버깅 설정이 미리 구성되어 있습니다.

## 4. 시리얼 로그 확인

`terminal.ps1`은 PowerShell 스크립트로, 시리얼 포트를 통해 MCU에서 전송하는 로그를 확인하는 데 사용됩니다.
 COM 포트와 통신 속도(Baud rate), 저장될 파일 Append 여부를 설정해 주세요.
 `serial_log.txt` 파일에 로그가 저장됩니다. 

### 4.1. 설정

```powershell
# terminal.ps1
$port = "COM3"       # 사용하는 COM 포트 번호로 변경
$baudRate = 115200   # 보드에 설정된 통신 속도와 일치시켜야 함
```

### 4.2. 실행
VSCode의 터미널에서 다음 명령을 실행하여 스크립트를 시작합니다.

```powershell
pwsh -ExecutionPolicy ByPass -File ./terminal.ps1
```

## 5. 프로젝트 구조 및 예제 코드

### 5.1. 파일 구조

```
C:/Users/TOVIS/test/TestRtos/
├── Core/
│   ├── Inc/
│   └── Src/
│       ├── main.c
│       ├── roundrobin.c
│       ├── preemption.c
│       ├── changepriority.c
│       └── receivingdata_multiplesource.c
├── Doc/
│   ├── ROUNDROBIN.MD
│   ├── PREEMPTION.MD
│   ├── CHANGEPRIORITY.MD
│   └── RECEIVINGDATA_MULTIPLESOURCE.MD
├── Drivers/
│   ├── CMSIS/
│   └── STM32F4xx_HAL_Driver/
├── Middlewares/
│   └── Third_Party/
│       └── FreeRTOS/
├── .gitignore
├── CMakeLists.txt
├── README.md
└── TestRtos.ioc
```

### 5.2. 예제 코드 설명

`Core/Src` 디렉토리에는 다양한 FreeRTOS 기능 테스트를 위한 예제 소스 코드들이 포함되어 있습니다.

- **`roundrobin.c`**: 동일 우선순위 태스크들의 라운드 로빈 스케줄링 예제
- **`preemption.c`**: 우선순위가 다른 태스크들의 선점형 스케줄링 예제
- **`changepriority.c`**: 태스크가 동적으로 우선순위를 변경하는 예제
- **`receivingdata_multiplesource.c`**: 여러 소스에서 데이터를 받아 처리하는 예제

각 예제 코드에 대한 자세한 설명은 `Doc` 폴더에 있는 해당 마크다운(.MD) 파일을 참고해 주세요.
