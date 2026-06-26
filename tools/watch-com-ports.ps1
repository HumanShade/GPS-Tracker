# Watches serial (COM) ports and logs when they appear / disappear, with millisecond
# timestamps. Useful for diagnosing an ESP32 over native USB:
#   * a port that appears and *stays*  -> board is in ROM download mode (good; flash now)
#   * a port that flickers in and out   -> firmware ran briefly then deep-slept (USB dropped)
#   * nothing at all                    -> strap not entering download mode, or cable/driver issue
#
# Usage (any PowerShell):  .\tools\watch-com-ports.ps1
# Stop with Ctrl-C. Optional: -IntervalMs 50  (faster polling), -Seconds 60 (auto-stop).
[CmdletBinding()]
param(
    [int]$IntervalMs = 75,
    [int]$Seconds = 0   # 0 = run until Ctrl-C
)

function Get-PortNames {
    Get-CimInstance Win32_PnPEntity -ErrorAction SilentlyContinue |
        Where-Object { $_.Name -match '\((COM\d+)\)' } |
        ForEach-Object { @{ Port = $Matches[1]; Name = $_.Name } }
}

Write-Host "Watching COM ports every $IntervalMs ms (Ctrl-C to stop)..." -ForegroundColor Cyan
$deadline = if ($Seconds -gt 0) { (Get-Date).AddSeconds($Seconds) } else { [datetime]::MaxValue }

$prev = @([System.IO.Ports.SerialPort]::GetPortNames())
foreach ($p in ($prev | Sort-Object)) {
    $n = (Get-PortNames | Where-Object { $_.Port -eq $p }).Name
    Write-Host ("{0:HH:mm:ss.fff}  present  {1}  {2}" -f (Get-Date), $p, $n)
}

while ((Get-Date) -lt $deadline) {
    $now = @([System.IO.Ports.SerialPort]::GetPortNames())
    foreach ($p in ($now | Where-Object { $_ -notin $prev })) {
        $n = (Get-PortNames | Where-Object { $_.Port -eq $p }).Name
        Write-Host ("{0:HH:mm:ss.fff}  + APPEARED  {1}  {2}" -f (Get-Date), $p, $n) -ForegroundColor Green
    }
    foreach ($p in ($prev | Where-Object { $_ -notin $now })) {
        Write-Host ("{0:HH:mm:ss.fff}  - gone      {1}" -f (Get-Date), $p) -ForegroundColor Red
    }
    $prev = $now
    Start-Sleep -Milliseconds $IntervalMs
}
