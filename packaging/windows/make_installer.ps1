# Build the Windows installer with Inno Setup 6 (installs it if missing).
# usage: make_installer.ps1 -SrcDir stage\MSHV-IC8TEM -Arch x64|x86|arm64 -OutDir dist
param([string]$SrcDir, [string]$Arch, [string]$OutDir)
$ErrorActionPreference = 'Stop'
$iscc = @("${env:ProgramFiles(x86)}\Inno Setup 6\ISCC.exe", "$env:ProgramFiles\Inno Setup 6\ISCC.exe", "$env:LOCALAPPDATA\Programs\Inno Setup 6\ISCC.exe") | Where-Object { Test-Path $_ } | Select-Object -First 1
if (-not $iscc) {
  Write-Host "Installing Inno Setup..."
  Invoke-WebRequest -UseBasicParsing -Uri "https://jrsoftware.org/download.php/is.exe" -OutFile "$env:RUNNER_TEMP\is.exe"
  Start-Process "$env:RUNNER_TEMP\is.exe" -ArgumentList '/VERYSILENT','/SUPPRESSMSGBOXES','/NORESTART','/SP-','/CURRENTUSER' -Wait
  $iscc = @("${env:ProgramFiles(x86)}\Inno Setup 6\ISCC.exe", "$env:ProgramFiles\Inno Setup 6\ISCC.exe", "$env:LOCALAPPDATA\Programs\Inno Setup 6\ISCC.exe") | Where-Object { Test-Path $_ } | Select-Object -First 1
}
if (-not $iscc) { throw "ISCC.exe not found" }
$src = (Resolve-Path $SrcDir).Path
New-Item -ItemType Directory -Force $OutDir | Out-Null
$out = (Resolve-Path $OutDir).Path
$name = "MSHV-IC8TEM-$env:MSHV_VERSION-windows-$Arch-setup"
& $iscc "/DAppVersion=$env:MSHV_VERSION" "/DSrcDir=$src" "/DArch=$Arch" "/DOutputName=$name" "/O$out" "packaging\windows\mshv-ic8tem.iss"
if ($LASTEXITCODE -ne 0) { throw "ISCC failed ($LASTEXITCODE)" }
Get-ChildItem $out
