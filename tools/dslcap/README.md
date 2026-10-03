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
       [--device LOCATION:GENERATION]
       --out /path/base
```

## Guarded device selection (capture unavailable)

`--device LOCATION:GENERATION` reserves an exact-identity capture interface, but
**this build cannot perform captures with it**. A syntactically valid request
returns exit status **2** and one JSON object:

```json
{"error":"exact-device capture is unavailable: driver scans can upload firmware before an exclusive claim","code":"device_selection_unavailable","option":"--device","value":"loc-20121500:100003421"}
```

The canonical macOS location format agreed with the separate registry-only
`--list-ids` work is `loc-<8 lowercase hexadecimal locationID digits>`, for
example `loc-20121500`. All eight digits are required, including leading zeros;
uppercase, signs, `0x`, whitespace and over-width/overflow values are rejected.
The format represents a raw unsigned 32-bit cached IORegistry `locationID`,
without rounding or masking. Decimal `538055936` is `0x20121500`, hence
`loc-20121500`; `loc-20120000` represents decimal `538050560`. This conversion
is arithmetic only, not a local observation of a connected device.

`GENERATION` is the unsigned 64-bit IORegistry entryID of that **same exact
matched device entry**, rendered as 1–16 lowercase hexadecimal digits with no
`0x` or leading zeros, except literal `0`. The listing contract obtains it from
a successful entryID API call and formats it with `PRIx64`; successful numeric
zero is valid. A missing location or failed/missing entryID result is
unselectable, never a substituted zero. A legacy `IOUSBDevice` listing fallback
must obtain generation from that same matched entry, never an ancestor or
another class. There is exactly one colon. Uppercase, signs, whitespace, extra
separators, empty generations and uint64 overflow are rejected. Old USB
topology selectors and serial selectors have no compatibility path. A suffix
made solely of canonical lowercase hex is interpreted only as an entryID,
even if those characters could also have appeared in an old serial; no serial
lookup, inference or fallback occurs.

The owner reports the example `loc-20121500:100003421`, absent serial
(`iSerialNumber == 0`), and a registry-only listing bench check with zero USB
traffic and unchanged session/address. These are owner-reported observations,
not local hardware validation by this change. Serial absence is allowed in the
identity contract: serial is an optional additional check for a future capture
implementation, never the selector. If a serial exists, the eventual selected
device must match its recorded value before mutation.

EntryIDs identify registry objects within the current boot, not durable hardware
identities or USB transition counters. XNU's [`attachToParent` implementation](https://github.com/apple-oss-distributions/xnu/blob/main/iokit/Kernel/IORegistryEntry.cpp#L1971)
assigns an ID if the object has none; reusing the same registry object can retain
its ID. A replacement entry after replug/re-enumeration has a new generation and
must fail the original request even at the same location or with the same serial.
**Detach is terminal regardless of ID equality**; an unchanged ID cannot prove
an uninterrupted USB attachment. Never resume or follow re-enumeration.

A cached listing is a snapshot, scoped to that boot and attachment, and may be
stale before open/claim. Its numeric pair proves neither a live attachment nor
exclusive ownership. A future implementation must retain the original entry,
bind that exact generation and location to one retained, exclusively claimed
transport handle, and revalidate the live binding before mutation; matching a
replacement to cached numbers is insufficient. This parser verifies syntax only
and does not inspect registry entries, serials or ownership.

Malformed identities, missing values, repeated `--device`, or combining it
with `--list` or `--list-ids` also return JSON and exit 2. Invalid UTF-8 is omitted
from the JSON `value`. Ordinary capture arguments, including `--out`, remain required.
After argument validation and optional parent-watch setup, the capability gate
runs before resource lookup, manifest reads, every `ds_*` call, and all USB
access. **The guarded path makes zero libusb calls and zero USB descriptor
requests**; it does not enumerate, open, claim, detach, configure, reset,
upload firmware/FPGA data, start hotplug handling or create capture files.
There is no fallback to name, address, first device or the legacy capture path.
Missing/ambiguous identity, changed generation/location, an available serial
mismatch, claim busy/failure, detach, and re-enumeration therefore cannot result
in a guarded capture or mutation: every request is rejected before those conditions are
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
for the old one and schedules reopening without checking registry generation
or serial. That is incompatible with treating every re-enumeration as a new,
forbidden device.

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
scan-free initialization, exact original registry entry/generation and location
binding to one retained, exclusively claimed handle before every mutation,
optional serial revalidation when a serial exists, loaders that use that handle
with no detach or force takeover, and disabling reconnect/attach scanning for
the guarded session. Missing or ambiguous matches, changed generation/location,
an available serial mismatch, claim failure, detach and any new device object
after re-enumeration must terminate it with JSON exit 2 or 3.
Successful exact-device capture and those runtime checks remain unimplemented.

### P8 runtime-only transport checkpoint (blocked)

Source-only review of the configured **libusb 1.0.30** establishes a transport
blocker even for one already-running DSLogic profile. The official tag resolves
to commit `87a55632db62c9bdc58cd31d3ccfa673f1bb017f`. No downloaded backend code
was executed, built, vendored or changed.

| Current call path | Concrete ownership / ordering problem |
| --- | --- |
| `sr_init` → `libusb_init` → [`darwin_init_context` / `darwin_scan_devices`](https://github.com/libusb/libusb/blob/87a55632db62c9bdc58cd31d3ccfa673f1bb017f/libusb/os/darwin_usb.c#L914) → `darwin_get_cached_device` → [`darwin_cache_device_descriptor`](https://github.com/libusb/libusb/blob/87a55632db62c9bdc58cd31d3ccfa673f1bb017f/libusb/os/darwin_usb.c#L1174) | Global discovery precedes application selection/claim. Caching attempts `USBDeviceOpenSeize`, requests descriptors, and can set configuration or change suspend state. |
| `libusb_open` → [`darwin_open`](https://github.com/libusb/libusb/blob/87a55632db62c9bdc58cd31d3ccfa673f1bb017f/libusb/os/darwin_usb.c#L1586) | Normal open also attempts `USBDeviceOpenSeize`; exclusive-access failure can continue with `is_open == false`. It is not the required non-seizing owned open. |
| `libusb_claim_interface` → [`darwin_claim_interface`](https://github.com/libusb/libusb/blob/87a55632db62c9bdc58cd31d3ccfa673f1bb017f/libusb/os/darwin_usb.c#L1836) | A missing interface can cause configuration changes before `USBInterfaceOpen` establishes the claim. |
| [`LIBUSB_OPTION_NO_DEVICE_DISCOVERY`](https://github.com/libusb/libusb/blob/87a55632db62c9bdc58cd31d3ccfa673f1bb017f/libusb/libusb.h#L1631); [`libusb_wrap_sys_device`](https://github.com/libusb/libusb/blob/87a55632db62c9bdc58cd31d3ccfa673f1bb017f/libusb/core.c#L1406); [Darwin backend table](https://github.com/libusb/libusb/blob/87a55632db62c9bdc58cd31d3ccfa673f1bb017f/libusb/os/darwin_usb.c#L3002) | No-discovery is Linux-only. Darwin has no wrap hook; core returns `LIBUSB_ERROR_NOT_SUPPORTED`. Neither API supplies selected entry/handle adoption. |

The `--list-ids` location/generation identifies a registry entry, not a libusb
transport handle; listing releases its entry references.
`sr_usb_dev_inst` holds opaque `libusb_device` / `libusb_device_handle` pointers,
and current control, FPGA/bulk, asynchronous capture and event paths depend on
that transport. Casting an IOKit object to those types is invalid. Comparing
only bus/address/location or a cached serial cannot bind the original selected
registry generation and is not an acceptable substitute.

Moving the claim above the firmware-version `command_ctl_rd` in `dsl.c`, or
adding scan-free driver construction, would fix only the later driver ordering.
It cannot undo the backend's earlier discovery, seize or configuration paths.
Mocking an unavailable transport-adoption API cannot validate its real binding.
This checkpoint leaves selected initialization/adoption unimplemented; the
rejecting CLI path remains unchanged and its zero-call mocks continue to cover
all selections.

**Decision before further implementation:** choose one explicit transport change:

1. A maintained, pinned libusb Darwin backend patch: introduce discovery-free
   initialization and exact original-entry/owned-handle adoption, ordinary
   non-seizing open, and claim failure without automatic configuration or
   replacement. This retains the existing DSL transfer/event machinery, but
   changes the dependency and its build/bundle contract; it is a backend fork.
2. A native selected IOKit transport adapter within `dslcap`: open/claim the
   retained entry directly and port the current `command_ctl_wr`/`command_ctl_rd`,
   FPGA/bulk writes, `dsl_start_transfers`/completion/cancellation and event
   handling. This is a separate transport implementation, not a handle wrapper
   or a new USB service.

After that choice, the first capture scope remains one confirmed runtime DSLogic
profile and finite buffer capture on the original claimed handle. Bootloader
upload and reconnect are explicitly unsupported. Missing/ambiguous/changed
attachment, busy/failed claim or detach must fail closed; no detach, auto-detach,
force takeover, selection fallback or reattach. Optional serial checks still
apply when a serial exists. Preserve GUI → helper → `dslcap`; the selected
transport lives in that process. No app-session or no-selector behavior changes
are needed for this transport decision.

Implementation stopped at this documented blocker, as requested, before either
overhaul. All selected requests still receive the explicit
`device_selection_unavailable` JSON/exit-2 refusal, including runtime, bootloader
and reconnect cases, without inspecting or touching their devices. The earlier
10–15 engineer-day estimate below assumed an available exact-handle adapter;
that assumption is not met by this backend and is not a current delivery promise.

### Proposed macOS lifecycle changes (design only)

Source review identifies the following minimum work for a future selected
session. **None of these driver changes is implemented by this PR.** The input
must be one retained IOKit-selected device entry, its exact uint64 entryID
generation and raw `locationID`, plus its recorded serial if present. An absent
serial is allowed; a present one must still match. Cached listing properties
cannot prove exclusive ownership or that the object survived selection/open/claim.
The adapter must bind that exact entry to one live transport object, revalidate
identity before mutation, and reject missing/ambiguous/changed entries; no
bus/address, name, first-device or same-location replacement fallback.

| Source / exact functions | Required selected-session change |
| --- | --- |
| `backend.c`: `sr_init`, `sr_exit`; `lib_main.c`: `ds_lib_init`, `ds_lib_exit`, `ds_reload_device_list`, `ds_active_device`, `open_device_instance` | Factor an explicit scan-free initialization/adoption path from the supplied selected object; do not call driver scans, reload, demo selection or the legacy `pick_device`. Current `sr_init` unconditionally calls `libusb_init`; proving exact Darwin handle adoption or introducing a selected IOKit transport adapter is the first feasibility gate. This checkout provides neither adapter nor a public IOKit-to-libusb binding; do not assume a wrapping API works on macOS. Preserve legacy initialization separately. |
| `hardware/DSL/dslogic.c`, `dscope.c`: `scan`, `DSLogic_dev_new`, `DSCope_dev_new`; `dsl.c`: `dsl_check_conf_profile`, `hw_dev_open`, `dsl_dev_open` | Extract profile/instance construction from scanning. Open only the retained selected object and claim interface 0 once, with no detach, auto-detach, force takeover or retry on another object. Move the claim before the firmware-version `command_ctl_rd` (which sends vendor OUT preparation), FPGA load and configuration; adopt the existing handle rather than reopen it. |
| `libsigrok-internal.h`: `sr_usb_dev_inst`; `dsdevice.c`: `sr_usb_dev_inst_new`, `sr_usb_dev_inst_free`; `dsl.c`: `dsl_dev_close`, `dsl_destroy_device` | Store original registry entry/generation and location bound to one retained handle, an optional recorded serial, claim ownership and a synchronized terminal-detach latch. Release/close exactly once after outstanding callbacks drain; never overwrite `usb_dev`/`devhdl` or send cleanup commands after detach. |
| `hardware/common/ezusb.c`: `ezusb_upload_firmware`, `ezusb_upload_verified`, `ezusb_reset`, `ezusb_install_firmware`, `ezusb_install_buffer` | Both verified and legacy upload paths borrow the same owned, claimed session handle. Remove selected-path fresh open/close/detach; check ownership before configuration, every CPU-reset request and every firmware chunk. If a bootloader cannot be claimed without first changing configuration, reject it. Any reset/upload that causes re-enumeration ends this session; never finish capture by following the replacement. |
| `hardware/DSL/command.c`: `command_ctl_wr`, `command_ctl_rd`; `dsl.c`: `dsl_fpga_config`, `dsl_fpga_arm`, `dsl_config_set`, `dsl_start_transfers`, `free_transfer`, `dsl_dev_acquisition_stop`; both driver files: `config_set`, `dev_open`, `dev_acquisition_start` | Pass the owned session through all transfer paths and check its claimed/live state. Every firmware/FPGA/config/register/threshold/start/stop transfer uses that one handle, including direct bulk writes and asynchronous submission/resubmission; a check only in `dev_open` is insufficient. Serialize termination against transfer submission. |
| `lib_main.c`: `process_attach_event`, `hotplug_event_listen_callback`, `update_device_handle`, `usb_hotplug_process_proc` | Disable attach/reload/reopen transactions for selected sessions. Selected-device removal or transfer `NO_DEVICE` must latch terminal failure and produce JSON exit 2/3 regardless of later ID equality. A removal-only IOKit notification may terminate the original session; no attach handler may resume the old object or follow a new entry/generation, even if location and an optional serial are identical. |

**Estimate and gates:** for one experienced C/macOS USB engineer, allow roughly
**10–15 engineer-days (2–3 work weeks)** for a first supported DSLogic runtime
profile: 2–3 days for the adapter/ownership feasibility spike, 3–5 for library
and transfer refactoring, and the remainder for mocks, review and controlled
hardware verification. This assumes a supported way to retain/claim the exact
IOKit-selected transport, available hardware with known registry generations,
coverage of both absent and present serials, and no new firmware protocol.
Broader DSLogic/DSCope modes and bootloader coverage are
roughly **4–6 engineer-weeks total**, conditional on those gates. If Darwin
adoption/exclusivity cannot be proved or startup requires re-enumeration, stop
and revise scope; this estimate does not cover replacing the whole USB backend
or enabling reconnect.

Before lifting the CLI rejection gate, require (1) source/backend proof of the
exact original entry/generation/location binding and non-seizing claim semantics,
(2) mocked call logs that prove claim precedes **every** mutation on the same
handle and zero mutations
for missing/ambiguous/changed generation/location, available serial mismatch,
busy/failed claim, detach (including reuse of an unchanged entryID) and new-object
re-enumeration, including asynchronous races and loader variants, and (3)
separately authorized macOS hardware tests of second-process contention,
disconnect/replug and supported bootloader/runtime profiles, plus no-flag
regressions. Listing cached registry properties alone satisfies none of the
ownership/transfer gates. This task performs source review and mocked parser
tests only; no local registry, production CLI, scan or capture test is used.

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
{"devices":[{"vid":10766,"pid":32,"model":"DSLogic Plus","location":"loc-20121500","generation":"100003421","serial":null,"state":"unknown"}]}
```

| Field | Type and meaning |
| --- | --- |
| `vid`, `pid` | JSON integers, unsigned 16-bit cached `idVendor`/`idProduct`, printed in decimal. Only pairs in both DSL tables in `libsigrok4DSL/hardware/DSL/dsl.h` are included. |
| `model` | JSON string from that table, independent of the cached product label. Duplicate speed profiles share one table entry. |
| `location` | JSON string `loc-` plus exactly eight lowercase hexadecimal digits from the complete nonzero 32-bit cached `locationID`; `null` if unavailable or malformed. Decimal 538055936 is `loc-20121500`; decimal 538050560 is `loc-20120000`. No masking or rounding. |
| `generation` | JSON string containing the `uint64_t` ID returned by `IORegistryEntryGetRegistryEntryID` for this exact matched entry. Canonical lowercase hexadecimal, 1–16 digits, no `0x` or leading zeros except literal `"0"`; `null` on API failure. JSON string avoids loss of high bits in clients with floating-point JSON numbers. |
| `serial` | Optional JSON string converted losslessly from the cached CFString to UTF-8, or `null` when absent/unavailable. An absent property is nonfatal and produces no warning. No case folding, normalization, replacement or truncation. JSON escaping preserves controls, quotes and backslashes. |
| `state` | Always `"unknown"`. VID/PID, product labels and `bcdDevice` do not establish bootloader/runtime, FPGA state or capture readiness. |

The coordinated selector representation is `<location>:<generation>`. Both are
required for attachment identity; serial is optional descriptive metadata, not a
selection key. Listing does not perform selection; the `--device` capture path remains unavailable.
A location describes the host/controller/port arrangement, not a globally unique
or permanent identifier. Controller changes and hub/port moves can change it.

Generation identifies the **registry entry object**, not a hardware serial or
firmware/FPGA generation. It comes from the very same entry handle used to read
VID/PID/location, with no parent, child, ancestor or other-class lookup. In legacy
fallback it is the matched `IOUSBDevice` entry's own ID; it is not mapped to or
borrowed from an `IOUSBHostDevice` object. Consumers must compare the same entry
mapping. IDs from different objects/classes cannot establish continuity merely
because VID/PID/location happen to match.

Apple's [API declaration and documentation](https://github.com/apple-oss-distributions/IOKitUser/blob/main/IOKitLib.h)
specify an ID shared across tasks but valid only within the machine's current
boot. The API's return status establishes success; it does not document zero as
an invalid successful value. A successful zero is formatted `"0"`, while any
failed call yields `null` and status 1, even if it writes zero or another value.
The [user-space implementation](https://github.com/apple-oss-distributions/IOKitUser/blob/main/IOKitLib.c)
sets the output to zero on failure, so testing status instead of the number is
essential. No claim is made that attached USB hardware normally receives ID zero.

Replug/re-enumeration that creates a replacement registry entry gives it a new
entry ID, even at the same location. The
[registry implementation](https://github.com/apple-oss-distributions/xnu/blob/main/iokit/Kernel/IORegistryEntry.cpp)
assigns an ID to an entry when first attached; retaining the same registry object
can retain its ID. The token therefore does not prove every USB session,
configuration or firmware transition was observed, and is not portable across
reboot/hosts. Enumeration/property/ID reads are a snapshot that can race removal
or re-enumeration. A selector must revalidate identity before its own device
operation; listing does not open, reserve, pin or lock the device.

Numeric properties must be integer CFNumbers that convert losslessly to a
nonnegative signed 64-bit value within the relevant unsigned bound. Booleans,
CFData, floating-point, negative and out-of-range values are rejected; a negative
CFNumber is not reinterpreted as unsigned bits. Serial CFStrings must be nonempty,
contain at most 4096 UTF-16 units / 16384 UTF-8 bytes, and convert completely with
`lossByte=0`. Empty strings, embedded NUL, unpaired surrogates, oversized values,
wrong types and allocation failures yield `null` and a diagnostic/status 1 when
the serial property is present, never a lossy serial.

Missing/malformed VID/PID prevents safe filtering: that entry is omitted, a
stderr diagnostic explains the incomplete inventory, and status is 1. Recognized
rows retain `null` for unreadable location or failed generation lookup and also
produce a diagnostic and status 1; generation is never fabricated from a USB
address, serial, topology value, other entry or a failed output parameter.
A missing serial property yields `null`, status 0 and no warning if location and
generation are complete. A **present** serial that has a wrong type or fails
strict conversion/allocation yields `null`, a diagnostic and status 1. The
property API returns NULL for an absent **or** inaccessible property; those cases
cannot be distinguished and are both optional `null` serial metadata. A null
serial does not certify that the USB descriptor's `iSerialNumber` was zero.
Optional cached product/revision properties are read but are not output or used
to infer state; their absence does not invalidate identity.
Enumeration, iterator invalidation and handle-release failures also return 1.
A successful empty result is status 0. Require status 0 and complete JSON before
using the result. Parent loss exits immediately with status 1 and may leave no
result. Stdout I/O errors detected by stdio return status 1. Without `--parent-fd`,
existing signal behavior is preserved: with default SIGPIPE handling, writing to
a closed pipe terminates by **SIGPIPE**, rather than returning status 1 or a
complete result. With `--parent-fd`, existing watcher setup ignores SIGPIPE, so
an actual broken-pipe write reports the I/O error and status 1 while the parent
is still alive. No result is guaranteed after signal termination or I/O failure.

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
| `IORegistryEntryGetRegistryEntryID(entry, &id)` | Once per recognized DSL entry, on the exact matched handle (including legacy fallback). Read the full 64-bit registry ID; check `KERN_SUCCESS` before formatting any value. No USB request, user client or device handle is opened. |
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
32-bit location values, same-entry/fallback generation mapping, zero/high-bit/max
64-bit IDs, failed-ID output rejection, optional missing versus malformed serial,
seven-field schema/order, Unicode and JSON escaping, wrong/missing CF types,
negative/floating/out-of-range numbers, strict serial failures, allocation failure,
registry/query/release/iterator faults, option conflicts/reserved tokens, parent
loss before/during fake registry calls, stdout errors and actual closed-pipe
SIGPIPE/EPIPE behavior, and capture/spool/parent
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
checking expected cached location, registry entry ID and optional serial values:

```sh
./build.dir/dslcap --list-ids
```

This revision was verified here with mocks only. The owner must bench the revised
command for installed-driver/cache behavior and USB traffic, and check that
replacement entries after replug/re-enumeration receive a changed ID. Cached
values and entry IDs cannot prove firmware state, every live USB transition or
capture readiness.

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
link libusb or IOKit. Guarded requests assert zero fake registry calls, including
both argument orders of the `--list-ids` conflict. The tests use the real parser/main, check strict macOS
`loc-xxxxxxxx` width/case/hex/overflow boundaries and canonical uint64 generation
syntax, including successful zero, maximum uint64, leading-zero and overflow
rejection. Old serial/USB topology/name/address forms, extra separators,
non-UTF-8 input, duplicate/conflicting flags, JSON escaping, JSON exit 2 and no
files are also checked.
A readable empty manifest pipe proves rejection precedes even a potentially
blocking resource preflight, with and without a valid parent watcher. The
existing fake captures/lists still verify no-flag behavior. This proves rejection
ordering and the absence of wrong-device mutation; it does not simulate or
prove live generation/location binding, successful claim-before-upload,
optional serial-change detection, claim-busy
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
