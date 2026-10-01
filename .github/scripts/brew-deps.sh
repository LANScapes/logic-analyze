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
# Bring every library the app bundles up to Homebrew's current version: the
# third-party notices offer the current source archives and refuse a bundled
# LGPL/GPL keg that is older. `brew install` leaves adequate older dependencies
# alone, so upgrade them explicitly. A link conflict is again only a warning.
deps=$(brew deps --union "${FORMULAE[@]}")
# shellcheck disable=SC2086 # one formula name per word
if ! brew upgrade --quiet --overwrite $deps; then
  echo "::warning::brew upgrade reported a problem (usually a link conflict)"
fi
# The runner image installed some of these long ago; a keg can be Homebrew's
# current version yet keep an older copy of its recipe. The notices offer source
# only for kegs built from the current recipe, so reinstall any that are not.
for f in $deps "${FORMULAE[@]}"; do
  keg="$(brew --prefix "$f")"
  kept="$keg/.brew/${f##*/}.rb"
  [ -f "$kept" ] || continue
  if ! cmp -s <(brew cat "$f" 2>/dev/null) "$kept"; then
    echo "reinstalling $f: its keg was built from an older recipe"
    brew reinstall --quiet "$f" || echo "::warning::brew reinstall $f reported a problem"
  fi
done
