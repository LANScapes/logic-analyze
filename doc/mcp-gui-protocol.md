# Logic Analyze GUI ↔ agent protocol (channel `g`, version 1)

This document is the interface between the Logic Analyze GUI (GPL, this repository) and the Logic Analyze Agent, the proprietary MCP component in the Mac App Store edition. The two are separate programs. They share no code and communicate only as described here. The GUI side is in `DSView/pv/mcp/`. It is compiled only with `-DLANSCAPES_APPSTORE=ON`. The default and brand-only builds contain none of it.

The GUI does exactly three things on this channel:

1. it keeps a connection open, which tells the agent that the app is running;
2. it delivers App Store purchase evidence;
3. it takes part in the device lease, so the analyzer has one user at a time.

There is no message that pairs a client, widens a client's access or changes the agent's settings. Clients (pairings) are managed only in the agent's own menu-bar item.

## Lifetime

MCP is available only while Logic Analyze is open.

- MCP is off by default. The user turns it on in **Options → MCP…**, and the GUI stores this setting.
- When the GUI launches with MCP on, or when the user turns MCP on, the GUI starts the agent. The agent is the nested app `Logic Analyze Agent.app` in the GUI's bundle, and the GUI opens it with `NSWorkspace`. Then the GUI connects.
- The agent serves MCP clients only while an authenticated GUI connection is open. It exits when the GUI disconnects or quits. Turning MCP off closes the connection.
- There is no login item and no background registration. Nothing starts the GUI: not the agent, not the relay, and not an MCP client.
- If the connection drops while MCP is on, the GUI connects again every 3 s. It starts the agent again at most every 30 s. After a version error it stops trying until the user turns MCP off and on again.

## Identities

| Item | Production | TestFlight | Development |
|---|---|---|---|
| Agent bundle ID | `com.lanscapes.LogicAnalyzer.agent` | `….agent.tf` | `….agent.dev` |
| App group A (sockets) | `BVM4W42ZKJ.com.lanscapes.la.ctl` | `….la.ctl.tf` | `….la.ctl.dev` |
| GUI bundle ID | `com.lanscapes.LogicAnalyzer` (all channels) | | |
| Team | `BVM4W42ZKJ` | | |

The CMake cache variable `LANSCAPES_MCP_CHANNEL` (`production`, the default, or `testflight` or `development`) selects the column.

## Socket

- `AF_UNIX` `SOCK_STREAM`, at `<group A container>/g<epoch>`. The security epoch is `1`, so the name is `g1`. The container is `containerURLForSecurityApplicationGroupIdentifier:`, which is `~/Library/Group Containers/<group A>/`.
- The path must fit `sun_path` (104 bytes). This holds for user names of up to about 36 characters. A longer path is reported as an error.
- **The GUI checks the agent.** Before it sends anything, the GUI reads the peer's audit token (`LOCAL_PEERTOKEN`) and checks that process's dynamic code against `anchor apple generic and identifier "<agent bundle ID>"`. It also checks that the team identifier in the signing information is `BVM4W42ZKJ`. If either check fails, the GUI closes the socket. *P7 may narrow the requirement to the delivered signing channel.*
- **The agent checks the GUI.** After the GUI's first bytes arrive, the agent authenticates the peer from its audit token and dynamic code (the GUI's identifier, channel requirement and team). It re-checks the peer token on every message. A peer that fails gets no semantics.
- The agent serves one authenticated GUI connection at a time. It closes a second one.

## Framing

Each message is one frame:

```
u32 big-endian length N, 1 <= N <= 32764 (32 KiB - 4)
N bytes: one JSON object, UTF-8
```

Every object has `"v": 1` and a string `"type"`. Integers are JSON integers, never `1.0`. The agent parses strictly. It refuses duplicate keys, fractions where integers are expected, trailing data, and any missing or extra key. A refused frame closes the connection. The GUI sends exactly the keys listed below.

## Messages

### GUI → agent

| type | Keys and values | When |
|---|---|---|
| `gui_hello` | `proto`: 1, `epoch`: 1, `build`: GUI protocol build (1), `min_build`: the oldest agent build the GUI accepts (1) | first, within 2 s of connecting |
| `evidence` | `jws`: the compact JWS of `AppTransaction` (≤ 16 KiB). `device_verification_id`: this app's `AppStore.deviceVerificationID`, as a canonical hyphenated UUID | after `gui_ok` when the GUI holds evidence; after each successful read (below) |
| `lease` | `op`: `released`, `busy` or `reclaim`. `nonce`: 32 lowercase hex characters. `boot_epoch`: the agent's, from `gui_ok`. `device`: `"default"`. `device_generation`: an integer ≥ 0 | device lease (below) |

```json
{"v":1,"type":"gui_hello","proto":1,"epoch":1,"build":1,"min_build":1}
{"v":1,"type":"evidence","jws":"eyJ…","device_verification_id":"51E35FBA-…"}
{"v":1,"type":"lease","op":"released","nonce":"<32 hex>","boot_epoch":"<32 hex>","device":"default","device_generation":1}
```

The GUI sends no other messages. In particular, it sends no status reports, because the open connection is the signal that the app is running.

### Agent → GUI

| type | Keys and values |
|---|---|
| `gui_ok` | `boot_epoch`: 32 lowercase hex characters (random each time the agent starts). `build` and `min_build` are integers |
| `gui_error` | `code`: `version`. The agent then closes the connection |
| `evidence_result` | `result`: `granted`, `not_newer`, `revoked` or `rejected`. `expires_at`: epoch milliseconds for `granted`, otherwise `null` |
| `lease_request` | `nonce`, `boot_epoch`, `device` (`"default"`), `device_generation` |
| `lease_returned` | the same keys. `nonce` is the nonce of the GUI's `reclaim` |

### Handshake

1. The GUI connects, checks the agent, and sends `gui_hello` at once. The agent's deadline is 2 s.
2. The agent authenticates the GUI and checks the versions. It answers `gui_ok`, or `gui_error` `version` and closes.
3. The GUI accepts `gui_ok` only if `build` ≥ the GUI's `min_build` and `min_build` ≤ the GUI's `build`. Otherwise it closes and shows "update Logic Analyze".
4. Every later message is valid only on this connection and with this `boot_epoch`. Any message before `gui_ok`, other than `gui_error`, ends the connection.

## Purchase evidence

The GUI reads the evidence. The agent verifies it. The GUI contains no verification logic.

- **Source.** The GUI reads `AppTransaction.shared` when MCP is on, at launch or when MCP is turned on. It reads it again every 24 h while it runs. It sends `VerificationResult<AppTransaction>.jwsRepresentation` together with its own `AppStore.deviceVerificationID`, both in one `evidence` message. It sends the evidence whether StoreKit marks it verified or unverified.
- **`AppTransaction.refresh()`** can show an App Store sign-in prompt, so the GUI calls it only when the user clicks **Confirm Purchase** in the MCP pane. It never calls it automatically.
- **Binding.** The agent checks that the JWS's `deviceVerification` is base64 SHA-384 of `lower(deviceVerificationNonce) + lower(device_verification_id)`. The ID is per bundle, so the GUI must send its own ID, not the agent's.
- **Resending.** The agent accepts one `evidence` message every 5 s on each connection. The GUI keeps at least 5.5 s between evidence messages, and delays one that comes too soon. On every new connection the GUI resends the latest evidence it has. Receiving the same evidence again is harmless (`not_newer`).
- **Results.**
  - `granted` with `expires_at`: MCP is authorized until then. That is 14 days after the JWS was issued.
  - `not_newer`: the agent already holds this or newer evidence.
  - `revoked`: the App Store reports the purchase as refunded or revoked.
  - `rejected`: the evidence failed verification, or it arrived too soon after the previous one.

  The pane shows the result. If there is no evidence, or the result is `rejected`, the pane shows **Confirm Purchase**.

## Device lease

The agent owns the lease state. The GUI mirrors it, and the GUI's code calls the states the same names:

| State | Meaning |
|---|---|
| `gui-owned` | The GUI may use the analyzer. This is the state after every `gui_ok`. |
| `release-pending` | The agent sent `lease_request`, and the GUI is deciding. The agent waits up to 30 s. |
| `mcp-owned` | The GUI released the analyzer, and MCP clients may capture. |
| `reclaim-pending` | The GUI sent `reclaim`. The agent returns the analyzer when the running MCP capture ends. |

Until exact device identity exists (P8), the only device is `"default"`. `device_generation` changes when the agent sees a new GUI connection or the GUI goes away. The GUI repeats in each reply the `nonce` and `device_generation` of the request it answers. The agent ignores a lease message with a wrong `boot_epoch`, nonce or generation, and it ignores duplicates.

**Release.** When `lease_request` arrives in `gui-owned`:

- If the GUI is not using an analyzer (the demo device or a file is selected), it answers `released` at once.
- If a save is running, it answers `busy`, because a save cannot be interrupted. The MCP client gets "busy" and may try again.
- If a capture is running, or there is captured data on screen, the GUI asks the user: "An MCP client wants to use the analyzer. Hand it over?" The question closes itself after 25 s, which is inside the agent's 30 s. No answer, or **No**, means `busy`.
- To release, the GUI stops any capture and closes the analyzer by switching to the demo device. Only then does it send `released`. It tells the user that an MCP client is using the analyzer.

While the analyzer is lent (`mcp-owned` or `reclaim-pending`), the GUI never opens it. Opening a file and the demo device still work, and the default device is the newest file or the demo device.

**Reclaim.** When the user selects the analyzer in the device list, or clicks **Take Back** in the MCP pane, the GUI sends `reclaim` with a new nonce. When the matching `lease_returned` arrives, the state is `gui-owned` again. If the GUI is still idle on the demo device, it switches back to the analyzer.

**Other cases:**

- `lease_request` in `mcp-owned` (the agent lost the GUI's earlier answer): the GUI answers `released` at once.
- `lease_request` in `release-pending`: the GUI keeps deciding and answers the newest nonce.
- `lease_request` in `reclaim-pending` (the agent returned the analyzer and wants it again before it answered the reclaim): the GUI keeps the analyzer and answers `busy`.
- Disconnect in any state: the lease ends. The GUI may use the analyzer again. When the GUI disconnects, the agent exits.

If the GUI and an MCP capture still collide (for example, an analyzer attached while it was lent), the exclusive USB claim makes one side fail with "device busy". Neither side loses data.

## Errors

The GUI closes the connection on any of the following:

- a malformed or oversize frame;
- an unknown or invalid message;
- a message before `gui_ok`;
- `gui_error`;
- a failed write (with a 2 s send timeout);
- end of stream.

It logs the reason, ends the lease, and reconnects as described in Lifetime. The agent closes the connection on any protocol violation from the GUI.
