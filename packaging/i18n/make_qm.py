#!/usr/bin/env python3
# MSHV-IC8TEM: build src/HvTranslations/ic8tem_<lang>.qm and .ts from ic8tem_strings.py
# (Qt .qm binary format, same as lrelease: hashes + messages sections).
import os, struct, sys
from xml.sax.saxutils import escape
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from ic8tem_strings import LANGS, MESSAGES

MAGIC = bytes([0x3C,0xB8,0x64,0x18,0xCA,0xEF,0x9C,0x95,0xCD,0x21,0x1C,0xBF,0x60,0xA1,0xBD,0xDD])
TAG_HASHES, TAG_MESSAGES, TAG_LANGUAGE = 0x42, 0x69, 0xA7
T_END, T_TRANSLATION, T_SOURCE, T_CONTEXT = 1, 3, 6, 7
QT_LANG = {"bg":"bg_BG","ru":"ru_RU","zh":"zh_CN","zhhk":"zh_HK","eses":"es_ES","caes":"ca_ES","ptpt":"pt_PT",
           "roro":"ro_RO","dadk":"da_DK","plpl":"pl_PL","frfr":"fr_FR","ptbr":"pt_BR","nbno":"nb_NO",
           "itit":"it_IT","cscz":"cs_CZ"}

def elf_hash(b):
    h = 0
    for c in b:
        h = ((h << 4) + c) & 0xFFFFFFFF
        g = h & 0xF0000000
        if g:
            h ^= g >> 24
        h &= ~g & 0xFFFFFFFF
    return h or 1

def build_qm(lang):
    msgs = bytearray()
    hashes = []
    for ctx, src, tr in MESSAGES:
        t = tr.get(lang)
        if not t:
            continue
        off = len(msgs)
        tb = t.encode("utf-16-be")
        msgs += bytes([T_TRANSLATION]) + struct.pack(">I", len(tb)) + tb
        sb = src.encode("utf-8")
        msgs += bytes([T_SOURCE]) + struct.pack(">I", len(sb)) + sb
        cb = ctx.encode("utf-8")
        msgs += bytes([T_CONTEXT]) + struct.pack(">I", len(cb)) + cb
        msgs += bytes([T_END])
        hashes.append((elf_hash(sb), off))
    hashes.sort()
    hb = b"".join(struct.pack(">II", h, o) for h, o in hashes)
    lb = QT_LANG[lang].encode("utf-8")
    out = MAGIC
    out += bytes([TAG_HASHES]) + struct.pack(">I", len(hb)) + hb
    out += bytes([TAG_MESSAGES]) + struct.pack(">I", len(msgs)) + bytes(msgs)
    out += bytes([TAG_LANGUAGE]) + struct.pack(">I", len(lb)) + lb
    return out

def build_ts(lang):
    ctxs = {}
    for ctx, src, tr in MESSAGES:
        ctxs.setdefault(ctx, []).append((src, tr.get(lang, "")))
    s = ['<?xml version="1.0" encoding="utf-8"?>', '<!DOCTYPE TS>',
         '<TS version="2.1" language="%s">' % QT_LANG[lang]]
    for ctx, items in ctxs.items():
        s.append("<context>\n    <name>%s</name>" % ctx)
        for src, t in items:
            s.append("    <message>\n        <source>%s</source>\n        <translation>%s</translation>\n    </message>"
                     % (escape(src), escape(t)))
        s.append("</context>")
    s.append("</TS>\n")
    return "\n".join(s)

if __name__ == "__main__":
    outdir = sys.argv[1] if len(sys.argv) > 1 else os.path.join(os.path.dirname(__file__), "..", "..", "src", "HvTranslations")
    for lang in LANGS:
        missing = [src for ctx, src, tr in MESSAGES if not tr.get(lang)]
        if missing:
            print("missing", lang, missing)
        with open(os.path.join(outdir, "ic8tem_%s.qm" % lang), "wb") as f:
            f.write(build_qm(lang))
        with open(os.path.join(outdir, "ic8tem_%s.ts" % lang), "w", encoding="utf-8") as f:
            f.write(build_ts(lang))
    print("ok", len(LANGS), "languages,", len(MESSAGES), "messages")
