#!/usr/bin/env python3
"""Write THIRD-PARTY-NOTICES.txt for a built app bundle.

    third_party_notices.py APP SRC OUT

Every library in Contents/Frameworks and Contents/PlugIns is traced to the
Homebrew keg it was copied from. That keg supplies the version, the license
(from `brew info`) and the license texts. The script fails if a bundled
library can't be traced, or has no license text.
Code vendored in the source tree is listed from the table below.
"""
import glob
import hashlib
import json
import os
import subprocess
import sys

CELLAR = "/opt/homebrew/Cellar"
LICENSE_NAMES = ("COPYING", "COPYRIGHT", "LICENSE", "LICENCE", "LICENSE.TXT", "LICENSE.md",
                 "LICENCE.md", "COPYING.LESSER", "COPYING.LIB")
# Kegs that ship no license file: texts vendored in packaging/legal/texts.
FALLBACK_TEXTS = {
    "glib": ["LGPL-2.1.txt"],
    "qtbase": ["LGPL-3.0.txt"],
    "qtsvg": ["LGPL-3.0.txt"],
}
# Kegs where the bundled part is licensed differently from the formula as a
# whole: gettext's tools are GPL-3, but the only part bundled, libintl, is LGPL-2.1.
OVERRIDE = {
    "gettext": ("LGPL-2.1-or-later", ["LGPL-2.1.txt"]),
}
# What the LGPL asks of us, said once per library it applies to.
LGPL_NOTE = {
    "glib": "Linked dynamically. You may replace Contents/Frameworks/libglib-2.0.0.dylib and "
            "libgthread-2.0.0.dylib with a modified build of the same interface. Source: "
            "https://download.gnome.org/sources/glib/",
    "libusb": "Linked dynamically. You may replace Contents/Frameworks/libusb-1.0.0.dylib with a "
              "modified build of the same interface. Source: https://github.com/libusb/libusb/releases",
    "gettext": "Only libintl is included, linked dynamically. Source: "
               "https://ftp.gnu.org/gnu/gettext/",
    "qtbase": "Qt is used under the LGPL-3.0, unmodified and linked dynamically as replaceable "
              "frameworks in Contents/Frameworks and Contents/PlugIns. Source: "
              "https://download.qt.io/official_releases/qt/",
    "qtsvg": "Used under the LGPL-3.0, unmodified and linked dynamically. Source: "
             "https://download.qt.io/official_releases/qt/",
}
# Code compiled into the analyzer from this source tree, not from Homebrew.
IN_TREE = [
    ("MiniZip 1.1", "Zlib", "Copyright (C) 1998-2010 Gilles Vollant; Zip64 changes "
     "Copyright (C) 2009-2010 Mathias Svensson", "common/minizip/zip.h", (14, 33)),
    ("QDarkStyleSheet (the dark and light themes)", "MIT", "Copyright (C) 2013-2014 Colin Duquesnoy; "
     "Copyright (C) 2015-2016 Alex Huszagh", "DSView/themes/LICENSE.md", None),
]


def die(msg):
    sys.exit(f"third_party_notices: {msg}")


def bundled_files(app):
    """Library name -> path inside the bundle, for each library in Frameworks and PlugIns."""
    out = {}
    fw = os.path.join(app, "Contents/Frameworks")
    for p in sorted(os.listdir(fw)):
        out[p] = os.path.join(fw, p)
    for p in sorted(glob.glob(os.path.join(app, "Contents/PlugIns/*/*.dylib"))):
        out["PlugIns/" + os.path.relpath(p, os.path.join(app, "Contents/PlugIns"))] = p
    return out


def keg_of(name):
    """(formula, version) of the keg that provides a bundled library."""
    base = os.path.basename(name)
    if base.endswith(".framework"):
        pats = [f"{CELLAR}/*/*/lib/{base}", f"{CELLAR}/*/*/Frameworks/{base}"]
    elif name.startswith("PlugIns/"):
        pats = [f"{CELLAR}/*/*/share/qt/plugins/{name[len('PlugIns/'):]}"]
    else:
        pats = [f"{CELLAR}/*/*/lib/{base}"]
    hits = sorted({h for p in pats for h in glob.glob(p)})
    if base == "Python.framework":
        hits = [h for h in hits if "/python@3.14/" in h]
    found = {tuple(os.path.relpath(h, CELLAR).split(os.sep)[:2]) for h in hits}
    if len(found) != 1:
        die(f"cannot trace {name} to one Homebrew keg (found {hits or 'none'})")
    return found.pop()


def license_texts(src, formula, version):
    if formula in OVERRIDE:
        return [os.path.join(src, "packaging/legal/texts", n) for n in OVERRIDE[formula][1]]
    keg = os.path.join(CELLAR, formula, version)
    files = [os.path.join(keg, n) for n in sorted(os.listdir(keg)) if n in LICENSE_NAMES]
    files += [os.path.join(src, "packaging/legal/texts", n) for n in FALLBACK_TEXTS.get(formula, [])]
    if not files:
        die(f"{formula} {version} ships no license text; add one to FALLBACK_TEXTS")
    return files


def main():
    app, src, out = (os.path.realpath(a) for a in sys.argv[1:4])
    kegs = {}
    for name in bundled_files(app):
        kegs.setdefault(keg_of(name), []).append(name)
    info = json.loads(subprocess.run(
        ["brew", "info", "--json=v2", *sorted({f for f, _ in kegs})],
        capture_output=True, text=True, check=True).stdout)
    meta = {f["name"]: f for f in info["formulae"]}

    lines = [
        "Logic Analyze: third-party notices",
        "",
        "Logic Analyze is free software under the GNU General Public License, version 3 or",
        "later (licenses/GPL-3.0.txt). It is based on DSView by DreamSourceLab, which is",
        "based on PulseView and the sigrok project. The DSLogic device firmware is",
        "Copyright DreamSourceLab under the MIT License (licenses/DreamSourceLab-firmware-MIT.txt).",
        "Source: https://github.com/nullifyr/logic-analyze",
        "",
        "This file lists the other software included in the app and its license terms.",
        "The LGPL-3.0 is a set of additional permissions on top of the GPL-3.0, whose text",
        "is in licenses/GPL-3.0.txt.",
        "Full license texts follow the list; a text shared by several components appears once.",
        "",
        "=" * 78,
    ]
    texts = {}  # sha256 -> (label, path)
    for (formula, version), names in sorted(kegs.items()):
        m = meta[formula]
        refs = []
        for p in license_texts(src, formula, version):
            data = open(p, "rb").read()
            key = hashlib.sha256(data).hexdigest()
            texts.setdefault(key, (f"T{len(texts) + 1}", p, data))
            if texts[key][0] not in refs:
                refs.append(texts[key][0])
        lines += [
            "",
            f"{m.get('full_name', formula)} {version}",
            f"  License:  {OVERRIDE[formula][0] if formula in OVERRIDE else m['license']}",
            f"  Homepage: {m['homepage']}",
            f"  Files:    {', '.join(names)}",
            f"  Text:     {', '.join(refs)}",
        ]
        if formula in LGPL_NOTE:
            lines.append(f"  Note:     {LGPL_NOTE[formula]}")
    for title, spdx, holders, path, span in IN_TREE:
        full = os.path.join(src, path)
        body = open(full, encoding="utf-8").read()
        if span:
            body = "\n".join(body.splitlines()[span[0]:span[1]]) + "\n"
        key = hashlib.sha256(body.encode()).hexdigest()
        texts.setdefault(key, (f"T{len(texts) + 1}", full, body.encode()))
        lines += ["", title, f"  License:  {spdx}", f"  {holders}", f"  Text:     {texts[key][0]}"]

    lines += ["", "=" * 78, "License texts", "=" * 78]
    for label, path, data in texts.values():
        rel = os.path.relpath(path, CELLAR) if path.startswith(CELLAR) else os.path.relpath(path, src)
        lines += ["", f"--- {label} ({rel}) " + "-" * max(4, 70 - len(label) - len(rel)), "",
                  data.decode("utf-8", errors="replace").rstrip()]

    with open(out, "w", encoding="utf-8") as fh:
        fh.write("\n".join(lines) + "\n")
    print(f"notices: {sum(len(n) for n in kegs.values())} bundled files from {len(kegs)} kegs, "
          f"{len(IN_TREE)} in-tree components, {len(texts)} license texts")


if __name__ == "__main__":
    main()
