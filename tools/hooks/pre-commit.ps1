#!/usr/bin/env pwsh

$ErrorActionPreference = "Stop"

if (-not (Get-Command clang-format -ErrorAction SilentlyContinue)) {
    Write-Host "clang-format was not found on PATH."
    Write-Host "Install clang-format or add it to PATH before committing C++ changes."
    exit 1
}

$gitOutput = & git diff --cached --name-only --diff-filter=ACMR -z
if ($null -eq $gitOutput) {
    exit 0
}

$rawFiles = [string]::Join("", [string[]]$gitOutput)
$files = $rawFiles -split "`0" | Where-Object {
    $_ -and ($_ -match '\.(c|cc|cxx|cpp|h|hpp)$')
}

if (-not $files) {
    exit 0
}

$failed = $false

foreach ($file in $files) {
    if (-not (Test-Path -LiteralPath $file)) {
        continue
    }

    & clang-format --dry-run --Werror $file
    if ($LASTEXITCODE -ne 0) {
        $failed = $true
    }
}

if ($failed) {
    Write-Host ""
    Write-Host "clang-format found staged C++ files that need formatting."
    Write-Host "Format individual files with:"
    Write-Host "  clang-format -i path\to\file.cpp"
    Write-Host ""
    Write-Host "Or format all Latite sources with:"
    Write-Host "  Get-ChildItem src -Recurse -Include *.h,*.hpp,*.cpp,*.cxx,*.cc | ForEach-Object { clang-format -i `$_.FullName }"
    Write-Host ""
    exit 1
}

exit 0
