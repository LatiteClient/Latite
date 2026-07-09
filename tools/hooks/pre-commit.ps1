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
$tempFiles = @()

try {
    foreach ($file in $files) {
        $checkoutOutput = & git checkout-index --temp -- $file
        if ($LASTEXITCODE -ne 0 -or -not $checkoutOutput) {
            Write-Host "Unable to read staged contents for $file."
            $failed = $true
            continue
        }

        $tempFile = ([string]($checkoutOutput | Select-Object -First 1) -split "`t", 2)[0]
        $tempFiles += $tempFile

        & clang-format --dry-run --Werror --assume-filename=$file $tempFile
        if ($LASTEXITCODE -ne 0) {
            $failed = $true
        }
    }
}
finally {
    foreach ($tempFile in $tempFiles) {
        Remove-Item -LiteralPath $tempFile -Force -ErrorAction SilentlyContinue
    }
}

if ($failed) {
    Write-Host ""
    Write-Host "clang-format found staged C++ contents that need formatting."
    Write-Host "Format individual files with:"
    Write-Host "  clang-format -i path\to\file.cpp"
    Write-Host "  git add path\to\file.cpp"
    Write-Host ""
    Write-Host "Or format all Latite sources with:"
    Write-Host "  Get-ChildItem src -Recurse -Include *.h,*.hpp,*.cpp,*.cxx,*.cc | ForEach-Object { clang-format -i `$_.FullName }"
    Write-Host "  git add src"
    Write-Host ""
    exit 1
}

exit 0
