[CmdletBinding()]
param(
    [string]$BuildDir = "apps/build/agent",
    [string]$Generator = "Ninja",
    [string]$BuildType = "Debug",
    [switch]$Clean,
    [switch]$ConfigureOnly,
    [string[]]$CMakeArgs = @(),
    [string]$QtRoot = "C:/Qt",
    [string]$CMakeToolDir = "CMake_64",
    [string]$NinjaDir = "Ninja",
    [string]$MinGwDir = "mingw1310_64"
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
$appsRoot = Join-Path $repoRoot "apps"
$buildPath = Join-Path $repoRoot $BuildDir

. (Join-Path $PSScriptRoot "dev_env.ps1") `
    -QtRoot $QtRoot `
    -CMakeToolDir $CMakeToolDir `
    -NinjaDir $NinjaDir `
    -MinGwDir $MinGwDir `
    -ShowOnly

if ($Clean -and (Test-Path -LiteralPath $buildPath)) {
    Write-Host "Removing build dir: $buildPath"
    Remove-Item -LiteralPath $buildPath -Recurse -Force
}

New-Item -ItemType Directory -Path $buildPath -Force | Out-Null

$configureArgs = @(
    "-S", $appsRoot,
    "-B", $buildPath,
    "-G", $Generator,
    "-DCMAKE_BUILD_TYPE=$BuildType",
    "-DCMAKE_C_COMPILER=$env:CMAKE_C_COMPILER",
    "-DCMAKE_CXX_COMPILER=$env:CMAKE_CXX_COMPILER"
)

if ($env:CMAKE_MAKE_PROGRAM) {
    $configureArgs += "-DCMAKE_MAKE_PROGRAM=$env:CMAKE_MAKE_PROGRAM"
}
if ($CMakeArgs.Count -gt 0) {
    $configureArgs += $CMakeArgs
}

Write-Host "Configuring apps..."
& cmake @configureArgs
if ($LASTEXITCODE -ne 0) {
    throw "CMake configure failed."
}

if ($ConfigureOnly) {
    Write-Host "Configure complete (build skipped by -ConfigureOnly)."
    exit 0
}

Write-Host "Building apps..."
& cmake --build $buildPath --config $BuildType -j
if ($LASTEXITCODE -ne 0) {
    throw "Build failed."
}

Write-Host "Build complete."
