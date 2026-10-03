# dslcap

`dslcap` is a headless DSLogic capture tool that uses libsigrok4DSL.
It prints one JSON result on stdout. Diagnostic logs go to stderr.

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

## Hardware-free tests

Run these commands from the repository root. They compile and run test
harnesses only. They do not initialize the device library, scan for devices,
or capture samples.

The spool harness includes parsing tests for the default, levels `0..5`,
out-of-range and nonnumeric values, and duplicate options. It also checks
the existing capture-data and output-file behavior. The startup harness
intercepts the CLI at its first libsigrok4DSL call, before initialization.
It checks the selected level, JSON argument errors before that call, and
the real logger's stderr routing. No device drivers are linked into either
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
