MSHV-IC8TEM - portable folder for Linux / BSD
=============================================
Run ./mshv-ic8tem from this folder. Settings and logs stay in this folder
(as in the Windows version), so extract it somewhere you can write.

Required system libraries (Qt 5.15, ALSA, PulseAudio, FFTW3):
  Debian/Ubuntu/Mint/Raspberry Pi OS:
    sudo apt install libqt5widgets5 libqt5network5 libqt5websockets5 libasound2 libpulse0 libfftw3-double3
  Fedora:    sudo dnf install qt5-qtbase qt5-qtwebsockets alsa-lib pulseaudio-libs fftw-libs-double
  openSUSE:  sudo zypper in libQt5Widgets5 libQt5Network5 libQt5WebSockets5 alsa libpulse0 libfftw3-3
  Slackware: qt5, alsa-lib, pulseaudio, fftw (included in a full install)
  FreeBSD:   pkg install qt5-widgets qt5-network qt5-websockets alsa-lib pulseaudio fftw3
  OpenBSD:   pkg_add qtbase qtwebsockets portaudio-svn fftw3
             audio input is off by default: doas sysctl kern.audio.record=1
             (add kern.audio.record=1 to /etc/sysctl.conf to keep it)

Serial ports (CAT/PTT): add your user to the "dialout" group (Debian/Ubuntu)
or "uucp"/"dialer" (other systems).
