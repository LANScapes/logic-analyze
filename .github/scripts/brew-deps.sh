#!/bin/bash
# Homebrew packages that the build and packaging use.
set -euo pipefail
export HOMEBREW_NO_AUTO_UPDATE=1 HOMEBREW_NO_INSTALL_CLEANUP=1
brew update --quiet
# The runner image links openssl@1.1 into /opt/homebrew/bin; it blocks linking openssl@3.
if brew list --versions openssl@1.1 >/dev/null 2>&1; then brew unlink openssl@1.1; fi
# --overwrite: the runner image ships some of these (e.g. openssl) outside Homebrew.
brew install --quiet --overwrite cmake pkgconf qtbase qtsvg boost fftw libusb glib python@3.14
brew list --versions qtbase qtsvg boost fftw libusb glib python@3.14
