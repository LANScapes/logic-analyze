#!/bin/bash
# Homebrew packages that the build and packaging use.
set -euo pipefail
export HOMEBREW_NO_AUTO_UPDATE=1 HOMEBREW_NO_INSTALL_CLEANUP=1
brew update --quiet
# --overwrite: the runner image ships some of these (e.g. openssl) outside Homebrew.
brew install --quiet --overwrite cmake pkgconf qtbase qtsvg boost fftw libusb glib python@3.14
brew list --versions qtbase qtsvg boost fftw libusb glib python@3.14
