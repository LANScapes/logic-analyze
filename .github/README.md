# Logic Analyze

Logic Analyze is a macOS app for DreamSourceLab DSLogic logic analyzers. LANScapes publishes it.

- **Base.** It is a downstream build of [DSView](https://github.com/DreamSourceLab/DSView) by DreamSourceLab, which is based on PulseView and the sigrok project.
- **License.** GNU General Public License, version 3 or later.
- **Affiliation.** LANScapes is not affiliated with DreamSourceLab. DSLogic is a DreamSourceLab product. This README names it only to identify the hardware that the app supports.

## Differences from DSView

- The app uses a native macOS window.
- You can move the toolbar to any side of the window. With the toolbar on a side, 16 channels use the full window height.
- The app has one set of interface icons. The Display menu is under Options.
- The app bundle is signed and contains its own Python. The third-party notices identify the exact library builds in the bundle.
- `dslcap` (`tools/dslcap`) is a command-line tool for captures without the user interface.

The `LANSCAPES_BRAND` CMake option controls the name, icons, toolbar and Options menu. Without this option, the tree builds DSView.

A build without the option is not identical to DSView. It also contains:

- the macOS window fixes,
- the path-length fixes,
- the `dslcap` target.

The [LANScapes/DSView](https://github.com/LANScapes/DSView) fork sends these changes to DSView as pull requests.

## Build the app

You need macOS and Homebrew.

1. Install the dependencies:
   ```sh
   .github/scripts/brew-deps.sh
   ```
2. Make sure that Homebrew Python 3.14 is installed. The app contains this Python. This command fails if it is not installed:
   ```sh
   .github/scripts/python-pin.sh
   ```
3. Configure the build. Use `bash`, because `zsh` does not split the Python arguments:
   ```sh
   bash -c 'cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_POLICY_VERSION_MINIMUM=3.5 -DLANSCAPES_BRAND=ON $(.github/scripts/python-pin.sh)'
   ```
4. Compile:
   ```sh
   cmake --build build -j
   ```
5. Make the app bundle. The result is `dist/Logic Analyze.app`, with an ad-hoc signature:
   ```sh
   packaging/macos/package.sh
   ```

To sign and notarize the app, add `--sign "Developer ID Application: ..." --notarize PROFILE`.

**NOTE:** The app runs only on the macOS version that built it, or later. Homebrew builds its libraries for that version. To record the version in the app, set `MIN_MACOS` when you run `package.sh`.

## Upstream updates

The [Upstream sync](workflows/upstream-sync.yml) workflow examines DSView every week. If DSView has new commits:

1. The workflow merges them into a `sync/upstream-<commit>` branch.
2. The workflow builds that branch.
3. The workflow opens a pull request. If the merge has a conflict, it opens an issue.

To let the workflow open pull requests:

1. Go to Settings › Actions › General.
2. Enable *Allow GitHub Actions to create and approve pull requests*.

Without this setting, the workflow opens an issue with a link to the branch.

## Releases

Push a `vX.Y.Z` tag. The [Release](workflows/release.yml) workflow then builds the app and drafts a release. The draft contains:

- the app,
- the source archive of each LGPL and GPL library in the app,
- the Homebrew recipe and patches for each of those libraries.

A person examines the draft and publishes it.

**CAUTION:** If the repository does not have the signing secrets, the workflow drafts an app with an ad-hoc signature only. Gatekeeper on other Macs will not open that app. Do not publish that draft. Delete it, or cancel the workflow and make the release on a signing Mac. The signing secrets are listed in [release.yml](workflows/release.yml).

## Support

- Email: la_support@lanscapes.net
- Bugs and requests: [issues](https://github.com/LANScapes/logic-analyze/issues)
- DSView README: [README.md](../README.md)
