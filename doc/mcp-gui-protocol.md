# Logic Analyze GUI ↔ agent protocol (channel `g`, version 1)

This is the interface between the Logic Analyze GUI (GPL, this repository) and the Logic Analyze Agent, the proprietary MCP component of the Mac App Store edition. The two programs share no code. The GUI side is in `DSView/pv/mcp/` and is compiled only with `-DLANSCAPES_APPSTORE=ON`.

On this channel the GUI does two things: it keeps a connection open, which is what lets the agent serve, and it lends the analyzer to MCP clients and takes it back. There is no purchase check and no client pairing.

## Lifetime

- MCP is off by default. The user turns it on in **Options → MCP…**, and the GUI stores the setting.
- With MCP on, the GUI opens the nested `Logic Analyze Agent.app` with `NSWorkspace` and connects. If the connection drops, it connects again every 3 s and reopens the agent at most every 30 s. After a version error it stops until MCP is turned off and on.
- The agent serves only while the GUI is connected, and exits at once when the GUI disconnects. Nothing else starts the agent or the GUI.

## Identities and socket

| | Production | TestFlight | Development |
|---|---|---|---|
| Agent identifier | `com.lanscapes.LogicAnalyzer.agent` | `….agent.tf` | `….agent.dev` |
| App group (socket) | `BVM4W42ZKJ.com.lanscapes.la.ctl` | `….la.ctl.tf` | `….la.ctl.dev` |

The CMake variable `LANSCAPES_MCP_CHANNEL` (`production`, `testflight` or `development`) picks the column. The team is `BVM4W42ZKJ`.

The socket is `AF_UNIX` `SOCK_STREAM` at `~/Library/Group Containers/<app group>/g1`.

- **The GUI checks the agent** before it sends anything. The peer's audit token (`LOCAL_PEERTOKEN`) must name code that satisfies `anchor apple generic and identifier "<agent identifier>"` and whose team identifier is `BVM4W42ZKJ`. If not, the GUI closes the socket.
- **The agent checks the GUI** the same way, against the GUI's identifier and team.
- The agent serves one GUI connection at a time.

## Framing and messages

Each frame is a big-endian `u32` length N (1 ≤ N ≤ 32764), then N bytes of one UTF-8 JSON object. Every object has `"v": 1` and a `"type"`. Integers are JSON integers.

| Direction | Message |
|---|---|
| GUI → agent | `{"v":1,"type":"gui_hello","proto":1,"epoch":1,"build":1,"min_build":1}` |
| agent → GUI | `{"v":1,"type":"gui_ok","build":B,"min_build":M}` or `{"v":1,"type":"gui_error","code":"version"}` |
| agent → GUI | `{"v":1,"type":"lease_request"}`, `{"v":1,"type":"lease_returned"}` |
| GUI → agent | `{"v":1,"type":"lease","op":"released"}`, `"busy"` or `"reclaim"` |

**Handshake.** The GUI sends `gui_hello` as soon as it connects (the agent waits 2 s). The agent answers `gui_ok`, or `gui_error` and closes. The GUI accepts `gui_ok` only if `build` ≥ its `min_build` (1) and `min_build` ≤ its `build` (1). Otherwise it closes and asks the user to update.

## Device lease

The analyzer has one user at a time. After `gui_ok` the GUI holds it.

**Release.** On `lease_request`:

- if the GUI is not using an analyzer (the demo device or a file is selected), it answers `released` at once;
- if a save is running, it answers `busy`;
- if a capture is running or captured data is on screen, it asks the user "An MCP client wants to use the analyzer. Hand it over?" The question closes itself after 25 s (the agent waits 30 s). No answer, or **No**, means `busy`.

To release, the GUI stops any capture and switches to the demo device, then sends `released`. While the analyzer is lent the GUI never opens it; files and the demo device still work.

**Reclaim.** When the user selects the analyzer or clicks **Take Back**, the GUI sends `reclaim`. The agent answers `lease_returned` once any running MCP capture ends, and the GUI switches back to the analyzer if it is still idle on the demo device.

**Repeats.** A `lease_request` while the GUI is deciding gets the one answer. One while the analyzer is lent gets `released` again. One while the GUI's `reclaim` is pending gets `busy`, and the GUI keeps the analyzer. A `lease_returned` when no reclaim is pending is ignored.

**Disconnect.** The lease ends and the GUI may use the analyzer again; the agent exits.

If the two still collide on USB, the exclusive claim makes one side fail with "device busy".

## Errors

The GUI closes the connection on a malformed or oversize frame, an unknown message, a lease message before `gui_ok`, `gui_error`, a failed write (2 s send timeout) or end of stream. It then reconnects as described in Lifetime.
