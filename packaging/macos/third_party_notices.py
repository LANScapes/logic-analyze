#!/usr/bin/env python3
"""Write THIRD-PARTY-NOTICES.txt for a built app bundle.

    third_party_notices.py APP SRC OUT

Every library in Contents/Frameworks and Contents/PlugIns is traced to the
Homebrew keg it was copied from, by matching Mach-O UUIDs (install_name_tool
and codesign do not change them). Each keg supplies the version, the license
(from `brew info`), the license texts and the exact source archive. Some
notices exist only in the source archive (fetched with `brew fetch -s` when
not cached), so that is read too:
- Qt's per-module attributions, from its SBOM, with license texts from its
  LICENSES directory;
- Python's incorporated-software notices, from Doc/license.rst;
- license files that a keg's own license file refers to.

The script fails if a bundled library cannot be traced, has no license text,
or its source archive cannot be had. Code vendored in the source tree is
listed from the IN_TREE table below.
"""
import glob
import hashlib
import json
import os
import re
import subprocess
import sys
import tarfile

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
# License texts read from the source archive (paths below its top directory):
# files that a keg's own license file refers to, and notices of code compiled
# into it. A number after the path keeps only that many leading lines.
FROM_SOURCE = {
    "dbus": ["LICENSES/AFL-2.1.txt"],
    "jpeg-turbo": ["README.ijg"],
    "freetype": ["docs/FTL.TXT", "src/bdf/README", "src/pcf/README"],
    # HACL* (hashlib's _md5, _sha1, _sha2, _sha3, _blake2, _hmac) is missing from
    # Doc/license.rst; its MIT notice heads each of its files.
    "python@3.14": [("Modules/_hacl/Hacl_Hash_SHA2.c", 23), "Lib/ctypes/macholib/README.ctypes"],
}
# Which license we use where the formula offers a choice.
CHOICE = {
    "dbus": "used under the Academic Free License 2.1",
    "freetype": "used under the FreeType License (FTL)",
    "graphite2": "used under the LGPL-2.1-or-later",
    "zstd": "used under the BSD-3-Clause license",
}
# Libraries under the LGPL or GPL: section 2 of the output lists their exact
# source, and says how to replace the LGPL ones.
COPYLEFT = {"glib", "libusb", "gettext", "qtbase", "qtsvg", "fftw", "graphite2"}
# Qt modules and plugins that ship; their SBOM attributions apply.
QT_MODULES = ("Core", "Gui", "Widgets", "DBus", "Svg", "QCocoaIntegrationPlugin",
              "QMacStylePlugin", "QGifPlugin", "QICOPlugin", "QJpegPlugin", "QSvgPlugin",
              "QSvgIconPlugin")
# Code compiled or copied into the app from this source tree, not from Homebrew:
# (title, license, holders, file with the license text, (first, last) line span or None).
# The MIT and BSD decoders also ship as source with their headers intact.
IN_TREE = [
    ("MiniZip 1.1 (common/minizip)", "Zlib", "Copyright (C) 1998-2010 Gilles Vollant; Zip64 changes "
     "Copyright (C) 2009-2010 Mathias Svensson", "common/minizip/zip.h", (0, 33)),
    ("QDarkStyleSheet (the dark and light themes)", "MIT", "Copyright (C) 2013-2014 Colin Duquesnoy; "
     "Copyright (C) 2015-2016 Alex Huszagh", "DSView/themes/LICENSE.md", None),
    ("RangeDict (spi_tpm decoder)", "MIT", "Copyright (c) 2015 WKPlus",
     "libsigrokdecode4DSL/decoders/spi_tpm/RangeDict-LICENSE.txt", None),
    ("caliper decoder", "MIT", "Copyright (C) 2020 Tomas Mudrunka",
     "libsigrokdecode4DSL/decoders/caliper/pd.py", (0, 22)),
    ("enc28j60 decoder", "MIT", "Copyright (C) 2019 Jiahao Li",
     "libsigrokdecode4DSL/decoders/enc28j60/pd.py", (0, 22)),
    ("ade77xx decoder", "MIT", "Copyright (C) 2017 Karl Palsson",
     "libsigrokdecode4DSL/decoders/ade77xx/pd.py", (0, 22)),
    ("nrf905 decoder", "MIT", "Copyright (C) 2020 Jorge Solla Rubiales",
     "libsigrokdecode4DSL/decoders/nrf905/pd.py", (0, 22)),
    ("mdio decoder", "BSD-2-Clause", "Copyright (C) 2016 Elias Oenal",
     "libsigrokdecode4DSL/decoders/mdio/pd.py", (0, 26)),
    ("cfp decoder", "BSD-2-Clause", "Copyright (C) 2018 Elias Oenal",
     "libsigrokdecode4DSL/decoders/cfp/pd.py", (0, 26)),
]
SPDX_TOKEN = r"[A-Za-z0-9.\-+]+"


def die(msg):
    sys.exit(f"third_party_notices: {msg}")


def run(*cmd):
    try:
        return subprocess.run(cmd, capture_output=True, text=True, check=False)
    except FileNotFoundError:
        die(f"{cmd[0]} not found; this script needs Homebrew and the Xcode command-line tools")


def uuids(path):
    """The set of LC_UUIDs of a Mach-O file or framework (one per architecture slice)."""
    if path.endswith(".framework"):
        name = os.path.basename(path)[:-len(".framework")]
        bins = sorted(glob.glob(os.path.join(path, "Versions/*", name)))
        bins = [b for b in bins if not os.path.islink(os.path.dirname(b))]
        if not bins:
            return set()
        path = bins[-1]
    return set(re.findall(r"^\s+uuid (\S+)", run("otool", "-arch", "all", "-l", path).stdout, re.M))


def bundled_files(app):
    """Library name -> path inside the bundle, for each library in Frameworks and PlugIns."""
    out = {}
    fw = os.path.join(app, "Contents/Frameworks")
    for p in sorted(os.listdir(fw)):
        out[p] = os.path.join(fw, p)
    for p in sorted(glob.glob(os.path.join(app, "Contents/PlugIns/*/*.dylib"))):
        out["PlugIns/" + os.path.relpath(p, os.path.join(app, "Contents/PlugIns"))] = p
    return out


def keg_of(cellar, name, path):
    """(formula, version) of the keg whose copy of this library has the same UUIDs."""
    base = os.path.basename(name)
    if base.endswith(".framework"):
        pats = [f"{cellar}/*/*/lib/{base}", f"{cellar}/*/*/Frameworks/{base}"]
    elif name.startswith("PlugIns/"):
        pats = [f"{cellar}/*/*/share/qt/plugins/{name[len('PlugIns/'):]}"]
    else:
        pats = [f"{cellar}/*/*/lib/{base}"]
    want = uuids(path)
    if not want:
        die(f"{name} has no LC_UUID; cannot trace it")
    found = {tuple(os.path.relpath(h, cellar).split(os.sep)[:2])
             for p in pats for h in glob.glob(p) if uuids(h) == want}
    if len(found) != 1:
        die(f"cannot trace {name} to exactly one Homebrew keg by UUID (matches: {sorted(found) or 'none'})")
    return found.pop()


def source_archive(formula):
    path = run("brew", "--cache", "-s", formula).stdout.strip()
    if not path or not os.path.exists(path):
        print(f"notices: fetching the {formula} source archive")
        r = run("brew", "fetch", "-s", formula)
        path = run("brew", "--cache", "-s", formula).stdout.strip()
        if r.returncode or not os.path.exists(path):
            die(f"cannot fetch the {formula} source archive: {r.stderr.strip()}")
    return path


def source_record(formula, version, keg, m):
    """(formula, version, url, sha256, recipe path, homebrew-core commit, patches JSON)
    for a library whose source is offered, checked against the recipe it was built with.

    Homebrew's metadata (url, checksum, patches, tap commit) describes its current
    recipe. The keg keeps the recipe its bottle was built from, which can differ in
    unrelated ways (bottle hashes, test blocks). Use the metadata only if the kept
    recipe builds from the same archive and applies exactly the listed patches."""
    stable = m["urls"]["stable"]
    recipe, tap_head = m.get("ruby_source_path", ""), m.get("tap_git_head", "")
    patches = m.get("patches", [])
    if not (stable.get("checksum") and recipe and tap_head):
        die(f"{formula}: Homebrew's metadata lacks the source checksum, recipe path or tap commit")
    kept_path = os.path.join(keg, ".brew", f"{formula}.rb")
    if not os.path.exists(kept_path):
        die(f"{formula}: keg {version} kept no recipe ({kept_path})")
    kept = open(kept_path, encoding="utf-8").read()
    head = kept.split("\n__END__")[0]
    # The archive's SHA-256 is its identity (the URL can name another mirror path).
    sha = re.search(r'^  sha256 "([0-9a-f]{64})"', head, re.M)
    if not (sha and sha.group(1) == stable["checksum"]):
        die(f"{formula}: keg {version} was built from a different source archive than "
            f"Homebrew's current recipe names; reinstall {formula} and package again")
    applied = [l for l in head.splitlines()
               if re.match(r"\s*patch\b", l) and ":DATA" not in l]
    if len(applied) != len(patches):
        die(f"{formula}: the recipe applies {len(applied)} patch(es), the metadata lists {len(patches)}")
    for p in patches:
        ref = p.get("file") or p.get("url") or ""
        if not ref or (ref not in kept and p.get("sha256", "-") not in kept):
            die(f"{formula}: patch {p} is not in the recipe")
    if any(re.search(r"\s", f) for f in (stable["url"], recipe, tap_head)):
        die(f"{formula}: unexpected whitespace in its source metadata")
    return (formula, version, stable["url"], stable["checksum"], recipe, tap_head,
            json.dumps(patches, separators=(",", ":")))


def read_from_archive(archive, wanted):
    """{relative path: bytes} for the members TOPDIR/<relative path> of a source archive."""
    out = {}
    if not wanted:
        return out
    with tarfile.open(archive) as tf:
        for m in tf:
            rel = m.name.split("/", 1)[1] if "/" in m.name else ""
            if m.isfile() and rel in wanted:
                out[rel] = tf.extractfile(m).read()
    missing = set(wanted) - set(out)
    if missing:
        die(f"{os.path.basename(archive)} lacks {sorted(missing)}")
    return out


def qt_attributions(cellar, formula, version, archive):
    """Attributions [(name, module, license, copyright)] for the shipped modules, and their license texts."""
    spdx = os.path.join(cellar, formula, version, "share/qt/sbom", f"{formula}-{version}.spdx")
    if not os.path.exists(spdx):
        die(f"{spdx} missing; cannot list Qt's third-party attributions")
    atts = []
    for block in re.split(r"^(?=PackageName: )", open(spdx, encoding="utf-8").read(), flags=re.M)[1:]:
        name = re.match(r"PackageName: (\S+)", block).group(1)
        if "_Attribution_" not in name:
            continue
        mod, what = name.split("_Attribution_", 1)
        if mod not in QT_MODULES:
            continue
        lic = re.search(r"^PackageLicenseConcluded: (.+)$", block, re.M).group(1).strip()
        cr = re.search(r"^PackageCopyrightText: (.*?)(?=^\w+: |\Z)", block, re.M | re.S)
        cr = re.sub(r"</?text>", "", cr.group(1)).strip() if cr else ""
        atts.append((what, mod, lic, cr))
    ids = sorted({i for _, _, lic, _ in atts for i in re.findall(SPDX_TOKEN, lic)
                  if i not in ("AND", "OR", "WITH", "LicenseRef-Qt-Commercial")})
    texts = read_from_archive(archive, [f"LICENSES/{i}.txt" for i in ids])
    return atts, {rel[len("LICENSES/"):-len(".txt")]: d for rel, d in texts.items()}


def python_incorporated(archive):
    """The incorporated-software part of CPython's Doc/license.rst."""
    rst = read_from_archive(archive, ["Doc/license.rst"])["Doc/license.rst"].decode("utf-8")
    i = rst.find("Licenses and Acknowledgements for Incorporated Software")
    if i < 0:
        die("CPython Doc/license.rst has no incorporated-software section")
    return rst[i:].encode()


class Texts:
    """License texts, each printed once and referred to by label."""

    def __init__(self):
        self.by_hash = {}

    def add(self, origin, data):
        key = hashlib.sha256(data).hexdigest()
        if key not in self.by_hash:
            self.by_hash[key] = (f"T{len(self.by_hash) + 1}", origin, data)
        return self.by_hash[key][0]


def main():
    app, src, out = (os.path.realpath(a) for a in sys.argv[1:4])
    global REPO
    REPO = os.environ["BRAND_REPO_URL"].rstrip("/")
    global SUPPORT
    SUPPORT = os.environ["BRAND_SUPPORT_EMAIL"]
    cellar = run("brew", "--cellar").stdout.strip()
    if not cellar:
        die("`brew --cellar` failed; this script needs Homebrew")
    kegs = {}
    for name, path in bundled_files(app).items():
        kegs.setdefault(keg_of(cellar, name, path), []).append(name)
    r = run("brew", "info", "--json=v2", *sorted({f for f, _ in kegs}))
    if r.returncode:
        die(f"brew info failed: {r.stderr.strip()}")
    meta = {f["name"]: f for f in json.loads(r.stdout)["formulae"]}
    texts = Texts()
    legal = os.path.join(src, "packaging/legal/texts")

    entries, sources, qt = [], [], []
    for (formula, version), names in sorted(kegs.items()):
        m = meta[formula]
        # Source URLs and checksums come from Homebrew's current formula, so they
        # describe the bundled keg only if the keg is that formula's current version.
        current = m["versions"]["stable"] + (f"_{m['revision']}" if m.get("revision") else "")
        if version != current:
            msg = (f"{formula}: the app bundles keg {version}, but Homebrew's formula is now "
                   f"{current}; run `brew upgrade {formula}`, rebuild and package again")
            if formula in COPYLEFT:
                die(msg + " (its source archive is offered, so it must match exactly)")
            print(f"notices: warning: {msg}; license texts are read from the {current} archive")
        archive = source_archive(formula)
        keg = os.path.join(cellar, formula, version)
        if formula in OVERRIDE:
            files = [os.path.join(legal, n) for n in OVERRIDE[formula][1]]
        else:
            files = [os.path.join(keg, n) for n in sorted(os.listdir(keg)) if n in LICENSE_NAMES]
            files += [os.path.join(legal, n) for n in FALLBACK_TEXTS.get(formula, [])]
        refs = []
        for p in files:
            rel = os.path.relpath(p, cellar) if p.startswith(cellar) else os.path.relpath(p, src)
            refs.append(texts.add(rel, open(p, "rb").read()))
        wanted = {(w, None) if isinstance(w, str) else w for w in FROM_SOURCE.get(formula, [])}
        got = read_from_archive(archive, [w for w, _ in wanted])
        for rel, nlines in sorted(wanted):
            data = got[rel]
            if nlines:
                data = b"".join(data.splitlines(keepends=True)[:nlines])
            refs.append(texts.add(f"{formula} {version} source: {rel}", data))
        if formula.startswith("python@"):
            refs.append(texts.add(f"CPython {version} Doc/license.rst, incorporated software",
                                  python_incorporated(archive)))
        if not refs:
            die(f"{formula} {version} has no license text; add one to FALLBACK_TEXTS")
        if formula in ("qtbase", "qtsvg"):
            atts, qtexts = qt_attributions(cellar, formula, version, archive)
            qt.append((formula, version, atts,
                       {i: texts.add(f"Qt {version} source: LICENSES/{i}.txt", d) for i, d in qtexts.items()}))
        lic = OVERRIDE[formula][0] if formula in OVERRIDE else m["license"]
        e = [f"{m.get('full_name', formula)} {version}", f"  License:  {lic}"]
        if formula in CHOICE:
            e.append(f"  Choice:   {CHOICE[formula]}")
        e += [f"  Homepage: {m['homepage']}",
              f"  Files:    {', '.join(names)}",
              f"  Text:     {', '.join(dict.fromkeys(refs))}"]
        entries.append(e)
        if formula in COPYLEFT:
            sources.append(source_record(formula, version, keg, m))

    tree = []
    for title, spdx, holders, path, span in IN_TREE:
        body = open(os.path.join(src, path), encoding="utf-8").read()
        if span:
            body = "\n".join(body.splitlines()[span[0]:span[1]]) + "\n"
        tree.append([title, f"  License:  {spdx}", f"  {holders}",
                     f"  Text:     {texts.add(path, body.encode())}"])

    L = [
        "Logic Analyze: third-party notices",
        "",
        "Logic Analyze is free software under the GNU General Public License, version 3 or",
        "later (licenses/GPL-3.0.txt). It is based on DSView by DreamSourceLab, which is",
        "based on PulseView and the sigrok project. The DSLogic device firmware is",
        "Copyright DreamSourceLab under the MIT License (licenses/DreamSourceLab-firmware-MIT.txt).",
        f"Source: {REPO}",
        "",
        "Section 1 lists the other software included in the app and its license terms.",
        "Section 2 says where to get the source of the LGPL and GPL libraries, and how to",
        "replace the LGPL ones. Section 3 has the full license texts; a text shared by",
        "several components appears once. The LGPL-3.0 is a set of additional permissions",
        "on top of the GPL-3.0, whose text is in licenses/GPL-3.0.txt.",
        "",
        "=" * 78, "1. Components", "=" * 78,
    ]
    for e in entries + tree:
        L += [""] + e
    for formula, version, atts, lic_refs in qt:
        L += ["", "-" * 78, f"Third-party code inside {formula} {version}, from Qt's own SBOM, for the",
              "modules and plugins this app ships", "-" * 78]
        for what, mod, lic, cr in atts:
            refs = [lic_refs[i] for i in re.findall(SPDX_TOKEN, lic) if i in lic_refs]
            L += ["", f"{what} (in Qt {mod})", f"  License:  {lic}"]
            L += [f"  {line.strip()}" for line in cr.splitlines() if line.strip()]
            if refs:
                L.append(f"  Text:     {', '.join(dict.fromkeys(refs))}")

    L += ["", "=" * 78, "2. Source code, and replacing the LGPL libraries", "=" * 78, "",
          "These libraries were built by Homebrew from the source archives below, with the",
          "patches and build options in each library's Homebrew build recipe (linked below at",
          "the homebrew-core revision used). Each release also publishes the archives, the",
          f"exact recipes and their patches at {REPO}/releases,",
          "and Lanscapes will provide them on request for at least three years after each",
          f"release: write to {SUPPORT}.", ""]
    for formula, version, url, sha, recipe, tap_head, patches in sources:
        L += [f"{formula} {version}", f"  {url}", f"  SHA-256 {sha}"]
        if recipe and tap_head:
            L.append(f"  Build recipe: https://github.com/Homebrew/homebrew-core/blob/{tap_head}/{recipe}")
        L.append("")
    if os.environ.get("NOTICES_SOURCES_OUT"):
        # One line per archive for the release job, fields separated by US (0x1f) so
        # that none can be empty-collapsed: formula, keg version, url, sha256, recipe
        # path in homebrew-core, homebrew-core commit, patches (JSON).
        with open(os.environ["NOTICES_SOURCES_OUT"], "w", encoding="utf-8") as fh:
            fh.writelines("\x1f".join(row) + "\n" for row in sources)
    L += ["The LGPL libraries (Qt, glib, libusb, libintl, graphite2) are linked dynamically.",
          "You can replace them with modified versions built from the sources above:",
          "  1. Copy Logic Analyze.app to a folder you can write to, such as your home folder.",
          "  2. Replace the library in Contents/Frameworks, or the Qt plugin in",
          "     Contents/PlugIns, with your build, keeping its file name.",
          "  3. Make your build load its dependencies from the app, as the copy it replaces",
          "     does: compare `otool -L` of the two, and change each path that points outside",
          "     the app with `install_name_tool -change OLD NEW`, and the library's own name",
          "     with `install_name_tool -id`. (If your build loads a second copy of Qt from",
          "     elsewhere, Qt's plugins fail and the app's icons disappear.)",
          "  4. Sign the copy with an ad-hoc signature:",
          "       codesign --force --deep --sign - \"Logic Analyze.app\"",
          "  5. Open the copy. macOS runs an ad-hoc signed app that you modified on your own Mac.",
          "In the Mac App Store edition, the purchase check is tied to Apple's signature, so a",
          "re-signed copy runs the free (GPL) features only."]

    L += ["", "=" * 78, "3. License texts", "=" * 78]
    for label, origin, data in texts.by_hash.values():
        L += ["", f"--- {label} ({origin}) " + "-" * max(4, 70 - len(label) - len(origin)), "",
              data.decode("utf-8", errors="replace").rstrip()]

    with open(out, "w", encoding="utf-8") as fh:
        fh.write("\n".join(L) + "\n")
    print(f"notices: {sum(len(n) for n in kegs.values())} bundled files from {len(kegs)} kegs, "
          f"{sum(len(a) for _, _, a, _ in qt)} Qt attributions, {len(IN_TREE)} in-tree components, "
          f"{len(texts.by_hash)} license texts")


if __name__ == "__main__":
    main()
