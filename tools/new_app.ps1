[CmdletBinding()]
param(
    [string]$Template = "",
    [string]$Name = "",
    [string]$Destination = "",
    [string]$TemplateSubdir = "",
    [string]$Libraries = "",
    [switch]$ListTemplates,
    [switch]$ListLibraries,
    [switch]$NoGitInit,
    [switch]$Force
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

function Test-IsGitTemplate([string]$Value) {
    return $Value -match "^(https?|ssh|git|file)://|^git@|\.git$"
}

function Get-TemplateCatalog([string]$RepoRoot) {
    $catalogPath = Join-Path $RepoRoot "templates/templates.csv"
    if (-not (Test-Path -LiteralPath $catalogPath)) {
        return @()
    }
    return @(Import-Csv -LiteralPath $catalogPath)
}

function Select-TemplateInteractive([object[]]$Catalog) {
    $Catalog = @($Catalog)
    if (-not $Catalog -or $Catalog.Count -eq 0) {
        throw "No templates available. Add entries to templates/templates.csv first."
    }

    Write-Host "Select a template:"
    for ($i = 0; $i -lt $Catalog.Count; $i++) {
        $entry = $Catalog[$i]
        $desc = $entry.description
        if ([string]::IsNullOrWhiteSpace($desc)) {
            $desc = "(no description)"
        }
        Write-Host ("  {0}) {1} [{2}] - {3}" -f ($i + 1), $entry.id, $entry.source, $desc)
    }

    while ($true) {
        $raw = Read-Host "Enter template number"
        $choice = 0
        if ([int]::TryParse($raw, [ref]$choice) -and $choice -ge 1 -and $choice -le $Catalog.Count) {
            return $Catalog[$choice - 1].id
        }
        Write-Warning "Invalid selection. Enter a number from the list."
    }
}

function Get-EligibleDestinationFolders([string]$RepoRoot) {
    $excluded = @("assets", "libraries", "packaging", "shared", "templates", "tools", ".git")
    $dirs = @(Get-ChildItem -LiteralPath $RepoRoot -Directory | Where-Object {
        $name = $_.Name
        ($excluded -notcontains $name) -and -not $name.StartsWith(".")
    } | Sort-Object Name)
    return $dirs
}

function Select-DestinationInteractive([string]$RepoRoot) {
    $dirs = @(Get-EligibleDestinationFolders -RepoRoot $RepoRoot)
    if (-not $dirs -or $dirs.Count -eq 0) {
        throw "No eligible destination folders found under $RepoRoot."
    }

    Write-Host "Select destination folder:"
    for ($i = 0; $i -lt $dirs.Count; $i++) {
        Write-Host ("  {0}) {1}" -f ($i + 1), $dirs[$i].Name)
    }

    while ($true) {
        $raw = Read-Host "Enter destination number"
        $choice = 0
        if ([int]::TryParse($raw, [ref]$choice) -and $choice -ge 1 -and $choice -le $dirs.Count) {
            return $dirs[$choice - 1].FullName
        }
        Write-Warning "Invalid selection. Enter a number from the list."
    }
}

function Get-AvailableLibraries([string]$RepoRoot) {
    $libsRoot = Join-Path $RepoRoot "libraries"
    if (-not (Test-Path -LiteralPath $libsRoot)) {
        return @()
    }

    $entries = @()
    Get-ChildItem -LiteralPath $libsRoot -Directory | ForEach-Object {
        $cmakeFile = Join-Path $_.FullName "CMakeLists.txt"
        if (-not (Test-Path -LiteralPath $cmakeFile)) {
            return
        }

        $desc = ""
        $firstLines = Get-Content -LiteralPath $cmakeFile -TotalCount 80 -ErrorAction SilentlyContinue
        foreach ($line in $firstLines) {
            if ($line -match "MSSM_LIB_DESC\s*:\s*(.+)$") {
                $desc = $Matches[1].Trim()
                break
            }
        }

        $entries += [PSCustomObject]@{
            name = $_.Name
            description = $desc
        }
    }

    return @($entries | Sort-Object name)
}

function Resolve-TemplatePath([string]$RepoRoot, [string]$TemplateName) {
    if ([System.IO.Path]::IsPathRooted($TemplateName) -and (Test-Path -LiteralPath $TemplateName)) {
        return (Resolve-Path -LiteralPath $TemplateName).Path
    }

    $candidates = @(
        (Join-Path $RepoRoot ("templates/" + $TemplateName)),
        (Join-Path $RepoRoot ("apps/" + $TemplateName)),
        (Join-Path $RepoRoot $TemplateName)
    )

    foreach ($candidate in $candidates) {
        if (Test-Path -LiteralPath $candidate) {
            return (Resolve-Path -LiteralPath $candidate).Path
        }
    }

    throw "Template '$TemplateName' was not found. Try an absolute path, repo URL, or run with -ListTemplates."
}

function Copy-TemplateTree([string]$SourceRoot, [string]$TargetRoot) {
    $excludeNames = @(".git", ".qtcreator", "build", "CMakeFiles")
    $sourcePrefix = [System.IO.Path]::GetFullPath($SourceRoot)
    if (-not $sourcePrefix.EndsWith([System.IO.Path]::DirectorySeparatorChar)) {
        $sourcePrefix += [System.IO.Path]::DirectorySeparatorChar
    }
    New-Item -ItemType Directory -Path $TargetRoot -Force | Out-Null

    Get-ChildItem -LiteralPath $SourceRoot -Force -Recurse | ForEach-Object {
        $fullPath = [System.IO.Path]::GetFullPath($_.FullName)
        if (-not $fullPath.StartsWith($sourcePrefix, [System.StringComparison]::OrdinalIgnoreCase)) {
            return
        }
        $relative = $fullPath.Substring($sourcePrefix.Length)
        if ([string]::IsNullOrWhiteSpace($relative)) {
            return
        }

        $segments = $relative -split "[/\\]"
        foreach ($segment in $segments) {
            if ($excludeNames -contains $segment) {
                return
            }
        }

        if ($segments[-1] -eq "CMakeCache.txt") {
            return
        }

        $destinationPath = Join-Path $TargetRoot $relative
        if ($_.PSIsContainer) {
            New-Item -ItemType Directory -Path $destinationPath -Force | Out-Null
        }
        else {
            $destinationDir = Split-Path -Parent $destinationPath
            New-Item -ItemType Directory -Path $destinationDir -Force | Out-Null
            Copy-Item -LiteralPath $_.FullName -Destination $destinationPath -Force
        }
    }
}

function Set-ProjectLibraries([string]$TargetRoot, [string]$LibrariesSpec) {
    if ([string]::IsNullOrWhiteSpace($LibrariesSpec)) {
        return
    }

    $cmakeFile = Join-Path $TargetRoot "CMakeLists.txt"
    if (-not (Test-Path -LiteralPath $cmakeFile)) {
        Write-Warning "No CMakeLists.txt in '$TargetRoot'; skipped library update."
        return
    }

    $normalized = (($LibrariesSpec -split "[,; ]+" | Where-Object { $_ -and $_.Trim() -ne "" }) -join " ")
    if ([string]::IsNullOrWhiteSpace($normalized)) {
        return
    }

    $content = Get-Content -LiteralPath $cmakeFile -Raw
    $replacement = "set(LIBRARIES ${normalized})"
    $pattern = "(?m)^\s*set\s*\(\s*LIBRARIES[^\)]*\)\s*$"
    if ([System.Text.RegularExpressions.Regex]::IsMatch($content, $pattern)) {
        $content = [System.Text.RegularExpressions.Regex]::Replace($content, $pattern, $replacement, 1)
    }
    else {
        $content += "`n${replacement}`n"
    }
    Set-Content -LiteralPath $cmakeFile -Value $content -NoNewline
}

$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
$catalog = Get-TemplateCatalog -RepoRoot $repoRoot
$availableLibraries = Get-AvailableLibraries -RepoRoot $repoRoot

if ($ListTemplates) {
    if (-not $catalog -or $catalog.Count -eq 0) {
        Write-Host "No template catalog found at templates/templates.csv"
        exit 0
    }

    Write-Host "Available templates:"
    foreach ($entry in $catalog) {
        $id = $entry.id
        $source = $entry.source
        $desc = $entry.description
        Write-Host "  $id [$source] - $desc"
    }
    exit 0
}

if ($ListLibraries) {
    if (-not $availableLibraries -or $availableLibraries.Count -eq 0) {
        Write-Host "No libraries found under libraries/"
        exit 0
    }
    Write-Host "Available libraries:"
    foreach ($lib in $availableLibraries) {
        if ([string]::IsNullOrWhiteSpace($lib.description)) {
            Write-Host "  $($lib.name)"
        }
        else {
            Write-Host "  $($lib.name) - $($lib.description)"
        }
    }
    exit 0
}

if (-not $PSBoundParameters.ContainsKey("Template") -or [string]::IsNullOrWhiteSpace($Template)) {
    $Template = Select-TemplateInteractive -Catalog $catalog
}

if ([string]::IsNullOrWhiteSpace($Name)) {
    $Name = Read-Host "Enter new project folder name"
}
if ([string]::IsNullOrWhiteSpace($Name)) {
    throw "Project name cannot be empty."
}
if ($Name -match "[/\\]") {
    throw "Project name must be a folder name only (no path separators)."
}

if (-not $PSBoundParameters.ContainsKey("Destination") -or [string]::IsNullOrWhiteSpace($Destination)) {
    $destinationRoot = Select-DestinationInteractive -RepoRoot $repoRoot
}
elseif ([System.IO.Path]::IsPathRooted($Destination)) {
    $destinationRoot = $Destination
}
else {
    $destinationRoot = Join-Path $repoRoot $Destination
}
if (-not (Test-Path -LiteralPath $destinationRoot)) {
    New-Item -ItemType Directory -Path $destinationRoot -Force | Out-Null
}
$destinationRoot = (Resolve-Path -LiteralPath $destinationRoot).Path
$targetPath = Join-Path $destinationRoot $Name

if (Test-Path -LiteralPath $targetPath) {
    if (-not $Force) {
        throw "Destination already exists: $targetPath (use -Force to replace)."
    }
    Remove-Item -LiteralPath $targetPath -Recurse -Force
}

$tempClonePath = ""
try {
    $templatePath = ""
    $templateUrl = ""
    $templateBranch = ""
    $catalogEntry = $catalog | Where-Object { $_.id -eq $Template } | Select-Object -First 1

    if ($catalogEntry) {
        if ($catalogEntry.source -eq "git") {
            $templateUrl = $catalogEntry.location
            $templateBranch = $catalogEntry.branch
            if ([string]::IsNullOrWhiteSpace($TemplateSubdir) -and -not [string]::IsNullOrWhiteSpace($catalogEntry.subdir)) {
                $TemplateSubdir = $catalogEntry.subdir
            }
        }
        else {
            $templatePath = Resolve-TemplatePath -RepoRoot $repoRoot -TemplateName $catalogEntry.location
        }
    }
    elseif (Test-IsGitTemplate $Template) {
        $templateUrl = $Template
    }

    if (-not [string]::IsNullOrWhiteSpace($templateUrl)) {
        $git = Get-Command git -ErrorAction SilentlyContinue
        if (-not $git) {
            throw "Git is required to use a template repository URL."
        }
        $tempClonePath = Join-Path ([System.IO.Path]::GetTempPath()) ("mssm-template-" + [System.Guid]::NewGuid().ToString("N"))
        $cloneArgs = @("clone", "--depth", "1")
        if (-not [string]::IsNullOrWhiteSpace($templateBranch)) {
            $cloneArgs += @("--branch", $templateBranch)
        }
        $cloneArgs += @($templateUrl, $tempClonePath)
        & git @cloneArgs | Out-Null
        if ($LASTEXITCODE -ne 0) {
            throw "Failed to clone template repository: $templateUrl"
        }
        $templatePath = $tempClonePath
        if (-not [string]::IsNullOrWhiteSpace($TemplateSubdir)) {
            $templatePath = Join-Path $templatePath $TemplateSubdir
        }
        if (-not (Test-Path -LiteralPath $templatePath)) {
            throw "Template subdir not found: $TemplateSubdir"
        }
    }
    else {
        $templatePath = Resolve-TemplatePath -RepoRoot $repoRoot -TemplateName $Template
    }

    Copy-TemplateTree -SourceRoot $templatePath -TargetRoot $targetPath
    Set-ProjectLibraries -TargetRoot $targetPath -LibrariesSpec $Libraries

    if (-not [string]::IsNullOrWhiteSpace($Libraries)) {
        $requested = @($Libraries -split "[,; ]+" | Where-Object { $_ -and $_.Trim() -ne "" })
        $known = @($availableLibraries | ForEach-Object { $_.name })
        foreach ($lib in $requested) {
            if ($known -notcontains $lib) {
                Write-Warning "Library '$lib' was not found under libraries/. Keeping it as specified."
            }
        }
    }

    if (-not $NoGitInit) {
        $git = Get-Command git -ErrorAction SilentlyContinue
        if ($git) {
            Push-Location $targetPath
            try {
                & git init | Out-Null
            }
            finally {
                Pop-Location
            }
        }
        else {
            Write-Warning "Git was not found; skipped git init."
        }
    }
}
finally {
    if ($tempClonePath -and (Test-Path -LiteralPath $tempClonePath)) {
        Remove-Item -LiteralPath $tempClonePath -Recurse -Force
    }
}

Write-Host "Created new app at: $targetPath"
Write-Host "Template source: $Template"
if (-not [string]::IsNullOrWhiteSpace($Libraries)) {
    Write-Host "Libraries: $Libraries"
}
if (-not $NoGitInit) {
    Write-Host "Initialized new git repo."
}
Write-Host "Next step: re-run CMake for your container folder (for example: apps/CMakeLists.txt)."
