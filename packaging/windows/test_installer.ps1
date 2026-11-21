# MSHV-IC8TEM - installer test on the CI runner (Windows): two installations side by side, the automatic
# update of the second one exactly as MSHV starts it (/SILENT /AUTOUPDATE=2 /DIR="<its folder>"), and the
# settings of each installation kept.
# usage: test_installer.ps1 -Setup dist\MSHV-IC8TEM-<version>-windows-x64-setup.exe
param([string]$Setup)
$ErrorActionPreference = 'Stop'
$setup = (Resolve-Path $Setup).Path
$a = Join-Path $env:SystemDrive 'MSHV-IC8TEM-NEW'      # default folder
$b = Join-Path $env:SystemDrive 'MSHV test 9700'       # second installation, with a space in the name
$desk = [Environment]::GetFolderPath('Desktop')
$sh = New-Object -ComObject WScript.Shell

function Run([string]$argline) {
  $p = Start-Process -FilePath $setup -ArgumentList $argline -Wait -PassThru
  if ($p.ExitCode -ne 0) { throw "setup $argline : exit $($p.ExitCode)" }
}
function Target([string]$name) {
  $l = Join-Path $desk "$name.lnk"
  if (-not (Test-Path -LiteralPath $l)) { return '' }
  return $sh.CreateShortcut($l).TargetPath
}
function Check([bool]$ok, [string]$what) {
  if (-not $ok) { throw "FAIL $what" }
  Write-Host "PASS $what"
}

Get-Process MSHV-IC8TEM -ErrorAction SilentlyContinue | Stop-Process -Force
Remove-Item -LiteralPath $a, $b -Recurse -Force -ErrorAction SilentlyContinue

# 1. first installation in the default folder, second one in another folder
Run '/VERYSILENT /SUPPRESSMSGBOXES /NORESTART /TASKS="desktopicon"'
Check (Test-Path "$a\MSHV-IC8TEM.exe") "first installation in $a"
Set-Content -LiteralPath "$a\settings\ci-marker.txt" -Value 'A'
Run ('/VERYSILENT /SUPPRESSMSGBOXES /NORESTART /TASKS="desktopicon" /DIR="' + $b + '"')
Check (Test-Path -LiteralPath "$b\MSHV-IC8TEM.exe") "second installation in $b"
Set-Content -LiteralPath "$b\settings\ci-marker.txt" -Value 'B'
Check ((Target 'MSHV-IC8TEM') -eq "$a\MSHV-IC8TEM.exe") "desktop shortcut MSHV-IC8TEM -> first installation"
Check ((Target 'MSHV-IC8TEM (MSHV test 9700)') -eq "$b\MSHV-IC8TEM.exe") "own desktop shortcut for the second installation"

# 2. automatic update of the second installation, as MSHV starts it
Remove-Item -LiteralPath "$a\COPYING.txt", "$b\COPYING.txt"
$p = Start-Process -FilePath $setup -ArgumentList ('/SILENT /SUPPRESSMSGBOXES /NORESTART /AUTOUPDATE=2 /DIR="' + $b + '"') -Wait -PassThru
Check ($p.ExitCode -eq 0) "automatic update of the second installation: exit 0"
Check (Test-Path -LiteralPath "$b\COPYING.txt") "second installation updated in its own folder"
Check (-not (Test-Path "$a\COPYING.txt")) "first installation not touched"
Check ((Get-Content -LiteralPath "$b\settings\ci-marker.txt") -eq 'B') "settings of the second installation kept"
Check ((Get-Content "$a\settings\ci-marker.txt") -eq 'A') "settings of the first installation kept"
Check ((Target 'MSHV-IC8TEM') -eq "$a\MSHV-IC8TEM.exe") "shortcut of the first installation unchanged"
Start-Sleep -Seconds 2
Check (-not (Get-Process MSHV-IC8TEM -ErrorAction SilentlyContinue)) "the installer did not start MSHV (AUTOUPDATE=2: MSHV restarts itself)"

# 3. installing again into the first folder while the registered folder is the second one: the first
#    installation keeps its own settings (up to ic8tem.30 they were replaced by those of the other folder)
Run ('/VERYSILENT /SUPPRESSMSGBOXES /NORESTART /DIR="' + $a + '"')
Check ((Get-Content "$a\settings\ci-marker.txt") -eq 'A') "settings kept when the registered folder is another installation"
Check ((Get-Content -LiteralPath "$b\settings\ci-marker.txt") -eq 'B') "settings of the other installation kept"

# 4. uninstall both (settings and logs stay, as designed)
foreach ($d in $b, $a) {
  if (Test-Path -LiteralPath "$d\unins000.exe") {
    Start-Process -FilePath "$d\unins000.exe" -ArgumentList '/VERYSILENT /SUPPRESSMSGBOXES /NORESTART' -Wait | Out-Null
  }
}
Get-Process MSHV-IC8TEM -ErrorAction SilentlyContinue | Stop-Process -Force
Remove-Item -LiteralPath $a, $b -Recurse -Force -ErrorAction SilentlyContinue
Write-Host "Installer test: all checks passed"
