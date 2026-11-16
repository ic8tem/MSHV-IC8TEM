# Installazione

Pacchetti pronti: <https://github.com/ic8tem/MSHV-IC8TEM/releases>

| Sistema | File |
|---|---|
| Windows 64 bit | `…-windows-x64-setup.exe` (installatore) o `…-windows-x64.zip` (portatile) |
| Windows 32 bit | `…-windows-x86-setup.exe` o `…-windows-x86.zip` |
| Windows ARM64 | `…-windows-arm64-setup.exe` o `…-windows-arm64.zip` |
| Windows 7 SP1, 8, 8.1 (64 e 32 bit) | gli stessi file di Windows 10/11 (Qt 5.15 supporta Windows 7 SP1); su Windows 7 il controllo degli aggiornamenti non funziona (manca `curl`) |
| Linux x86_64 / aarch64 | `…-linux-<arch>.AppImage` (autonomo) o `…-linux-<arch>.tar.gz` (Qt5 di sistema) |
| Debian / Ubuntu / Mint | `…-debian-ubuntu-mint-<arch>.deb` |
| Fedora / openSUSE | `…-fedora-opensuse-x86_64.rpm` / `…-fedora-opensuse-aarch64.rpm` (ARM64) |
| Alpine Linux (musl) x86_64 / ARM64 | `…-linux-musl-x86_64.tar.gz` / `…-linux-musl-aarch64.tar.gz` (Qt5 di sistema: `apk add qt5-qtbase qt5-qtbase-x11 qt5-qtwebsockets alsa-lib libpulse fftw-double-libs`) |
| Linux i686 (32 bit) | `…-linux-i686.tar.gz` / `…-debian-ubuntu-mint-i386.deb` |
| Raspberry Pi OS 64 bit | `…-raspios-64bit-arm64.deb` |
| Raspberry Pi OS 32 bit | `…-raspios-32bit-armhf.deb` |
| Linux armv7l | `…-linux-armv7l-armhf.tar.gz` |
| Slackware ARM64 | `mshv-ic8tem-…-aarch64-1_ic8tem.txz` (installpkg) |
| FreeBSD / OpenBSD amd64 | `…-freebsd-amd64.tar.gz` / `…-openbsd-amd64.tar.gz` |
| macOS Apple Silicon / Intel | `…-macos-apple-silicon.dmg` / `…-macos-intel.dmg`, oppure lo `.zip` con l'app |

Il file `SHA256SUMS.txt` contiene le impronte di controllo.

## Dove finiscono impostazioni e log

* **Windows (installatore)**: programma in `%LOCALAPPDATA%\Programs\MSHV-IC8TEM`, senza diritti di amministratore. Impostazioni e log restano dopo aggiornamenti e disinstallazione.
* **Cartella del programma scrivibile** (zip portatile): tutto accanto all'eseguibile, come l'MSHV originale.
* **Cartella non scrivibile**: `%APPDATA%\MSHV-IC8TEM` (Windows) o `~/.local/share/MSHV-IC8TEM` (Linux/BSD).
* **macOS**: `~/Library/Application Support/MSHV-IC8TEM`.
* Variabile `MSHV_DATA_DIR`: forza una cartella a scelta.

Per riusare le impostazioni dell'MSHV originale, copiate la sua cartella `settings` nella cartella dati prima del primo avvio.

## Compilare

Vedi il README: *Actions › Build all distributions* su GitHub, `compila.bat` su Windows, `packaging/linux/build_unix.sh` su Linux/BSD.
