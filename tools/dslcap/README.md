# dslcap

`dslcap` captures DSLogic samples without the graphical interface using libsigrok4DSL. It prints one
JSON result on stdout. Library diagnostics use stderr. A successful capture
creates `<base>.bin`; an existing file at that path is never replaced.

```text
dslcap --list [--res DIR] [--parent-fd N] [--res-manifest FD] [--log-level N]
dslcap --list-ids [--parent-fd N]
dslcap --channels 0,1 --samplerate 10000000 --samples 1000000
       [--vth 1.6] [--mode buffer|stream] [--trigger CH[:R|F|C|1|0]]
       [--trigpos PERCENT] [--timeout SEC] [--res DIR]
       [--parent-fd N] [--res-manifest FD] [--log-level N]
       --out /path/base
```

## Read-only macOS identity listing

`--list-ids` reads **cached IORegistry properties** on macOS. It accepts only
optional `--parent-fd N`. It branches after argument validation and parent-watcher
setup, before resource lookup/preflight, logging/callback setup, `ds_lib_init()`
or any libsigrok driver scan. `DSLCAP_RES` does not affect it. Combining this mode
with `--list`, capture, resource or log-level options is an argument error (status
2). The exact token `--list-ids` and values beginning `--list-ids=` are reserved
and cannot be swallowed as another option's argument and enter the legacy scan.
The equals form is unsupported; use `./--list-ids=true` or an absolute path for a
file with that name.

This path makes **zero libusb calls and zero USB requests**, including zero
standard `GET_DESCRIPTOR` requests. It does not read a live USB device descriptor
or follow `iSerialNumber`; the serial comes from the existing `USB Serial Number`
registry property. The OS may have populated that cache earlier. Nothing here
refreshes it or asks the device for a missing string. Direct libusb enumeration
was rejected because backend initialization can open devices, change configuration
or suspend state before application filtering (see the
[libusb v1.0.30 Darwin backend](https://github.com/libusb/libusb/blob/v1.0.30/libusb/os/darwin_usb.c#L1082-L1176)
and [Linux backend](https://github.com/libusb/libusb/blob/v1.0.30/libusb/os/linux_usbfs.c#L892-L1005)).
Non-macOS production builds refuse before any registry or USB call: stdout is
`{"devices":[]}` plus a newline, stderr explains that the backend is unavailable,
and exit status is 1. There is no production override.

### JSON and identity contract

One JSON object is printed on stdout; diagnostics use stderr:

```json
{"devices":[{"vid":10766,"pid":32,"model":"DSLogic Plus","location":"loc-20121500","serial":"example:exact","state":"unknown"}]}
```

| Field | Type and meaning |
| --- | --- |
| `vid`, `pid` | JSON integers, unsigned 16-bit cached `idVendor`/`idProduct`, printed in decimal. Only pairs in both DSL tables in `libsigrok4DSL/hardware/DSL/dsl.h` are included. |
| `model` | JSON string from that table, independent of the cached product label. Duplicate speed profiles share one table entry. |
| `location` | JSON string `loc-` plus exactly eight lowercase hexadecimal digits from the complete nonzero 32-bit cached `locationID`; `null` if unavailable or malformed. Decimal 538055936 is `loc-20121500`; decimal 538050560 is `loc-20120000`. No masking or rounding. |
| `serial` | JSON string converted losslessly from the cached CFString to UTF-8, or `null`. No case folding, normalization, replacement or truncation. JSON escaping preserves controls, quotes and backslashes. |
| `state` | Always `"unknown"`. VID/PID, product labels and `bcdDevice` do not establish bootloader/runtime, FPGA state or capture readiness. |

The coordinated selector representation is `<location>:<serial>`, split on the
**first colon**, preserving all subsequent colons and exact UTF-8 serial bytes.
Both identity fields must be present; unknown serials never become an empty
string or wildcard. This change does not implement selection or depend on its
branch. A location describes the host/controller/port arrangement, not a globally
unique or permanent identifier. Controller changes, hub/port moves and reconnects
can change it. A device that re-enumerates at the same location is a new device;
matching identity fields alone cannot prove continuity or distinguish collisions.

Numeric properties must be integer CFNumbers that convert losslessly to a
nonnegative signed 64-bit value within the relevant unsigned bound. Booleans,
CFData, floating-point, negative and out-of-range values are rejected; a negative
CFNumber is not reinterpreted as unsigned bits. Serial CFStrings must be nonempty,
contain at most 4096 UTF-16 units / 16384 UTF-8 bytes, and convert completely with
`lossByte=0`. Empty strings, embedded NUL, unpaired surrogates, oversized values,
wrong types and allocation failures yield `null`, never a lossy identity.

Missing/malformed VID/PID prevents safe filtering: that entry is omitted, a
stderr diagnostic explains the incomplete inventory, and status is 1. Recognized
rows retain `null` for unreadable location/serial and also produce a diagnostic
and status 1. The property API returns NULL for an absent **or** inaccessible
property; the command cannot distinguish these and does not silently report a
complete identity. Optional cached product/revision properties are read but are
not output or used to infer state; their absence does not invalidate identity.
Enumeration, iterator invalidation and handle-release failures also return 1.
A successful empty result is status 0. Require status 0 and complete JSON before
using the result. Parent loss exits immediately with status 1 and may leave no
result; stdout failure is status 1.

### Exact registry / CoreFoundation API inventory

`IOUSBHostDevice` is queried first. `IOUSBDevice` is queried only when the first
class yields no recognized DSL row **and** no errors. The classes are never
combined, avoiding duplicate views of one device. Unrelated entries do not block
fallback; an error does. This fallback does not discover legacy-only entries
when at least one DSL device is already visible in the modern class. Rows follow
iterator order; there is no sort, retry, reset or follow-up scan. Each property is
an individual cached snapshot, not an atomic identity: registry changes can race
these reads and stale or inconsistent cache values cannot establish live identity.

The complete application registry and CF call inventory is:

| API | Purpose / ownership |
| --- | --- |
| `IOServiceMatching("IOUSBHostDevice")`, optionally `IOServiceMatching("IOUSBDevice")` | Create one class-matching dictionary per queried class. |
| `IOServiceGetMatchingServices(kIOMainPortDefault, matching, &iterator)` | Read the class matches. Consumes the dictionary on **success and failure**. A successful null iterator is empty. No user client is opened. |
| `IOIteratorNext(iterator)` | Obtain each registry entry; zero ends the traversal. |
| `IORegistryEntryCreateCFProperty(entry, key, kCFAllocatorDefault, 0)` | At most one read per key per entry: `idVendor`, `idProduct`; for a recognized pair also `locationID`, `USB Serial Number`, `USB Product Name`, `bcdDevice`. Each returned nonnull property is released exactly once. Unmatched pairs receive only the two ID reads. |
| `CFGetTypeID`, `CFNumberGetTypeID`, `CFNumberIsFloatType`, `CFNumberGetValue(..., kCFNumberSInt64Type, ...)` | Validate each numeric snapshot and perform checked conversion. |
| `CFStringGetTypeID`, `CFStringGetLength`, `CFStringGetBytes(..., kCFStringEncodingUTF8, 0, false, ...)` | Validate each string; measure once and, if valid and allocated, copy once. Both conversions must cover the whole UTF-16 range with the exact measured byte count. `CFRangeMake` and `CFSTR` construct the range and constant keys. |
| `CFRelease(property)` | Release every obtained property snapshot, including invalid values and conversion/allocation failures. |
| `IOIteratorIsValid(iterator)` | Check validity after `IOIteratorNext` returns zero; do not reset an invalid iterator. |
| `IOObjectRelease(entry / iterator)` | Release each acquired registry entry and iterator, including an iterator returned on enumeration failure. This releases registry handles, not USB interfaces. |

No `IOUSBDeviceInterface`, `IOServiceOpen`, device open, device request, reset,
set-configuration, interface claim/detach, control transfer, firmware/FPGA
transfer or sampling operation appears in this path. It does not initialize
libusb, enumerate through a libusb backend, or initialize/scan libsigrok4DSL.

### Hardware-free verification and owner bench

The existing spool CI step includes `test_list_ids.c`, runs the real CLI and
listing implementation with fake registry APIs, and links GLib alone. Its header
exports only the registry allowlist; an added IOKit/device or libusb call fails
compilation/linking. Library stubs reject DS initialization/scan/activation,
resource/log setup, callbacks and teardown. Every property read checks the exact
key, type API and once-per-entry bounds; retained property copies, consumed
matching dictionaries and released entries/iterators must balance.

Tests cover all 22 unique pairs / 25 DSL speed profiles in both directions, empty
and multiple inventories, legacy fallback and duplicate class views, unchanged
32-bit location values, Unicode and JSON escaping, wrong/missing CF types,
negative/floating/out-of-range numbers, strict serial failures, allocation failure,
registry/query/release/iterator faults, option conflicts/reserved tokens, parent
loss before/during fake registry calls, stdout failure, and capture/spool/parent
regressions. Run the spool command below from the repository root.

On macOS, the additional `dslcap_test_registry_cf` target uses actual
CoreFoundation **memory objects**, with the same fake registry and DS APIs; it
links CoreFoundation and GLib, **no IOKit or libusb**:

```sh
cmake --build build --target dslcap_test_registry_cf
./build.dir/dslcap_test_registry_cf /tmp/ids-test-raw /tmp/ids-test-output.bin
```

Compile the production `dslcap` target to verify the SDK/link, but do not run
production inventory, CLI, GUI or packaging as a hardware-free test. No real
registry or device bench was performed for this change. The owner can separately
run the following on the intended macOS host while observing USB traffic and
checking expected cached serial/location values:

```sh
./build.dir/dslcap --list-ids
```

That owner bench remains necessary to validate installed-driver/cache behavior
and absence of USB traffic on the intended host. Cached values cannot prove
firmware state, live-device continuity or capture readiness.

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

The built executable can list Demo Device with `dslcap --list`. Capture still
selects DSLogic as before; these tests do not change selection to enable Demo
Device capture.

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
