# =============================================================================
#  SmartGPS Attendance - phone / LAN helper
#  Prints the URL to open on a phone, opens the Windows Firewall port and
#  (optionally) updates the LAN hint used by the in-app "Dùng trên điện thoại" card.
#
#  Usage:
#    powershell -ExecutionPolicy Bypass -File scripts\phone-url.ps1
#    powershell -ExecutionPolicy Bypass -File scripts\phone-url.ps1 -Update
# =============================================================================
[CmdletBinding()]
param(
    [switch]$Update,
    [int]$Port = 8080
)

$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent $PSScriptRoot
$compose = Join-Path $root 'docker\docker-compose.yml'

function Get-LanIp {
    $all = Get-NetIPAddress -AddressFamily IPv4 |
        Where-Object {
            $_.IPAddress -notlike '127.*' -and
            $_.IPAddress -notlike '169.254.*' -and
            $_.InterfaceAlias -notmatch 'vEthernet|Loopback|WSL|Docker|VirtualBox|VMware'
        }
    $preferred = $all | Where-Object { $_.IPAddress -like '192.168.*' -or $_.IPAddress -like '10.*' } | Select-Object -First 1
    $ip = if ($preferred) { $preferred.IPAddress } else { ($all | Select-Object -First 1).IPAddress }
    if (-not $ip) { throw 'No LAN IPv4 address found. Connect the PC to Wi-Fi/Ethernet first.' }
    return $ip
}

function Ensure-FirewallRule {
    param([int]$Port)

    $ruleName = "SmartGPS Attendance TCP $Port"
    $isAdmin = ([Security.Principal.WindowsPrincipal][Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole(
        [Security.Principal.WindowsBuiltInRole]::Administrator)

    if (-not $isAdmin) {
        Write-Host 'Requesting administrator rights to open the firewall port...' -ForegroundColor Yellow
        Start-Process powershell -Verb RunAs -ArgumentList @(
            '-NoProfile', '-ExecutionPolicy', 'Bypass', '-Command',
            "New-NetFirewallRule -DisplayName '$ruleName' -Direction Inbound -Protocol TCP -LocalPort $Port -Action Allow -Profile Private,Domain -ErrorAction SilentlyContinue | Out-Null"
        ) | Out-Null
        return
    }

    if (-not (Get-NetFirewallRule -DisplayName $ruleName -ErrorAction SilentlyContinue)) {
        New-NetFirewallRule -DisplayName $ruleName -Direction Inbound -Protocol TCP `
            -LocalPort $Port -Action Allow -Profile Private,Domain -ErrorAction SilentlyContinue | Out-Null
    }
    Write-Host "Firewall: TCP $Port is open (profile Private/Domain)." -ForegroundColor Green
}

$ip = Get-LanIp
$url = "http://${ip}:$Port"

Write-Host ''
Write-Host '  ============================================' -ForegroundColor DarkGray
Write-Host '   SmartGPS Attendance - phone access' -ForegroundColor Cyan
Write-Host '  ============================================' -ForegroundColor DarkGray
Write-Host ''
Write-Host "   Phone URL : $url" -ForegroundColor Green
Write-Host '   Steps     : 1) phone and PC on the same Wi-Fi' -ForegroundColor Gray
Write-Host '               2) open the URL above in Chrome/Safari' -ForegroundColor Gray
Write-Host '               3) menu -> Add to Home Screen (runs full-screen)' -ForegroundColor Gray
Write-Host ''

Ensure-FirewallRule -Port $Port

try {
    $health = Invoke-WebRequest -UseBasicParsing -TimeoutSec 5 -Uri "http://localhost:$Port/api/health"
    Write-Host "Server     : $($health.StatusCode) OK on localhost:$Port" -ForegroundColor Green
} catch {
    Write-Host "Server     : not reachable on localhost:$Port (start it with docker compose up -d)" -ForegroundColor Yellow
}

if ($Update) {
    $envFile = Join-Path $root 'docker\.env'
    $envLines = @()
    if (Test-Path $envFile) { $envLines = Get-Content $envFile }
    $newLines = @()
    $found = $false
    foreach ($line in $envLines) {
        if ($line -match '^\s*LAN_HINT\s*=') {
            if (-not $found) { $newLines += "LAN_HINT=$url"; $found = $true }
        } else {
            $newLines += $line
        }
    }
    if (-not $found) { $newLines += "LAN_HINT=$url" }
    if (-not ($newLines -match '^\s*CLASSROOM_CIDR\s*=')) {
        $newLines += 'CLASSROOM_CIDR=192.168.1.0/24,192.168.137.0/24,172.16.0.0/12,127.0.0.0/8'
    }
    $newLines | Set-Content -Path $envFile -Encoding ASCII
    $env:LAN_HINT = $url
    Write-Host "Updating the container with LAN_HINT=$url ..." -ForegroundColor Yellow
    docker compose -f $compose up -d | Out-Null
    Write-Host 'Done. Reload the web app to see the new phone link.' -ForegroundColor Green
}
