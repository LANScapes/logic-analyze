# Collection lifecycle regression

`test_collect_lifecycle.c` exercises the actual collection entry points and worker
in `lib_main.c` with real GLib threads and joins. Session operations and acquisition
are mocked. It never initializes the device library, discovers devices, starts
hotplug, or links USB/registry libraries. The libusb header is needed for the
library's types only.

From a configured project build on macOS or Linux:

```sh
cmake --build build --target ds_test_collect_lifecycle
build.dir/ds_test_collect_lifecycle
```

Or compile the harness alone on macOS, using existing Homebrew dependencies:

```sh
cc -std=c99 -O2 -Ilibsigrok4DSL -Icommon \
  -I"$(pkg-config --variable=includedir libusb-1.0)" \
  $(pkg-config --cflags glib-2.0) \
  libsigrok4DSL/tests/test_collect_lifecycle.c \
  $(pkg-config --libs glib-2.0) -Wl,-dead_strip -o /tmp/test_collect_lifecycle
/tmp/test_collect_lifecycle
```

On Linux replace `-Wl,-dead_strip` with
`-ffunction-sections -fdata-sections -Wl,--gc-sections -pthread`.

The six cases check normal and error completion callback restart rejection,
completion before the creator publishes the worker handle, queued restart while
the old callback is held open, 50 repeat captures followed by Stop and a single
capture, and Stop immediately after Start. Condition waits have three-second
deadlines and the entire suite has a 15-second alarm. Self-joins fail immediately
at the instrumented join boundary; all valid joins call real GLib. The harness
checks that each collection thread is joined exactly once.

The existing hosted Build workflow compiles the production change and runs its
spool regression. This new explicit test target must be built and run separately;
the workflow is unchanged.
