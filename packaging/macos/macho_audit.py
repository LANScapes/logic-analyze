#!/usr/bin/env python3
"""Scrub and audit every Mach-O file in a macOS app bundle.

    macho_audit.py scrub APP     remove search paths and IDs that point outside the bundle
    macho_audit.py audit APP MIN fail unless every dependency resolves inside the bundle
                                 (or to the OS), and every slice targets macOS <= MIN

Dependencies are resolved the way dyld does: @executable_path from the app's
main executables, @loader_path from the loading file, @rpath through the
loading file's LC_RPATHs and those of the executables that load it.
"""
import os
import re
import subprocess
import sys

SYSTEM = ("/System/", "/usr/lib/")


def is_system(p):
    """System paths only after normalization: /usr/lib/../../tmp is not system."""
    return p.startswith("/") and os.path.normpath(p).startswith(SYSTEM)


def run(*cmd):
    return subprocess.run(cmd, capture_output=True, text=True, check=False)


def macho_files(app):
    out = []
    for root, _, files in os.walk(app):
        for name in files:
            p = os.path.join(root, name)
            if os.path.islink(p):
                continue
            if "Mach-O" in run("file", "-b", p).stdout:
                out.append(p)
    return out


def load_commands(path):
    """Per-slice load commands: list of dicts {arch, loads, rpaths, id, minos}."""
    text = run("otool", "-arch", "all", "-l", path).stdout
    slices, cur = [], None
    blocks = re.split(r"^(?=Load command \d+)", text, flags=re.M)
    for block in blocks:
        head = re.search(r"\(architecture (\S+)\):", block)
        if head or cur is None:
            cur = {"arch": head.group(1) if head else "thin", "loads": [], "rpaths": [],
                   "id": None, "minos": []}
            slices.append(cur)
        cmd = re.search(r"^\s+cmd (LC_\w+)", block, flags=re.M)
        if not cmd:
            continue
        cmd = cmd.group(1)
        name = re.search(r"^\s+name (.+?) \(offset \d+\)\s*$", block, flags=re.M)
        path = re.search(r"^\s+path (.+?) \(offset \d+\)\s*$", block, flags=re.M)
        if cmd in ("LC_LOAD_DYLIB", "LC_LOAD_WEAK_DYLIB", "LC_REEXPORT_DYLIB", "LC_LOAD_UPWARD_DYLIB"):
            cur["loads"].append((name.group(1), cmd == "LC_LOAD_WEAK_DYLIB"))
        elif cmd == "LC_RPATH":
            cur["rpaths"].append(path.group(1))
        elif cmd == "LC_ID_DYLIB":
            cur["id"] = name.group(1)
        elif cmd == "LC_BUILD_VERSION":
            m = re.search(r"^\s+minos (\S+)", block, flags=re.M)
            if m:
                cur["minos"].append(m.group(1))
        elif cmd == "LC_VERSION_MIN_MACOSX":
            m = re.search(r"^\s+version (\S+)", block, flags=re.M)
            if m:
                cur["minos"].append(m.group(1))
    return [s for s in slices if s["loads"] or s["rpaths"] or s["id"] or s["minos"]]


def inside(app, p):
    real = os.path.realpath(p)
    return real == app or real.startswith(app + os.sep)


def expand(token_path, loader, exe_dirs):
    if token_path.startswith("@loader_path"):
        return [os.path.normpath(os.path.dirname(loader) + token_path[len("@loader_path"):])]
    if token_path.startswith("@executable_path"):
        return [os.path.normpath(d + token_path[len("@executable_path"):]) for d in exe_dirs]
    return [token_path]


def version_tuple(v):
    return tuple(int(x) for x in v.split("."))


def main():
    mode, app = sys.argv[1], os.path.realpath(sys.argv[2])
    exes = [os.path.join(app, "Contents/MacOS", f) for f in os.listdir(os.path.join(app, "Contents/MacOS"))]
    exe_dirs = [os.path.join(app, "Contents/MacOS")]
    files = macho_files(app)
    errors = []

    if mode == "scrub":
        for f in files:
            os.chmod(f, os.stat(f).st_mode | 0o200)
            seen = set()
            for s in load_commands(f):
                for rp in s["rpaths"]:
                    if rp in seen:
                        continue  # one deletion removes the path from every slice
                    seen.add(rp)
                    targets = expand(rp, f, exe_dirs)
                    if all(is_system(t) for t in targets):
                        continue
                    if not all(inside(app, t) for t in targets):
                        r = run("install_name_tool", "-delete_rpath", rp, f)
                        if r.returncode:
                            errors.append(f"{f}: cannot delete rpath {rp}: {r.stderr.strip()}")
                lid = s["id"]
                if lid and lid.startswith("/") and not is_system(lid):
                    rel = os.path.relpath(f, os.path.join(app, "Contents/Frameworks"))
                    r = run("install_name_tool", "-id", "@rpath/" + rel, f)
                    if r.returncode:
                        errors.append(f"{f}: cannot rewrite id: {r.stderr.strip()}")
    elif mode == "audit":
        minimum = version_tuple(sys.argv[3])
        exe_rpaths = []
        for e in exes:
            for s in load_commands(e):
                for rp in s["rpaths"]:
                    exe_rpaths += expand(rp, e, exe_dirs)
        for f in files:
            for s in load_commands(f):
                own = []
                for rp in s["rpaths"]:
                    for t in expand(rp, f, exe_dirs):
                        if not is_system(t) and not inside(app, t):
                            errors.append(f"{f} [{s['arch']}]: rpath leaves the bundle: {rp}")
                        own.append(t)
                for v in s["minos"]:
                    if version_tuple(v) > minimum:
                        errors.append(f"{f} [{s['arch']}]: built for macOS {v}, above {sys.argv[3]}")
                for dep, weak in s["loads"]:
                    if is_system(dep):
                        continue
                    if dep.startswith("@rpath/"):
                        cands = [os.path.join(d, dep[len("@rpath/"):]) for d in own + exe_rpaths]
                    else:
                        cands = expand(dep, f, exe_dirs)
                    found = [c for c in cands if os.path.exists(c)]
                    if not found:
                        if not weak:
                            errors.append(f"{f} [{s['arch']}]: unresolved dependency {dep}")
                        elif any(not is_system(c) and not inside(app, c) for c in cands):
                            # A missing weak import would load from wherever it later appears.
                            errors.append(f"{f} [{s['arch']}]: weak dependency {dep} could load from outside the bundle")
                        continue
                    if not inside(app, found[0]):
                        errors.append(f"{f} [{s['arch']}]: {dep} resolves outside the bundle: {found[0]}")
        for root, dirs, names in os.walk(app):
            for n in dirs + names:
                p = os.path.join(root, n)
                if os.path.islink(p) and not inside(app, p):
                    errors.append(f"symlink leaves the bundle: {p} -> {os.readlink(p)}")
    else:
        sys.exit(f"unknown mode {mode}")

    for e in errors:
        print(e)
    if errors:
        print(f"FAIL: {len(errors)} problem(s) in {len(files)} Mach-O files")
        sys.exit(1)
    print(f"{mode} clean: {len(files)} Mach-O files")


if __name__ == "__main__":
    main()
