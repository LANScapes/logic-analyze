#!/bin/bash
# Print the CMake arguments that pin the embedded Python to Homebrew's python@3.14,
# the version packaging/macos/package.sh bundles. FindPython3 uses these exact
# artifacts instead of searching, so the interpreter, headers and library agree.
set -euo pipefail
v=${1:-3.14}
fw="$(brew --prefix "python@$v")/Frameworks/Python.framework/Versions/$v"
for f in "$fw/bin/python$v" "$fw/include/python$v/Python.h" "$fw/lib/libpython$v.dylib"; do
  [ -e "$f" ] || { echo "python-pin: missing $f" >&2; exit 1; }
done
echo "-DPython3_EXECUTABLE=$fw/bin/python$v -DPython3_INCLUDE_DIR=$fw/include/python$v -DPython3_LIBRARY=$fw/lib/libpython$v.dylib"
