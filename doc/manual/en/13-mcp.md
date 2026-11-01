<!-- edition: appstore -->
# Connect an AI assistant (MCP)

## About MCP

MCP (Model Context Protocol) connects an AI assistant on your Mac to Logic Analyze. Claude Code, Claude Desktop and Cursor are examples of AI assistants. With MCP, the assistant can capture, measure and decode data through the app.

MCP is off until you turn it on. These conditions are necessary before the assistant can use the app:

- Logic Analyze is open.
- MCP is on.
- A DSLogic device has a connection to the Mac. The assistant cannot capture data without the device.

The app shows each capture of the assistant in the main window. You can stop a capture of the assistant.

## Turn MCP on or off

1. Click **MCP** on the toolbar. The **MCP** window opens.
2. Click **Turn On**.

The dot on the **MCP** button is green when MCP is on. The dot is orange when the assistant captures data.

To turn MCP off, click **Turn Off** in the **MCP** window.

## Stop a capture of the assistant

1. Click **MCP** on the toolbar.
2. In the **MCP** window, click **Stop**.

When the assistant does not capture data, the **Now:** line shows **Idle**.

## Connect the assistant to the app

The assistant connects to the app through the **Logic Analyze Agent** program in the app bundle. Give the path of this program to the assistant.

For Claude Code, type this command in Terminal:

```sh
claude mcp add logic-analyze -- "/Applications/Logic Analyze.app/Contents/Library/LoginItems/Logic Analyze Agent.app/Contents/MacOS/logic-analyze-mcp"
```

For Claude Desktop and Cursor, add this text to the MCP configuration file of the assistant:

```json
{
  "mcpServers": {
    "logic-analyze": {
      "command": "/Applications/Logic Analyze.app/Contents/Library/LoginItems/Logic Analyze Agent.app/Contents/MacOS/logic-analyze-mcp"
    }
  }
}
```

The configuration file of Claude Desktop is `~/Library/Application Support/Claude/claude_desktop_config.json`. The configuration file of Cursor is `~/.cursor/mcp.json`. If the file has an `mcpServers` object, add only the `logic-analyze` item to it.

> [!NOTE]
> If you turn MCP on after the assistant starts, connect the `logic-analyze` server again in the assistant. In Claude Code, type `/mcp` and connect the server again. Quit Claude Desktop and open it again.
<!-- end edition -->
