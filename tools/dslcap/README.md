# dslcap

`dslcap` captures DSLogic samples without the graphical interface. It prints one
JSON result on stdout. Library diagnostics use stderr. A successful capture
creates `<base>.bin`; an existing file at that path is never replaced.

```text
dslcap --list [--res DIR] [--parent-fd N]
dslcap --channels 0,1 --samplerate 10000000 --samples 1000000
       [--vth 1.6] [--mode buffer|stream] [--trigger CH[:R|F|C|1|0]]
       [--trigpos PERCENT] [--timeout SEC] [--res DIR] [--parent-fd N]
       --out /path/base
```

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
