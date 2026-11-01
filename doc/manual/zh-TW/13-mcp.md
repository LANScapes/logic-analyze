<!-- edition: appstore -->
# 連接 AI 助理 (MCP)

## 關於 MCP

MCP (Model Context Protocol) 把您 Mac 上的 AI 助理連接到 Logic Analyze。Claude Code、Claude Desktop 和 Cursor 是 AI 助理的範例。使用 MCP，助理可以透過此應用程式擷取、量測和解碼資料。

在您開啟 MCP 之前，MCP 為關閉狀態。助理使用此應用程式需要以下條件：

- Logic Analyze 已開啟。
- MCP 已開啟。
- DSLogic 裝置已連接到 Mac。沒有裝置，助理無法擷取資料。

應用程式在主視窗中顯示助理的每次擷取。您可以停止助理的擷取。

## 開啟或關閉 MCP

1. 按一下工具列上的 **MCP**。**MCP** 視窗開啟。
2. 按一下 **開啟**。

MCP 開啟時，**MCP** 按鈕上的圓點為綠色。助理擷取資料時，圓點為橘色。

要關閉 MCP，在 **MCP** 視窗中按一下 **關閉**。

## 停止助理的擷取

1. 按一下工具列上的 **MCP**。
2. 在 **MCP** 視窗中按一下 **停止**。

助理不擷取資料時，**目前：** 行顯示 **閒置**。

## 把助理連接到應用程式

助理透過應用程式套件中的 **Logic Analyze Agent** 程式連接到應用程式。把此程式的路徑提供給助理。

對於 Claude Code，在「終端機」中輸入以下指令：

```sh
claude mcp add logic-analyze -- "/Applications/Logic Analyze.app/Contents/Library/LoginItems/Logic Analyze Agent.app/Contents/MacOS/logic-analyze-mcp"
```

對於 Claude Desktop 和 Cursor，把以下文字加入助理的 MCP 設定檔中：

```json
{
  "mcpServers": {
    "logic-analyze": {
      "command": "/Applications/Logic Analyze.app/Contents/Library/LoginItems/Logic Analyze Agent.app/Contents/MacOS/logic-analyze-mcp"
    }
  }
}
```

Claude Desktop 的設定檔是 `~/Library/Application Support/Claude/claude_desktop_config.json`。Cursor 的設定檔是 `~/.cursor/mcp.json`。如果檔案中已有 `mcpServers` 物件，只加入 `logic-analyze` 項目。

> [!NOTE]
> 如果在助理啟動後才開啟 MCP，請在助理中重新連接 `logic-analyze` 伺服器。在 Claude Code 中，輸入 `/mcp` 並重新連接伺服器。結束 Claude Desktop，然後重新開啟它。
<!-- end edition -->
