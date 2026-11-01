<!-- edition: appstore -->
# 连接 AI 助手 (MCP)

## 关于 MCP

MCP (Model Context Protocol) 把您 Mac 上的 AI 助手连接到 Logic Analyze。Claude Code、Claude Desktop 和 Cursor 是 AI 助手的示例。使用 MCP，助手可以通过此应用采集、测量和解码数据。

在您开启 MCP 之前，MCP 处于关闭状态。助手使用此应用需要以下条件：

- Logic Analyze 已打开。
- MCP 已开启。
- DSLogic 设备已连接到 Mac。没有设备，助手无法采集数据。

应用在主窗口中显示助手的每次采集。您可以停止助手的采集。

## 开启或关闭 MCP

1. 单击工具栏上的 **MCP**。**MCP** 窗口打开。
2. 单击 **开启**。

MCP 开启时，**MCP** 按钮上的圆点为绿色。助手采集数据时，圆点为橙色。

要关闭 MCP，在 **MCP** 窗口中单击 **关闭**。

## 停止助手的采集

1. 单击工具栏上的 **MCP**。
2. 在 **MCP** 窗口中单击 **停止**。

助手不采集数据时，**当前：** 行显示 **空闲**。

## 把助手连接到应用

助手通过应用包中的 **Logic Analyze Agent** 程序连接到应用。把此程序的路径提供给助手。

对于 Claude Code，在“终端”中输入以下命令：

```sh
claude mcp add logic-analyze -- "/Applications/Logic Analyze.app/Contents/Library/LoginItems/Logic Analyze Agent.app/Contents/MacOS/logic-analyze-mcp"
```

对于 Claude Desktop 和 Cursor，把以下文本添加到助手的 MCP 配置文件中：

```json
{
  "mcpServers": {
    "logic-analyze": {
      "command": "/Applications/Logic Analyze.app/Contents/Library/LoginItems/Logic Analyze Agent.app/Contents/MacOS/logic-analyze-mcp"
    }
  }
}
```

Claude Desktop 的配置文件是 `~/Library/Application Support/Claude/claude_desktop_config.json`。Cursor 的配置文件是 `~/.cursor/mcp.json`。如果文件中已有 `mcpServers` 对象，只添加 `logic-analyze` 项。

> [!NOTE]
> 如果在助手启动后才开启 MCP，请在助手中重新连接 `logic-analyze` 服务器。在 Claude Code 中，输入 `/mcp` 并重新连接服务器。退出 Claude Desktop，然后重新打开它。
<!-- end edition -->
