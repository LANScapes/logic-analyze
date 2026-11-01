<!-- edition: appstore -->
# AI アシスタントを接続する (MCP)

## MCP について

MCP (Model Context Protocol) は、Mac 上の AI アシスタントを Logic Analyze に接続します。Claude Code、Claude Desktop、Cursor は AI アシスタントの例です。MCP を使うと、アシスタントはアプリを通してデータをキャプチャ、測定、デコードできます。

MCP は、オンにするまでオフです。アシスタントがアプリを使うには、次の条件が必要です。

- Logic Analyze が開いている。
- MCP がオンである。
- DSLogic デバイスが Mac に接続されている。デバイスがないと、アシスタントはデータをキャプチャできません。

アプリは、アシスタントの各キャプチャをメインウィンドウに表示します。アシスタントのキャプチャは停止できます。

## MCP をオンまたはオフにする

1. ツールバーの **MCP** をクリックします。**MCP** ウィンドウが開きます。
2. **オンにする** をクリックします。

MCP がオンのとき、**MCP** ボタンの点は緑色です。アシスタントがデータをキャプチャしているとき、点はオレンジ色です。

MCP をオフにするには、**MCP** ウィンドウで **オフにする** をクリックします。

## アシスタントのキャプチャを停止する

1. ツールバーの **MCP** をクリックします。
2. **MCP** ウィンドウで **停止** をクリックします。

アシスタントがデータをキャプチャしていないとき、**現在：** の行は **待機中** を表示します。

## アシスタントをアプリに接続する

アシスタントは、アプリバンドルの中の **Logic Analyze Agent** プログラムを通してアプリに接続します。このプログラムのパスをアシスタントに設定します。

Claude Code では、ターミナルで次のコマンドを入力します。

```sh
claude mcp add logic-analyze -- "/Applications/Logic Analyze.app/Contents/Library/LoginItems/Logic Analyze Agent.app/Contents/MacOS/logic-analyze-mcp"
```

Claude Desktop と Cursor では、アシスタントの MCP 設定ファイルに次のテキストを追加します。

```json
{
  "mcpServers": {
    "logic-analyze": {
      "command": "/Applications/Logic Analyze.app/Contents/Library/LoginItems/Logic Analyze Agent.app/Contents/MacOS/logic-analyze-mcp"
    }
  }
}
```

Claude Desktop の設定ファイルは `~/Library/Application Support/Claude/claude_desktop_config.json` です。Cursor の設定ファイルは `~/.cursor/mcp.json` です。ファイルに `mcpServers` オブジェクトがすでにある場合は、`logic-analyze` の項目だけを追加します。

> [!NOTE]
> アシスタントの起動後に MCP をオンにした場合は、アシスタントで `logic-analyze` サーバーを再び接続します。Claude Code では、`/mcp` と入力してサーバーを再び接続します。Claude Desktop は、終了して再び開きます。
<!-- end edition -->
