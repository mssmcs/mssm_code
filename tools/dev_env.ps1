[CmdletBinding()]
param(
    [string]$QtRoot = "C:/Qt",
    [string]$CMakeToolDir = "CMake_64",
    [string]$NinjaDir = "Ninja",
    [string]$MinGwDir = "mingw1310_64",
    [switch]$ShowOnly
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

function Add-PathIfExists([string]$PathToAdd) {
    if (Test-Path -LiteralPath $PathToAdd) {
        $current = [System.Environment]::GetEnvironmentVariable("PATH", "Process")
        $parts = @($current -split ";" | Where-Object { $_ -and $_.Trim() -ne "" })
        if ($parts -notcontains $PathToAdd) {
            [System.Environment]::SetEnvironmentVariable("PATH", "$PathToAdd;$current", "Process")
        }
        return $true
    }
    return $false
}

$cmakeBin = Join-Path $QtRoot ("Tools/" + $CMakeToolDir + "/bin")
$ninjaBin = Join-Path $QtRoot ("Tools/" + $NinjaDir)
$mingwBin = Join-Path $QtRoot ("Tools/" + $MinGwDir + "/bin")

[void](Add-PathIfExists -PathToAdd $cmakeBin)
[void](Add-PathIfExists -PathToAdd $ninjaBin)
[void](Add-PathIfExists -PathToAdd $mingwBin)

$cmakeExe = Get-Command cmake -ErrorAction SilentlyContinue
$ninjaExe = Get-Command ninja -ErrorAction SilentlyContinue
$gccExe = Get-Command gcc -ErrorAction SilentlyContinue
$gxxExe = Get-Command g++ -ErrorAction SilentlyContinue

if (-not $cmakeExe) {
    throw "Could not find 'cmake'. Checked Qt path: $cmakeBin"
}
if (-not $gxxExe) {
    throw "Could not find 'g++'. Checked Qt path: $mingwBin"
}

$env:CMAKE_C_COMPILER = $gccExe.Source
$env:CMAKE_CXX_COMPILER = $gxxExe.Source
if ($ninjaExe) {
    $env:CMAKE_MAKE_PROGRAM = $ninjaExe.Source
}

Write-Host "Developer environment ready (process-local):"
Write-Host "  cmake: $($cmakeExe.Source)"
if ($ninjaExe) {
    Write-Host "  ninja: $($ninjaExe.Source)"
}
else {
    Write-Host "  ninja: (not found; non-Ninja generators may still work)"
}
Write-Host "  gcc:   $($gccExe.Source)"
Write-Host "  g++:   $($gxxExe.Source)"

if (-not $ShowOnly) {
    & $cmakeExe.Source --version
    if ($ninjaExe) { & $ninjaExe.Source --version }
    & $gxxExe.Source --version
}
