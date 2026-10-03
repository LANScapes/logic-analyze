# dslcap

`dslcap` captures DSLogic samples without the graphical interface using libsigrok4DSL. It prints one
JSON result on stdout. Library diagnostics use stderr. A successful capture
creates `<base>.bin`; an existing file at that path is never replaced.

```text
dslcap --list [--res DIR] [--parent-fd N] [--res-manifest FD] [--log-level N]
dslcap --channels 0,1 --samplerate 10000000 --samples 1000000
       [--vth 1.6] [--mode buffer|stream] [--trigger CH[:R|F|C|1|0]]
       [--trigpos PERCENT] [--timeout SEC] [--res DIR]
       [--parent-fd N] [--res-manifest FD] [--log-level N]
       [--device LOCATION:SERIAL]
       --out /path/base
```

## Guarded device selection (capture unavailable)

`--device LOCATION:SERIAL` reserves an exact-identity capture interface, but
**this build cannot perform captures with it**. A syntactically valid request
returns exit status **2** and one JSON object:

```json
{"error":"exact-device capture is unavailable: driver scans can upload firmware before an exclusive claim","code":"device_selection_unavailable","option":"--device","value":"loc-20121500:ABC123"}
```

The canonical macOS location format agreed with the separate registry-only
`--list-ids` work is `loc-<8 lowercase hexadecimal locationID digits>`, for
example `loc-20120000`. All eight digits are required, including leading zeros;
uppercase, signs, `0x`, whitespace and over-width/overflow values are rejected.
The format represents a raw unsigned 32-bit cached IORegistry `locationID`,
without rounding or masking. Decimal `538055936` is `0x20121500`, hence
`loc-20121500`; `loc-20120000` represents decimal `538050560`. This conversion
is arithmetic only, not an observation of a connected device. An unavailable
cached location or serial is unselectable, not a substitute zero/empty identity.

The earlier `usb-<decimal bus>-<dot-separated decimal port path>` syntax remains
accepted for compatibility, for example `usb-1-2.3`. Bus is 0..255; the path
contains one to seven ports, each 1..255, with no leading zeros. Neither syntax
enables capture in this build. The first colon separates location from the
nonempty UTF-8 serial. Subsequent colons, spaces, Unicode and case are preserved
verbatim; there is no trimming or normalization. A null or unavailable location
or serial in identity-list JSON supplies no selectable identity. Syntax
validation does not verify that a device exists or that its serial matches.
USB topology is not a persistent identity across detach or re-enumeration.

Malformed identities, missing values, repeated `--device`, or combining it
with `--list` also return JSON and exit 2. Invalid UTF-8 is omitted from the
JSON `value`. Ordinary capture arguments, including `--out`, remain required.
After argument validation and optional parent-watch setup, the capability gate
runs before resource lookup, manifest reads, every `ds_*` call, and all USB
access. **The guarded path makes zero libusb calls and zero USB descriptor
requests**; it does not enumerate, open, claim, detach, configure, reset,
upload firmware/FPGA data, start hotplug handling or create capture files.
There is no fallback to name, address, first device or the legacy capture path.
Missing/ambiguous identity, changed/unreadable serial, claim busy/failure,
detach, and same-identity re-enumeration therefore cannot result in a guarded
capture or mutation: every request is rejected before those conditions are
inspected. They are not individually detected or given distinct errors yet.
The existing `--parent-fd` parent-loss behavior still exits immediately with
status 1 and no additional JSON; it can precede delivery of the guard result.

### Architectural blocker traced before implementation

The existing call graph is:

```text
dslcap main
  ds_lib_init                          libsigrok4DSL/lib_main.c
    process_attach_event(0)
      hardware driver scan(NULL)       hardware/DSL/{dslogic,dscope}.c
        dsl_check_conf_profile         hardware/DSL/dsl.c
        ezusb_upload_firmware          hardware/common/ezusb.c
          libusb_open (fresh handle)
          kernel-driver detach (non-macOS, if active)
          libusb_set_configuration
          ezusb_reset / install firmware / ezusb_reset
          libusb_close
    register hotplug + start USB hotplug thread
  pick_device (first name containing DSLogic)
    ds_active_device / open_device_instance
      driver dev_open / dsl_dev_open   hardware/DSL/dsl.c
        hw_dev_open (fresh open; vendor firmware-version read)
        libusb_claim_interface (only here)
        hardware status / optional dsl_fpga_config / device configuration
```

Both firmware-loader branches (manifest-verified and legacy) mutate without
an interface claim. Resource verification authenticates bytes but does not
establish device ownership. `dsl_check_conf_profile` reads manufacturer/product
strings rather than a serial, before any claim. DSLogic/DSCope scan accepts only
the optional `SR_CONF_CONN` bus/address filter, and initialization passes no
filter. The runtime `hw_dev_open` firmware-version read also calls
`command_ctl_rd` before claiming; that helper sends a vendor OUT
`CMD_CTL_RD_PRE` command before its IN response. The public `ds_get_device_list`
result contains only a handle and name;
there is no API to initialize without hardware scans or to adopt an exact,
retained, already-claimed libusb handle. Later hotplug attach processing calls
the same scans again; firmware upload intentionally waits for reconnect.
In `lib_main.c`, `hotplug_event_listen_callback` can call `update_device_handle`
while waiting for reconnect; it substitutes the newly attached device object
for the old one and schedules reopening without checking a serial. That is
incompatible with treating every re-enumeration as a new, forbidden device.

A descriptor-only candidate list followed by a CLI claim cannot make this safe:
the existing library would still scan unrelated devices and its loaders would
open different handles. Releasing a CLI claim so the driver could reopen would
also lose the ownership guarantee. A runtime-only shortcut would still expose
other bootloader devices to initialization and hotplug scans. This change stops
at the rejecting guard; it changes no driver or firmware loader. Ordinary
no-flag capture/list behavior is unchanged. One explicitly scoped parser safety
change prevents an incomplete earlier option from swallowing the selector as
its value (for example, `--out --device`): values equal to `--device` or starting
with `--device=` are rejected before any scan. Use `./--device` for a literal
relative path with that name.
In particular, legacy `--list` still invokes scanning and may mutate hardware;
it must not be used as safe identity discovery.

Enabling exact capture requires a separately reviewed driver lifecycle change:
scan-free initialization, descriptor-only exact selection with nonempty serial
revalidation, retention of the original device object and one exclusive claimed
handle before every mutation, loaders that use that handle with no detach or
force takeover, and disabling reconnect/attach scanning for the guarded session.
Missing or ambiguous matches, changed serial, claim failure, detach and any new
device object after re-enumeration must terminate it with JSON exit 2 or 3.
Successful exact-device capture and those runtime checks remain unimplemented.

### Proposed macOS lifecycle changes (design only)

Source review identifies the following minimum work for a future selected
session. **None of these driver changes is implemented by this PR.** The input
must be one retained IOKit-selected device entry, its entry identity/generation,
raw `locationID` and nonempty verbatim serial. Cached listing properties cannot
prove exclusive ownership or that the object survived selection/open/claim.
The adapter must bind that exact entry to one live transport object, revalidate
identity before mutation, and reject missing/ambiguous/changed entries; no
bus/address, name, first-device or same-location replacement fallback.

| Source / exact functions | Required selected-session change |
| --- | --- |
| `backend.c`: `sr_init`, `sr_exit`; `lib_main.c`: `ds_lib_init`, `ds_lib_exit`, `ds_reload_device_list`, `ds_active_device`, `open_device_instance` | Factor an explicit scan-free initialization/adoption path from the supplied selected object; do not call driver scans, reload, demo selection or the legacy `pick_device`. Current `sr_init` unconditionally calls `libusb_init`; proving exact Darwin handle adoption or introducing a selected IOKit transport adapter is the first feasibility gate. This checkout provides neither adapter nor a public IOKit-to-libusb binding; do not assume a wrapping API works on macOS. Preserve legacy initialization separately. |
| `hardware/DSL/dslogic.c`, `dscope.c`: `scan`, `DSLogic_dev_new`, `DSCope_dev_new`; `dsl.c`: `dsl_check_conf_profile`, `hw_dev_open`, `dsl_dev_open` | Extract profile/instance construction from scanning. Open only the retained selected object and claim interface 0 once, with no detach, auto-detach, force takeover or retry on another object. Move the claim before the firmware-version `command_ctl_rd` (which sends vendor OUT preparation), FPGA load and configuration; adopt the existing handle rather than reopen it. |
| `libsigrok-internal.h`: `sr_usb_dev_inst`; `dsdevice.c`: `sr_usb_dev_inst_new`, `sr_usb_dev_inst_free`; `dsl.c`: `dsl_dev_close`, `dsl_destroy_device` | Store original object identity, one retained handle, claim ownership and a synchronized terminal-detach latch. Release/close exactly once after outstanding callbacks drain; never overwrite `usb_dev`/`devhdl` or send cleanup commands after detach. |
| `hardware/common/ezusb.c`: `ezusb_upload_firmware`, `ezusb_upload_verified`, `ezusb_reset`, `ezusb_install_firmware`, `ezusb_install_buffer` | Both verified and legacy upload paths borrow the same owned, claimed session handle. Remove selected-path fresh open/close/detach; check ownership before configuration, every CPU-reset request and every firmware chunk. If a bootloader cannot be claimed without first changing configuration, reject it. Any reset/upload that causes re-enumeration ends this session; never finish capture by following the replacement. |
| `hardware/DSL/command.c`: `command_ctl_wr`, `command_ctl_rd`; `dsl.c`: `dsl_fpga_config`, `dsl_fpga_arm`, `dsl_config_set`, `dsl_start_transfers`, `free_transfer`, `dsl_dev_acquisition_stop`; both driver files: `config_set`, `dev_open`, `dev_acquisition_start` | Pass the owned session through all transfer paths and check its claimed/live state. Every firmware/FPGA/config/register/threshold/start/stop transfer uses that one handle, including direct bulk writes and asynchronous submission/resubmission; a check only in `dev_open` is insufficient. Serialize termination against transfer submission. |
| `lib_main.c`: `process_attach_event`, `hotplug_event_listen_callback`, `update_device_handle`, `usb_hotplug_process_proc` | Disable attach/reload/reopen transactions for selected sessions. Selected-device removal or transfer `NO_DEVICE` must latch terminal failure and produce JSON exit 2/3. A removal-only IOKit notification may terminate the original session; no attach handler may follow a new entry, even if location and serial are identical. |

**Estimate and gates:** for one experienced C/macOS USB engineer, allow roughly
**10–15 engineer-days (2–3 work weeks)** for a first supported DSLogic runtime
profile: 2–3 days for the adapter/ownership feasibility spike, 3–5 for library
and transfer refactoring, and the remainder for mocks, review and controlled
hardware verification. This assumes a supported way to retain/claim the exact
IOKit-selected transport, available hardware with known serials, and no new
firmware protocol. Broader DSLogic/DSCope modes and bootloader coverage are
roughly **4–6 engineer-weeks total**, conditional on those gates. If Darwin
adoption/exclusivity cannot be proved or startup requires re-enumeration, stop
and revise scope; this estimate does not cover replacing the whole USB backend
or enabling reconnect.

Before lifting the CLI rejection gate, require (1) source/backend proof of the
exact-object binding and non-seizing claim semantics, (2) mocked call logs that
prove claim precedes **every** mutation on the same handle and zero mutations
for missing/ambiguous/changed identity, busy/failed claim, detach and new-object
re-enumeration, including asynchronous races and loader variants, and (3)
separately authorized macOS hardware tests of second-process contention,
disconnect/replug and supported bootloader/runtime profiles, plus no-flag
regressions. Listing cached registry properties alone satisfies none of the
ownership/transfer gates. This task performs source review and mocked parser
tests only; no local registry, production CLI, scan or capture test is used.

## Log level

Both capture and `--list` accept `--log-level N`. The default is `1`.

| N | Messages |
| --- | --- |
| 0 | None |
| 1 | Errors |
| 2 | Errors and warnings |
| 3 | Errors, warnings, and information |
| 4 | All of the above and debug messages |
| 5 | All of the above and detailed messages |

`N` must be a whole decimal number from `0` to `5`. Leading zeros are
accepted. Signs, whitespace, fractions, nonnumeric text, and values outside
the range are rejected. The option can appear only once.

An invalid, missing, or duplicate value returns exit status `2` and one JSON
error on stdout before any libsigrok4DSL call. For example, `--log-level 6`
returns:

```json
{"error":"invalid option value","option":"--log-level","value":"6"}
```

There is no environment variable for the log level. Without the option,
the tool keeps its existing error-only logging behavior.

## Parent lifetime

Use `--parent-fd N` when a parent process must prevent an orphaned capture.
`N` must be an inherited, open, blocking pipe read descriptor above 2. Regular
files, sockets, directories, write ends, nonblocking descriptors, and descriptors
0, 1 and 2 are rejected. The argument is an unsigned decimal integer that fits
in an `int`; repeated `--parent-fd` options are rejected.

The parent must hold the pipe's only write end. Do not inherit that write end
into `dslcap` or another process. Keep it open until the child has delivered its
result and exited. Closing it cancels the child. An executed child can inherit
the read end explicitly, for example with Python:

```python
import os
import subprocess

read_fd, write_fd = os.pipe()
try:
    child = subprocess.Popen(
        ["dslcap", "--parent-fd", str(read_fd),
         "--channels", "0,1", "--samples", "1000000", "--out", "/tmp/capture"],
        pass_fds=(read_fd,),  # the write end stays only in this parent
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
    )
finally:
    os.close(read_fd)

try:
    stdout, stderr = child.communicate()
    # Accept a capture only with exit status 0 and a complete JSON result.
finally:
    os.close(write_fd)
```

If the parent exits, the operating system closes its write end. The watcher
starts before any libsigrok setup or initialization and blocks reading its own
close-on-exec duplicate of `N`. Pipe bytes are ignored. EOF or a read error
terminates the whole process with `_exit(1)`; interrupted reads are retried.
An already-closed pipe is detected before library initialization, even if it
contains unread bytes. The watcher stays active during capture, conversion,
result delivery and library teardown. It does not wait for capture callbacks,
flush stdio, run exit handlers, or call library cleanup functions.
When combining this option with another pre-init operation (such as a resource
manifest reader), start the watcher immediately after argument validation and
before that operation so parent loss can interrupt it as well.

Before exiting on parent loss, the watcher removes the temporary output and any
capture file this invocation published but has not yet reported successfully.
The raw spool is unlinked as soon as it is created. File creation and atomic
publication share a short bookkeeping lock with the watcher; capture I/O,
conversion, stdout and library calls do not hold that lock. Synchronous pipe
checks also guard publication and result delivery. Existing destination files
are never owned by the watcher and are never deleted by it.

Parent loss produces no additional JSON or stderr diagnostic. `_exit` discards
buffered stdout. Bytes already delivered cannot be recalled; do not accept a
partial JSON result. A complete result delivered before parent loss can retain
its complete `.bin` file. Without `--parent-fd`, behavior is unchanged.

With this flag, `SIGPIPE` is ignored before watcher startup. If the parent also
closes its stdout or stderr reader, writes report an I/O error instead of killing
the child ahead of cleanup. A failed stdout result removes an unreported capture
file and exits with status 1. Invocations without the flag keep their original
signal behavior. Failure to configure `SIGPIPE` is a watcher setup error.

Invalid descriptors or watch setup failures exit with status 2 and one JSON
argument error on stdout, including `"option":"--parent-fd"` and the supplied
`"value"` (when present). No libsigrok call occurs on these failure paths. Other
exit statuses are 0 for success, 1 for runtime or I/O errors, and 3 for a failed
capture.

## No-hardware regression tests

The existing spool harness includes `test_parent_fd.c`. It runs the real CLI in
bounded child processes with library stubs and syscall fault injection. It
covers invalid descriptors, setup failures before all library calls, pre-init
EOF, buffered pipe data, read errors and interruption, blocked initialization,
a held callback mutex, blocked stdout, teardown, temporary-file and publication
races, existing files, normal results and the invocation without the flag.
Output creation, `fdopen`, signal setup, stdout failures, and actual broken-pipe
reader closure (alone and racing with parent-pipe closure) are also checked.
Every parent-loss test checks prompt exit, stdout/stderr and file cleanup.
The same harness is run by the existing CI; no workflow change is needed.

It also includes `test_device_guard.c`. All mocked libsigrok entry points count
calls and abort if reached by a guarded invocation; this executable does not
link libusb or IOKit. The tests use the real parser/main, check strict macOS
`loc-xxxxxxxx` width/case/hex/overflow boundaries and legacy USB location syntax,
missing/empty/non-UTF-8 serials, verbatim serials (including colons, Unicode,
spaces and JSON escapes), duplicate/conflicting flags, JSON exit 2 and no files.
A readable empty manifest pipe proves rejection precedes even a potentially
blocking resource preflight, with and without a valid parent watcher. The
existing fake captures/lists still verify no-flag behavior. This proves rejection
ordering and the absence of wrong-device mutation; it does not simulate or
prove successful claim-before-upload, serial-change detection, claim-busy
handling, detach handling or object retention across re-enumeration. Those
runtime tests belong to the blocked driver work. No real analyzer is needed or
accessed by this harness.

On macOS, from the repository root:

```sh
cc -std=c99 -Wall -Wextra -Werror \
  -Ilibsigrok4DSL -Icommon $(pkg-config --cflags glib-2.0) \
  tools/dslcap/test_spool.c $(pkg-config --libs glib-2.0) \
  -Wl,-dead_strip -o /tmp/test_spool
/tmp/test_spool /tmp/dslcap-raw /tmp/dslcap-output.bin
```

On Linux, add `-D_DEFAULT_SOURCE -ffunction-sections -fdata-sections`, link with
`-pthread -lm`, and replace `-Wl,-dead_strip` with `-Wl,--gc-sections`.

The fake CLI includes Demo Device and DSLogic for no-flag list/capture tests.
Do not run the production `dslcap --list` for hardware-free testing: its driver
scans can upload firmware even if only a demo result is of interest.

## Hardware-free logging tests

Run these commands from the repository root. They compile and run test
harnesses only. They do not initialize the device library, scan for devices,
or capture samples.

The spool harness includes parsing tests for the default, levels `0..5`,
out-of-range and nonnumeric values, and duplicate options. It also checks
the existing capture-data and output-file behavior. The startup harness
intercepts the CLI at its first libsigrok4DSL call, before initialization.
It checks the selected level, JSON argument errors before that call, and
the real logger's stderr routing. Combined cases use a held parent pipe to
verify watcher startup before the first library call, the default and all log
levels, and invalid/duplicate log arguments before watcher startup. No device drivers are linked into either
harness. Valid startup is intercepted before the CLI can produce its capture
result; these tests do not verify a hardware capture.

macOS:

```sh
mkdir -p build-log-level-tests
cc -Ilibsigrok4DSL -Icommon $(pkg-config --cflags glib-2.0) \
  tools/dslcap/test_spool.c $(pkg-config --libs glib-2.0) \
  -Wl,-dead_strip -o build-log-level-tests/test_spool
cc -Ilibsigrok4DSL -Icommon $(pkg-config --cflags glib-2.0) \
  tools/dslcap/test_log_level.c libsigrok4DSL/log.c common/log/xlog.c \
  $(pkg-config --libs glib-2.0) -Wl,-dead_strip \
  -o build-log-level-tests/test_log_level
build-log-level-tests/test_spool build-log-level-tests/raw build-log-level-tests/output.bin
build-log-level-tests/test_log_level
```

Linux:

```sh
mkdir -p build-log-level-tests
cc -D_DEFAULT_SOURCE -ffunction-sections -fdata-sections \
  -Ilibsigrok4DSL -Icommon $(pkg-config --cflags glib-2.0) \
  tools/dslcap/test_spool.c $(pkg-config --libs glib-2.0) \
  -lm -pthread -Wl,--gc-sections -o build-log-level-tests/test_spool
cc -D_DEFAULT_SOURCE -ffunction-sections -fdata-sections \
  -Ilibsigrok4DSL -Icommon $(pkg-config --cflags glib-2.0) \
  tools/dslcap/test_log_level.c libsigrok4DSL/log.c common/log/xlog.c \
  $(pkg-config --libs glib-2.0) -lm -pthread -Wl,--gc-sections \
  -o build-log-level-tests/test_log_level
build-log-level-tests/test_spool build-log-level-tests/raw build-log-level-tests/output.bin
build-log-level-tests/test_log_level
```

## Resource verification

`dslcap` captures without the GUI. Its resource directory is selected by `--res
DIR`, `DSLCAP_RES`, or the existing executable/install resource lookup.

```text
dslcap --channels 0,1 --samplerate 10000000 --samples 1000000
       [--vth 1.6] [--mode buffer|stream] [--trigger CH[:R|F|C|1|0]]
       [--trigpos PERCENT] [--timeout SEC] [--res DIR]
       [--parent-fd N] [--res-manifest FD] [--log-level N]
       --out /path/base
```

`--res-manifest FD` enables optional SHA-256 verification of firmware and FPGA
bitstreams. FD must be an inherited, open, readable descriptor. The process reads
it directly, from its current position through EOF; it does not seek, close it,
or reopen `/dev/fd` or `/proc/self/fd`. A pipe works: the writer must close its end
after sending the manifest. Invalid arguments and preflight failures return exit
status 2 with a JSON `error`, before `ds_lib_init()`.

Each line is exactly 64 hexadecimal SHA-256 characters, one space, and a path
relative to the resource directory, for example:

```text
<64 hex characters> DSLogic.fw
<64 hex characters> DSLogic33.bin
<64 hex characters> DSLogic50.bin
```

Include **every `.fw` and `.bin` under the directory**, including subdirectories
and resources for devices other than the one being captured. Other entries are
permitted and are also verified. These extensions cover all firmware/bitstream
paths in the current DSL profiles. Uppercase hex and CRLF are accepted, as is a
final line without a newline. Empty manifests, blank lines, comments, duplicate
paths, malformed hashes, absolute paths, `.`/`..` components, empty components,
backslashes and control characters are rejected. Paths may contain spaces.

The flag requires a real directory and regular, nonempty resource files. Symlinks
in the resource directory or in manifest paths are rejected. Relative paths are
limited to 4095 bytes and directory traversal to 64 levels. A manifest is limited
to 1 MiB, `.fw` files to 64 KiB (the FX2 address space), other listed files to
16,777,215 bytes (the FPGA protocol length), and cached resources to 256 MiB total.
Read, allocation and size failures are fatal; partial reads, growing files and
non-regular files cannot be uploaded.

Preflight opens each listed file once beneath the opened directory, reads it
completely, checks EOF and hashes that buffer. It also checks manifest coverage
of the resource directory. Only after the entire preflight succeeds does library
initialization begin. A missing entry or hash mismatch for any resource present
under `--res` therefore prevents **all USB initialization and uploads**, including
firmware uploads that would otherwise precede an FPGA failure.

The loaders use the same immutable cached memory that was hashed; they never
reread the disk files. Replacing, deleting or editing a file after successful
preflight cannot change the uploaded bytes. A newly added file has no cached
buffer and cannot be uploaded. The cache describes the verified snapshot;
directory changes after that snapshot do not replace it.

The scan/open code still performs the usual USB enumeration, descriptor/status
reads and device activation after successful preflight. Some profiles name
assets absent from the shipped resource directory. If such an asset is requested,
the cache lookup fails before that loader opens/configures/resets the device or
sends any load command/data; earlier enumeration traffic may already have occurred.
Preflight does not require nonexistent files for every possible hardware profile.
Device selection and listing behavior is otherwise unchanged.

The changed loaders are `ezusb_upload_firmware()` (validation before opening,
configuration and CPU reset), `ezusb_install_firmware()` (including direct calls),
and `dsl_fpga_config()` (validation before PROG_B, LED, status/config commands and
bulk transfer). DSLogic/DSCope firmware scans, device-open FPGA loads, and threshold
changes all use these loaders. Without the flag, their existing file paths and
behavior are retained. Verification is independent of `LANSCAPES_BRAND`.

For a manifest prepared by a trusted caller:

```sh
exec 3< /path/to/trusted-manifest.txt
./build.dir/dslcap --res /path/to/res --res-manifest 3 --out /tmp/capture
exec 3<&-
```

The manifest must come from the caller's trusted expected hashes. Generating it
from already modified files would authenticate those modified bytes.

Library callers can opt in with
`ds_set_firmware_resource_manifest(fd, &error)` after setting the resource
directory and before initializing the library. They retain ownership of `fd`.
Configuration is immutable while library threads run. Failed setup remains in
fail-closed mode. Call with `-1` after `ds_lib_exit()` to release cached memory
and disable verification. `dslcap` registers process-exit cleanup.

## Hardware-free tests

After the normal CMake configuration:

```sh
cmake --build build --target dslcap dslcap_test_resources
./build.dir/dslcap_test_resources
```

The test runs the actual preflight, CLI main, firmware and FPGA loaders with
`ds_lib_init` and USB side effects stubbed. It checks whole-directory missing
entries and hash mismatches before initialization, malformed manifests, descriptor
ownership/read errors, path escapes/symlinks, file types and size limits, injected
short reads/EINTR/EOF/I/O errors/growth, and failed-setup behavior. USB stubs assert
zero calls on failure and the exact cached buffer address on every successful
data upload after the disk files are replaced/deleted. Short USB transfers fail.
No analyzer is enumerated or accessed.

The existing spool test includes manifest argument validation and is still run by
the unchanged CI workflow. The resource regression target is explicitly requested
and run locally; the existing workflow does not run it. Hardware firmware
re-enumeration, FPGA programming and captures still require bench validation.

## Combined parent, manifest and logging checks

When both flags are supplied, the parent watcher starts immediately after
argument validation, before resource discovery or manifest preflight. The
synchronous `parent_check()` immediately before `ds_log_level(o.log_level)` is retained.
Parent loss interrupts a blocked manifest reader without library initialization
or resource upload. The two inherited descriptors remain caller-owned; the
watcher owns its close-on-exec duplicate, and manifest input is read directly.

The resource harness also exercises both flags together with a live parent,
invalid manifest, already-lost parent, and parent loss while manifest input is
blocked. These bounded child processes use the actual watcher/preflight and
stubbed library initialization/USB calls. No real-device list, scan, capture or
upload is run by these checks.

With all three flags, the resource harness checks the default and log levels
`0..5`, exact startup order (watcher, logging, manifest reads, initialization),
one JSON result on stdout, and severity filtering on stderr. Invalid or duplicate
log arguments fail before watcher startup or any library call. Invalid manifests,
an already-lost parent, and parent loss during a blocked manifest read are also
checked with explicit logging enabled. Initialization and USB side effects remain
stubbed.

The spool/parent harness additionally checks normal publication and cleanup on
parent loss during publication or stdout delivery with all three flags. Its
manifest API is stubbed; the resource harness tests the actual preflight.
