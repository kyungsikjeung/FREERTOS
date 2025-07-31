# 로그
```code
$port = New-Object System.IO.Ports.SerialPort COM6, 115200, None, 8, One
$port.Open()

$logFile = "C:\Logs\serial_log.txt"

while ($true) {
    if ($port.BytesToRead -gt 0) {
        $line = $port.ReadLine()
        $timestamp = (Get-Date).ToString("yyyy-MM-dd HH:mm:ss.fff")
        $logLine = "$timestamp $line"

        Write-Host $logLine
        Add-Content -Path $logFile -Value $logLine
    }
}
```