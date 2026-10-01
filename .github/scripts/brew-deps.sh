#!/bin/bash
# Homebrew packages that the build and packaging use.
set -euo pipefail
export HOMEBREW_NO_AUTO_UPDATE=1 HOMEBREW_NO_INSTALL_CLEANUP=1
FORMULAE=(cmake pkgconf qtbase qtsvg boost fftw libusb glib python@3.14)
brew update --quiet
# The runner image has files of its own in /opt/homebrew (for example an
# openssl symlink to openssl@1.1) that make `brew link` fail for upgraded
# dependencies. The build uses Homebrew's opt/ paths, not the links, so a link
# conflict is only a warning; a formula that did not install is an error.
if ! brew install --quiet --overwrite "${FORMULAE[@]}"; then
  echo "::warning::brew install reported a problem (usually a link conflict); checking each formula"
fi
for f in "${FORMULAE[@]}"; do
  brew list --versions "$f" || { echo "::error::$f is not installed"; exit 1; }
  [ -d "$(brew --prefix "$f")" ] || { echo "::error::$f has no opt/ prefix"; exit 1; }
done
