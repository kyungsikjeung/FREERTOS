# =============================
# !사용자 설정 (COM 포트, 보드레이트 및 로그 설정)
# =============================
$portName = "COM6"                 # 사용할 COM 포트
$baudRate = 115200                 # 보레이트 설정
$appendLog = $false                 # true: 기존 로그에 추가 / false: 기존 로그 삭제

# 색상 설정
$colors = @("White", "Cyan", "Yellow", "Green", "Magenta")
$colorIndex = 0

# =============================
# 초기 설정
# =============================
$logFile = ".\serial_log.txt"

# 기존 로그 삭제 여부
if (-not $appendLog -and (Test-Path $logFile)) {
    Remove-Item $logFile
}

# 시리얼 포트 열기
$port = New-Object System.IO.Ports.SerialPort $portName, $baudRate, "None", 8, "One"
$port.Open()

Write-Host "`n[INFO] Listening on $portName at $baudRate baud..." -ForegroundColor Green
Write-Host "[INFO] Logging to: $logFile" -ForegroundColor Green
Write-Host "[INFO] Append mode: $appendLog" -ForegroundColor Green
Write-Host "[INFO] Color cycling enabled`n" -ForegroundColor Green

# =============================
# 데이터 읽기 루프
# =============================
while ($true) {
    if ($port.BytesToRead -gt 0) {
        $line = $port.ReadLine()
        $timestamp = (Get-Date).ToString("HH:mm:ss.fff")
        $logLine = "$timestamp $line"

        # Task별로 색상 구분
        if ($line -match "Task1") {
            Write-Host $logLine -ForegroundColor Cyan
        }
        elseif ($line -match "Task2") {
            Write-Host $logLine -ForegroundColor Yellow
        }
        else {
            # 기타 메시지는 흰색
            Write-Host $logLine -ForegroundColor White
        }
        
        # 로그 파일에 저장 (색상 정보 없이)
        Add-Content -Path $logFile -Value $logLine
    }
}
