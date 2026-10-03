# dslcap resource verification

`dslcap` captures without the GUI. Its resource directory is selected by `--res
DIR`, `DSLCAP_RES`, or the existing executable/install resource lookup.

```text
dslcap --channels 0,1 --samplerate 10000000 --samples 1000000
       [--vth 1.6] [--mode buffer|stream] [--trigger CH[:R|F|C|1|0]]
       [--trigpos PERCENT] [--timeout SEC] [--res DIR] [--res-manifest FD]
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
