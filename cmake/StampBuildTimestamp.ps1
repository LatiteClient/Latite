param(
    [Parameter(Mandatory = $true)]
    [string]$BinaryPath,

    [AllowEmptyString()]
    [string]$Timestamp = "",

    [AllowEmptyString()]
    [string]$Configuration = ""
)

$ErrorActionPreference = "Stop"

if (-not [string]::IsNullOrWhiteSpace($Configuration) -and
    $Configuration -notin @("Debug", "Nightly")) {
    Write-Host "Skipping Latite UTC build timestamp stamp for $Configuration configuration"
    return
}

if ([string]::IsNullOrWhiteSpace($Timestamp)) {
    $Timestamp = [DateTime]::UtcNow.ToString("yyyy-MM-dd_HH-mm-ss'Z'")
}

if ($Timestamp -notmatch '^\d{4}-\d{2}-\d{2}_\d{2}-\d{2}-\d{2}Z$') {
    throw "Build timestamp must use YYYY-MM-DD_HH-mm-ssZ UTC+0 format; got '$Timestamp'"
}

$placeholder = [Text.Encoding]::ASCII.GetBytes("0000-00-00_00-00-00Z")
$replacement = [Text.Encoding]::ASCII.GetBytes($Timestamp)

if ($placeholder.Length -ne $replacement.Length) {
    throw "Replacement timestamp length must match placeholder length"
}

$bytes = [IO.File]::ReadAllBytes($BinaryPath)
$patchCount = 0

for ($i = 0; $i -le $bytes.Length - $placeholder.Length; $i++) {
    $matched = $true
    for ($j = 0; $j -lt $placeholder.Length; $j++) {
        if ($bytes[$i + $j] -ne $placeholder[$j]) {
            $matched = $false
            break
        }
    }

    if ($matched) {
        [Array]::Copy($replacement, 0, $bytes, $i, $replacement.Length)
        $patchCount++
        $i += $placeholder.Length - 1
    }
}

if ($patchCount -eq 0) {
    throw "Build timestamp placeholder was not found in '$BinaryPath'"
}

[IO.File]::WriteAllBytes($BinaryPath, $bytes)
Write-Host "Stamped Latite UTC build timestamp $Timestamp into $BinaryPath"
