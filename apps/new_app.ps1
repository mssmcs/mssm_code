[CmdletBinding()]
param(
    [Parameter(ValueFromRemainingArguments = $true)]
    [string[]]$Args
)

$scriptPath = Join-Path $PSScriptRoot "../tools/new_app.ps1"
if (-not (Test-Path -LiteralPath $scriptPath)) {
    throw "Cannot find tools/new_app.ps1 at: $scriptPath"
}

& $scriptPath @Args
exit $LASTEXITCODE
