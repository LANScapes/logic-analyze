# dslcap

`dslcap` captures DSLogic samples without the graphical interface. It prints one
JSON result on stdout. Library diagnostics use stderr. A successful capture
creates `<base>.bin`; an existing file at that path is never replaced.

```text
dslcap --list [--res DIR] [--parent-fd N] [--res-manifest FD]
dslcap --list-ids [--parent-fd N]
dslcap --channels 0,1 --samplerate 10000000 --samples 1000000
       [--vth 1.6] [--mode buffer|stream] [--trigger CH[:R|F|C|1|0]]
       [--trigpos PERCENT] [--timeout SEC] [--res DIR]
       [--parent-fd N] [--res-manifest FD]
       --out /path/base
```

## Guarded descriptor identity listing

`--list-ids` is a **fail-closed scaffold**, not a usable hardware inventory yet.
It accepts only optional `--parent-fd N`; combining it with `--list`, capture or
resource options is an argument error (status 2). It branches immediately after
argument validation and parent-watcher setup, before resource lookup, manifest
preflight, logging/callback setup, `ds_lib_init()` or any libsigrok driver scan.
`DSLCAP_RES` does not affect this path.

**Every production backend currently refuses the operation before any libusb
call**, on macOS, Linux and other platforms. It returns status **1**, prints
exactly `{"devices":[]}` plus a newline on stdout, and explains on stderr that
no audited descriptor-only enumeration path exists and USB was not initialized.
An empty array with a nonzero status is an unavailable/incomplete inventory,
never a successful finding that no device exists. Callers must require status 0
and a complete JSON object. Parent loss retains the existing immediate status 1
with no additional result; stdout failure is also status 1.

### Why enumeration is guarded

The source audit is pinned to upstream **libusb v1.0.30**; it establishes reachable
operations, not that every enumeration performs them, nor the exact behavior of
a locally installed or patched binary.

* [Darwin backend](https://github.com/libusb/libusb/blob/v1.0.30/libusb/os/darwin_usb.c#L1082-L1176):
  `darwin_init` -> `darwin_init_context` -> `darwin_scan_devices` ->
  `darwin_get_cached_device` -> `darwin_cache_device_descriptor`.
  Initialization scans before application VID/PID filtering. Caching tries
  native `USBDeviceOpenSeize` (1099), then standard device `GET_DESCRIPTOR`
  (1102). A non-Apple invalid descriptor with zero `bNumConfigurations` or
  `bcdUSB` may trigger native `SetConfiguration(1)` (1119) if opened. Failed
  descriptor reads may unsuspend/resuspend using `USBDeviceSuspend` (1142,
  1162). Thus the cache-building operation exceeds the allowed contract.
  The separate `darwin_get_device_string` reads IORegistry properties without
  opening a handle; the problem is initialization, not a claim that every serial
  read seizes the device. Scan/init paths are at 1430–1455 and 842–873.
* [Linux backend](https://github.com/libusb/libusb/blob/v1.0.30/libusb/os/linux_usbfs.c#L892-L1005):
  `op_init` -> `linux_scan_devices` -> `linux_default_scan_devices` may fall back
  from sysfs to usbfs (1288–1298). `linux_enumerate_device` -> `initialize_device`
  may open usbfs read-only (929), then read/write (986), and call
  `usbfs_get_active_config` (1001). That sends native `IOCTL_USBFS_CONTROL` with
  `GET_CONFIGURATION` (830–846), which exceeds standard `GET_DESCRIPTOR`.
  USB-node opens can also resume suspended devices (37–59). `op_open` likewise
  opens read/write (1366–1388). An available sysfs path does not prove the
  fallback unreachable. This PR does not enable Linux from API names alone.

There is no runtime flag, environment variable or CMake option to bypass the
production guard. A future change must establish an audited backend contract
before enabling it. This PR does not patch libusb or implement native registry
enumeration. The requested functional macOS/hardware inventory remains
**unfulfilled**.

### Descriptor core and identity contract

The core is exercised only with fake libusb in the regression harness. It
projects the VID/PID/model entries from both `supported_DSLogic` and
`supported_DSCope` in `libsigrok4DSL/hardware/DSL/dsl.h`, collapsing duplicate
speed profiles with the same model. A test compares every source profile and
core entry in both directions to prevent table drift. It requests no USB speed,
manufacturer, product, configuration, BOS, status or vendor-specific data.
Unmatched VID/PID pairs are omitted without opening them.

The output shape for this tested core is one JSON object:

```json
{"devices":[{"vid":10766,"pid":1,"model":"DSLogic","location":"usb-1-2.3","serial":"example","state":"unknown"}]}
```

* `vid` and `pid`: JSON integers, unsigned 16-bit values in decimal.
* `model`: JSON string from the profile table, not a USB product-string query.
* `location`: JSON string `usb-<bus>-<port>[.<port>...]`, decimal numbers without
  leading zeroes, for one to seven ports; `null` when the bus/complete port path
  is unavailable. This identifies a physical topology within a host/controller
  arrangement. It excludes the changing USB address, but is not globally unique
  or guaranteed stable across reboot, controller renumbering, hub/port changes
  or reconnects. A re-enumeration is a new device even at the same path.
* `serial`: JSON string decoded strictly from the descriptor at `iSerialNumber`,
  using the first language advertised by string descriptor zero. UTF-16LE
  surrogate pairs become UTF-8; bytes are JSON-escaped as needed, with no Unicode
  normalization, case folding or ASCII replacement. Absent index zero is
  `null`, with no handle open or descriptor request. Access/busy/detach errors,
  malformed or short descriptors, empty strings, embedded NUL and unpaired
  surrogates produce `null`, a stderr diagnostic and status 1. An absent serial
  is a complete finding (status 0 in the core), but cannot be selected by serial.
  Unknown/unreadable serials must never match an empty string or wildcard.
* `state`: always `"unknown"`. These VID/PIDs are shared by pre-firmware and
  runtime devices; the requested descriptors establish neither firmware/FPGA
  state nor readiness. A profile model name does not establish runtime state.

A missing location or any unreadable device descriptor also sets status 1 and
explains the incomplete inventory on stderr. A descriptor failure cannot safely
be treated as an unrelated device. Enumeration/init failure yields an empty
array, diagnostic and status 1. Recognized rows with incomplete identities
retain `null` fields; there is no invented address/name/first-device substitute.
Rows follow the one enumeration snapshot; no retry, reconnect or follow-up scan
occurs. The coordinated selector representation is `<location>:<serial>`, split
on the first colon (serials may contain colons). Both fields must be present.
This scaffold does not implement selection or depend on the selector branch.

### Exact USB call and request inventory

Production `--list-ids`: **zero libusb calls and zero USB requests**. For the
fake-tested core (currently unreachable with a production backend), the complete
application call allowlist is:

| Call | Purpose and bounds |
| --- | --- |
| `libusb_init(&ctx)` | One private context; guard must have allowed the backend first. |
| `libusb_get_device_list(ctx, &list)` | One enumeration snapshot. |
| `libusb_get_device_descriptor(dev, &desc)` | Once per enumerated device; reads libusb's cached device descriptor, including VID/PID and `iSerialNumber`. This accessor sends no request; building that cache can, as audited above. |
| `libusb_get_bus_number(dev)` | Once per matching device; cached topology. |
| `libusb_get_port_numbers(dev, ports, 7)` | Once per matching device; complete cached port path or unknown. |
| `libusb_open(dev, &handle)` | Once per matching device with nonzero `iSerialNumber`; solely for string requests; failures are incomplete identity. Backend opens need separate audit. |
| `libusb_control_transfer(...)` | At most two standard string `GET_DESCRIPTOR` requests per successfully opened handle, detailed below. |
| `libusb_close(handle)` | Once for each successfully opened handle, on every read/decode outcome. No interfaces were claimed. |
| `libusb_free_device_list(list, 1)` | Releases the snapshot/references when allocated. |
| `libusb_exit(ctx)` | Once after a successful init, including enumeration failures. |

The only application requests are device-recipient standard IN
`GET_DESCRIPTOR`: `bmRequestType=0x80`, `bRequest=0x06`, `wLength=256`, timeout
1000 ms. First `wValue=0x0300`, `wIndex=0` reads the language table; only if valid,
`wValue=0x0300 | iSerialNumber`, `wIndex=<first advertised nonzero LANGID>` reads
the serial. These are **active string descriptor requests**, not reads of a
cached device descriptor. Negative, truncated, wrong-type, odd-length or
inconsistent-length responses fail. No English-language fallback or retries.

No reset, configuration/alternate-setting change, interface claim/release,
kernel-driver detach/attach/auto-detach, halt clearing, vendor/status request,
firmware/FPGA transfer, bulk/interrupt/isochronous sampling or driver scan is
made by this path. The production guard also prevents the backend's reachable
non-allowlisted operations described above.

### Hardware-free verification

`test_list_ids.c` is included by the existing spool harness, so the unchanged CI
spool step executes it. The harness links only GLib (no real libusb), uses the
actual CLI and listing core, and runs bounded child processes. Library stubs
assert against libsigrok initialization, listing/activation, resource setup,
callbacks and teardown. The fake USB header exports only the call allowlist;
added USB actions fail compilation/linking, and every control request must match
the exact standard `GET_DESCRIPTOR` fields and index/language/order above.

It checks the pre-init production guard, zero-device and multi-device output,
all 22 unique VID/PID/model pairs against all 25 upstream speed profiles,
unrelated devices, absent serials, permission/detach and descriptor failures,
Unicode/surrogates/JSON escaping, maximum serial/port paths, no invented location,
conflicting arguments in both orders, parent loss before/during fake init,
stdout failure and context/list/handle cleanup. Run the spool command below from
the repository root. Compile the normal `dslcap` target to check the real USB
header/link; **do not run it against hardware** to validate this scaffold.

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

## Resource verification

`dslcap` captures without the GUI. Its resource directory is selected by `--res
DIR`, `DSLCAP_RES`, or the existing executable/install resource lookup.

```text
dslcap --channels 0,1 --samplerate 10000000 --samples 1000000
       [--vth 1.6] [--mode buffer|stream] [--trigger CH[:R|F|C|1|0]]
       [--trigpos PERCENT] [--timeout SEC] [--res DIR]
       [--parent-fd N] [--res-manifest FD]
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

## Combined parent and manifest checks

When both flags are supplied, the parent watcher starts immediately after
argument validation, before resource discovery or manifest preflight. The
synchronous `parent_check()` immediately before `ds_log_level(1)` is retained.
Parent loss interrupts a blocked manifest reader without library initialization
or resource upload. The two inherited descriptors remain caller-owned; the
watcher owns its close-on-exec duplicate, and manifest input is read directly.

The resource harness also exercises both flags together with a live parent,
invalid manifest, already-lost parent, and parent loss while manifest input is
blocked. These bounded child processes use the actual watcher/preflight and
stubbed library initialization/USB calls. No real-device list, scan, capture or
upload is run by these checks.
