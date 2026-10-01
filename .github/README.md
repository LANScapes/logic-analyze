# Logic Analyze

Logic Analyze is a macOS app for DreamSourceLab DSLogic logic analyzers, published by Lanscapes. It is a downstream build of [DSView](https://github.com/DreamSourceLab/DSView) by DreamSourceLab, which is based on PulseView and the sigrok project. It is free software under the GNU General Public License, version 3 or later.

Lanscapes is not affiliated with DreamSourceLab. DSLogic is DreamSourceLab's product; it is named here only to say which hardware the app works with.

## What is different from DSView

- A native macOS window, and a toolbar you can drag to any side of the window, so 16 channels get the full height.
- One set of interface icons, and the Display menu moved under Options.
- A self-contained, signed app bundle with its own Python, and third-party notices traced to the exact library builds it ships.
- `dslcap`, a command-line tool for headless captures (`tools/dslcap`).

The branding and interface changes (name, icons, toolbar, Options menu) sit behind the `LANSCAPES_BRAND` CMake option; without it, this tree builds DSView. A plain build still differs from upstream DSView in a few shared places: the macOS window and path-length fixes, and the extra `dslcap` target. Those are offered back to DSView as pull requests from the [LANScapes/DSView](https://github.com/LANScapes/DSView) fork.

## Building

On macOS with Homebrew:

```sh
.github/scripts/brew-deps.sh
.github/scripts/python-pin.sh >/dev/null &&   # the app embeds Homebrew's Python 3.14
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_POLICY_VERSION_MINIMUM=3.5 -DLANSCAPES_BRAND=ON \
  $(.github/scripts/python-pin.sh)
cmake --build build -j
packaging/macos/package.sh          # dist/Logic Analyze.app, ad-hoc signed
```

`package.sh --sign "Developer ID Application: ..." --notarize PROFILE` signs and notarizes it. The app requires the macOS version it was built on or later (Homebrew's libraries are built for the host); set `MIN_MACOS` when packaging to state it.

## Keeping up with DSView

The [Upstream sync](workflows/upstream-sync.yml) workflow checks DSView every week. When DSView has new commits, it merges them into a `sync/upstream-<commit>` branch, builds it, and opens a pull request. If the merge conflicts, it opens an issue instead. To let it open pull requests, enable *Allow GitHub Actions to create and approve pull requests* under Settings › Actions › General; without that, it opens an issue with a link to the branch.

## Releases

Pushing a `vX.Y.Z` tag builds the app and drafts a release with it and, for each bundled LGPL and GPL library, its source archive plus the Homebrew recipe and patches it was built with. A person reviews and publishes the draft. See [release.yml](workflows/release.yml) for the signing secrets.

## Support

la_support@lanscapes.net. Bugs and requests: [issues](https://github.com/LANScapes/logic-analyze/issues).

DSView's own README is [README.md](../README.md).
