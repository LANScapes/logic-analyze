#!/bin/bash
# collect-sources.sh SOURCES.TSV OUTDIR
# For each LGPL/GPL library that third_party_notices.py listed (NOTICES_SOURCES_OUT),
# put in OUTDIR:
#   <formula>-<version>-<archive name>          the upstream source archive (SHA-256 checked)
#   <formula>-<version>-homebrew-recipe.tar.gz  the recipe and install receipt from the
#                                               bundled keg, plus every patch the recipe applies
set -euo pipefail
tsv=$1
out=$2
mkdir -p "$out"
cellar=$(brew --cellar)
work=$(mktemp -d)
trap 'rm -rf "$work"' EXIT

sha_of() { shasum -a 256 "$1" | cut -d' ' -f1; }

while IFS=$'\t' read -r formula version url sha recipe tap_head patches; do
  [ -n "$sha" ] || { echo "::error::$formula: no source checksum"; exit 1; }
  keg="$cellar/$formula/$version"
  [ -d "$keg" ] || { echo "::error::$formula: keg $keg is not installed"; exit 1; }

  # Upstream source archive.
  path=$(brew --cache -s "$formula")
  [ -f "$path" ] || brew fetch -s "$formula" >/dev/null
  got=$(sha_of "$path")
  if [ "$got" != "$sha" ]; then
    echo "::error::$formula archive SHA-256 $got does not match the notices ($sha)"; exit 1
  fi
  cp "$path" "$out/$formula-$version-$(basename "$url")"

  # The recipe and receipt of the bundled keg, and the patches it applies.
  d="$work/$formula-$version-homebrew-recipe"
  mkdir -p "$d/patches"
  cp "$keg"/.brew/*.rb "$d/"
  cp "$keg/INSTALL_RECEIPT.json" "$d/"
  n=0
  while IFS=$'\t' read -r kind ref psha; do
    [ -n "$kind" ] || continue
    n=$((n + 1))
    if [ "$kind" = file ]; then
      # A patch kept in homebrew-core itself, at the revision the recipe came from.
      curl -fsSL --retry 3 -o "$d/patches/$(basename "$ref")" \
        "https://raw.githubusercontent.com/Homebrew/homebrew-core/$tap_head/$ref"
    else
      f="$d/patches/$n-$(basename "${ref%%\?*}")"
      curl -fsSL --retry 3 -o "$f" "$ref"
      if [ -n "$psha" ] && [ "$(sha_of "$f")" != "$psha" ]; then
        echo "::error::$formula patch $ref does not match its recipe checksum"; exit 1
      fi
    fi
  done < <(python3 -c '
import json, sys
for p in json.loads(sys.argv[1]):
    if "file" in p:
        print("file", p["file"], "", sep="\t")
    elif "url" in p:
        print("url", p["url"], p.get("sha256", ""), sep="\t")
' "$patches")
  {
    echo "$formula $version, as bundled in Logic Analyze."
    echo
    echo "Homebrew built it from $url"
    echo "(SHA-256 $sha) with the recipe in this directory:"
    echo "  https://github.com/Homebrew/homebrew-core/blob/$tap_head/$recipe"
    echo "patches/ holds the $n patch(es) the recipe applies; any inline patch is at the end"
    echo "of the recipe itself (after __END__). INSTALL_RECEIPT.json records how the keg was"
    echo "built or poured. Homebrew's build documentation: https://docs.brew.sh/Formula-Cookbook"
  } > "$d/README.txt"
  tar -C "$work" -czf "$out/$formula-$version-homebrew-recipe.tar.gz" "$(basename "$d")"
done < "$tsv"
ls -l "$out"
