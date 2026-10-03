# Logic Analyze GUI ↔ agent protocol (channel `g`, version 1, build 2)

This is the interface between the Logic Analyze GUI (GPL, this repository) and the Logic Analyze Agent, the proprietary MCP component of the Mac App Store edition. The two programs share no code; this file is the contract. The GUI side is in `DSView/pv/mcp/` and is compiled only with `-DLANSCAPES_APPSTORE=ON`.

The GUI is the only program that opens the analyzer. When an MCP client asks for a capture, the agent asks the GUI, and the GUI runs it in its normal session, on screen, as if the user had pressed Start. The user watches the waveform live; the toolbar shows **MCP ●** while an MCP capture drives the app. The GUI then writes the data to the agent's staging directory in exactly the format `dslcap` writes, so the MCP server's import path is unchanged.

## Lifetime

- MCP is off by default. The user turns it on in **Options → MCP…**, and the GUI stores the setting.
- With MCP on, the GUI opens the nested `Logic Analyze Agent.app` with `NSWorkspace` and connects. If the connection drops, it connects again every 3 s and reopens the agent at most every 30 s. After a version error it stops until MCP is turned off and on.
- The agent serves only while the GUI is connected, and exits at once when the GUI disconnects. Nothing else starts the agent or the GUI.

## Identities, socket and staging directory

| | Production | TestFlight | Development |
|---|---|---|---|
| Agent identifier | `com.lanscapes.LogicAnalyzer.agent` | `….agent.tf` | `….agent.dev` |
| App group A (socket) | `BVM4W42ZKJ.com.lanscapes.la.ctl` | `….la.ctl.tf` | `….la.ctl.dev` |
| App group B (capture store) | `BVM4W42ZKJ.com.lanscapes.la.cap` | `….la.cap.tf` | `….la.cap.dev` |

The CMake variable `LANSCAPES_MCP_CHANNEL` (`production`, `testflight` or `development`) picks the column. The team is `BVM4W42ZKJ`. **The GUI's entitlements must list both app groups, A and B** (`com.apple.security.application-groups`), because it now writes captures into group B.

The socket is `AF_UNIX` `SOCK_STREAM` at `~/Library/Group Containers/<group A>/g1`.

The staging directory is `~/Library/Group Containers/<group B>/captures/staging/`. The agent creates it (mode 0700) and sweeps it; the GUI only creates files in it, and fails the capture (`failed`) if it does not exist. The GUI finds the container with `containerURLForSecurityApplicationGroupIdentifier`; no path goes over the socket.

- **The GUI checks the agent** before it sends anything. The peer's audit token (`LOCAL_PEERTOKEN`) must name code that satisfies `anchor apple generic and identifier "<agent identifier>"` and whose team identifier is `BVM4W42ZKJ`. If not, the GUI closes the socket.
- **The agent checks the GUI** the same way, against the GUI's identifier and team.
- The agent serves one GUI connection at a time.

## Framing

Each frame is a big-endian `u32` length N (1 ≤ N ≤ 32764), then N bytes of one UTF-8 JSON object. Every object has `"v": 1` and a `"type"`. Integers are JSON integers. Unknown keys in a known message are ignored.

## Messages

| Direction | Message |
|---|---|
| GUI → agent | `{"v":1,"type":"gui_hello","proto":1,"epoch":1,"build":2,"min_build":2}` |
| agent → GUI | `{"v":1,"type":"gui_ok","build":B,"min_build":M}` or `{"v":1,"type":"gui_error","code":"version"}` |
| agent → GUI | `{"v":1,"type":"devices","id":N}` |
| GUI → agent | `{"v":1,"type":"devices_ok","id":N,"devices":["Demo Device","DSLogic Plus"],"selected":"DSLogic Plus"}` |
| agent → GUI | `{"v":1,"type":"capture","id":N,"name":"<base name>","req":{…}}` |
| GUI → agent | `{"v":1,"type":"capture_started","id":N}` |
| GUI → agent | `{"v":1,"type":"capture_done","id":N,"name":"<base name>","meta":{…}}` |
| GUI → agent | `{"v":1,"type":"capture_error","id":N,"code":"busy"\|"no_device"\|"unsupported"\|"stopped"\|"failed","message":"…"}` |
| agent → GUI | `{"v":1,"type":"capture_cancel","id":N}` |

`id` is a JSON integer ≥ 0 chosen by the agent; every reply carries the request's `id`.

**Handshake.** The GUI sends `gui_hello` as soon as it connects (the agent waits 2 s). The agent answers `gui_ok`, or `gui_error` and closes. The GUI accepts `gui_ok` only if `build` ≥ its `min_build` (2) and `min_build` ≤ its `build` (2). Otherwise it closes and asks the user to update. Build 2 is this capture protocol; build 1 was the device lease (`lease`, `lease_request`, `lease_returned`), which no longer exists. The agent should send `"build":2,"min_build":2`.

**devices.** The GUI answers at once with the names in its device list, in list order (the demo device first), and the name of the device selected in the GUI (a file opened as a device shows its file name). It does not open or change any device.

### capture

`name` is the capture's file base name, chosen by the agent (the MCP `capture_id`): 1 to 128 characters of `A-Z a-z 0-9 _ -`, not starting with `-`. Anything else is `unsupported`.

`req` carries the fields of the MCP `dslogic_capture` tool after the server's own checks, with the same meaning as `dslcap`'s options:

| Field | Type | Default | `dslcap` option |
|---|---|---|---|
| `channels` | array of distinct integers 0–15 | required | `--channels` |
| `samplerate_hz` | integer > 0, one of the device's rates | required | `--samplerate` |
| `samples` | integer ≥ 1, samples per channel | 1000000 | `--samples` |
| `duration_s` | number > 0; used only when `samples` is absent: samples = max(1, floor(duration_s × samplerate_hz)) | | |
| `threshold_v` | number 0–5 | 1.6 | `--vth` |
| `mode` | `"buffer"` or `"stream"` | `"buffer"` | `--mode` |
| `trigger_channel` | integer, one of `channels`, or null | null (no trigger) | `--trigger CH` |
| `trigger_edge` | `"R"` rising, `"F"` falling, `"C"` either edge, `"1"` high, `"0"` low | `"R"` | `--trigger CH:E` |
| `trigger_position_percent` | integer 0–100 | 10 | `--trigpos` |
| `timeout_ms` | integer ≥ 1 | 30000 | `--timeout` (seconds) |

Other keys (such as `label`) are ignored. A field of the wrong type or out of range is `unsupported`.

The GUI and `dslcap` turn the request into device settings with the same code (`tools/dslcap/capcore.c`, `cap_apply()`): operation mode first, then the channel mode with the most channels that offers the rate, only the requested channels enabled, the threshold (devices without one: an explicit `threshold_v` is `unsupported`), the rate, the depth (`samples` rounded up to the driver's 1024-sample alignment in buffer mode), and a simple trigger on one channel at `trigger_position_percent`.

The GUI, on `capture`:

1. Answers `capture_error` `busy` at once if a capture (the user's or another MCP one) or a save is running.
2. Uses the selected device if it is an analyzer. If the demo device or a file is selected, it switches to the first analyzer in the device list; with none, `no_device`.
3. Applies the request through `cap_apply()`; a setting the device does not have is `unsupported`. The toolbar then shows the new rate and depth, and only the requested channels are enabled.
4. Starts the capture as if the user pressed Start, sends `capture_started`, and shows **MCP ●** in the toolbar. The waveform draws live.
5. Ends the capture after `timeout_ms` from `capture_started` if it has not finished, with `capture_error` `failed` (`"capture timed out"`).
6. On completion writes `<name>.bin` and then `<name>.json` into the staging directory (below) and sends `capture_done` with `meta` equal to the JSON file's contents.

**Stops.**

- The user presses Stop during an MCP capture: if any samples arrived, the GUI writes what it has and sends `capture_done` with `"stopped_by_user": true` in `meta` (`samples` < `samples_requested`). With no samples at all it sends `capture_error` `stopped`.
- `capture_cancel` with the running capture's `id`: the GUI stops the capture, writes nothing, and sends `capture_error` `stopped` (`"cancelled by the agent"`). A `capture_cancel` for any other `id` is ignored.
- A device error, detach, overflow or a write failure: `capture_error` `failed` with `dslcap`'s message for it (for example `"device detached during capture"`, `"cannot write capture data"`).
- If the connection drops during an MCP capture, the capture goes on as the user's own; nothing is written.

Only one MCP capture runs at a time. After `capture_done` or `capture_error` the data stays on screen as an ordinary capture; the user can save it.

### Error codes

| `code` | Meaning |
|---|---|
| `busy` | The user's capture, another MCP capture or a save is running. Try again later. |
| `no_device` | No analyzer in the device list. |
| `unsupported` | A malformed request, or a setting the device does not offer (rate, channel/rate combination, threshold, trigger channel). `message` is `dslcap`'s message for it where there is one. |
| `stopped` | Cancelled by the agent, or stopped by the user before any sample arrived. |
| `failed` | Anything else: timeout, device error, the staging directory is missing, a write failed. |

## Files in the staging directory

The layout is exactly `dslcap`'s; both are written by the same code (`tools/dslcap/capcore.c`, `cap_write_bin()` and `cap_print_report()`).

**`<name>.bin`**: for each enabled channel in ascending channel order, that channel's samples packed LSB-first in little-endian 64-bit words, `words_per_channel` = ceil(`samples` / 64) words (8 bytes each) per channel, channel after channel. Bits past `samples` in the last word are zero. The file size is `len(channels) × words_per_channel × 8`. Written to a temporary name and published with `link()`, so it never replaces an existing file.

**`<name>.json`**: one JSON object, the same record `dslcap` prints on stdout, followed by a newline:

```json
{"device":"DSLogic Plus","samplerate":10000000,"samples_requested":1000000,"samples":1000000,
 "words_per_channel":15625,"channels":[0,1],"vth":1.600,"mode":"buffer","format":"cross",
 "trigger":"0:R","trigger_pos":100352,"timed_out":false,"lib_error_event":0,"elapsed_s":0.123,
 "bin":"<name>.bin","limit_samples":1000448,"packet_error":false,"overflow":false}
```

- `samplerate`, `limit_samples` and `vth` are read back from the device (`vth` is null on devices without a threshold).
- `trigger` is `"CH:E"` as `dslcap`'s `--trigger` takes it, or null; `trigger_pos` is the trigger's sample index reported by the device, or -1.
- `format` is the device's wire format (`"split"` or `"cross"`); the `.bin` layout does not depend on it.
- `bin` is the `.bin` file's name, relative to the staging directory.
- The GUI adds `"stopped_by_user": true` when the user stopped the capture early; the key is absent otherwise. `dslcap` never writes it.

`<name>.json` is written after `<name>.bin`, with the same temporary-then-`link()` publishing, so its presence means both files are complete. The agent owns both files from then on (it imports or sweeps them).

## Errors

The GUI closes the connection on a malformed or oversize frame, an unknown message type, a `devices`, `capture` or `capture_cancel` before `gui_ok`, `gui_error`, a failed write (2 s send timeout) or end of stream. It then reconnects as described in Lifetime.

## Parity with dslcap

`dslcap` stays as a developer tool only because it gives the same result as an in-app MCP capture. `tests/capture_parity` (ctest `capture_parity`) runs one request through `dslcap --device demo` and through the GUI's capture path (offscreen) on the demo device's deterministic pattern, and compares `<name>.json` (except `elapsed_s` and `bin`) and `<name>.bin` byte for byte.
