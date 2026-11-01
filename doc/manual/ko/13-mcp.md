<!-- edition: appstore -->
# AI 어시스턴트 연결 (MCP)

## MCP 정보

MCP(Model Context Protocol)는 Mac의 AI 어시스턴트를 Logic Analyze에 연결합니다. Claude Code, Claude Desktop, Cursor는 AI 어시스턴트의 예입니다. MCP를 사용하면 어시스턴트가 앱을 통해 데이터를 캡처, 측정, 디코드할 수 있습니다.

MCP는 켤 때까지 꺼져 있습니다. 어시스턴트가 앱을 사용하려면 다음 조건이 필요합니다.

- Logic Analyze가 열려 있습니다.
- MCP가 켜져 있습니다.
- DSLogic 장치가 Mac에 연결되어 있습니다. 장치가 없으면 어시스턴트는 데이터를 캡처할 수 없습니다.

앱은 어시스턴트의 각 캡처를 주 창에 표시합니다. 어시스턴트의 캡처를 정지할 수 있습니다.

## MCP 켜기 또는 끄기

1. 도구 막대에서 **MCP**를 클릭하십시오. **MCP** 창이 열립니다.
2. **켜기**를 클릭하십시오.

MCP가 켜져 있으면 **MCP** 버튼의 점이 녹색입니다. 어시스턴트가 데이터를 캡처하면 점이 주황색입니다.

MCP를 끄려면 **MCP** 창에서 **끄기**를 클릭하십시오.

## 어시스턴트의 캡처 정지

1. 도구 막대에서 **MCP**를 클릭하십시오.
2. **MCP** 창에서 **중지**를 클릭하십시오.

어시스턴트가 데이터를 캡처하지 않으면 **현재:** 줄에 **대기 중**이 표시됩니다.

## 어시스턴트를 앱에 연결

어시스턴트는 앱 번들 안의 **Logic Analyze Agent** 프로그램을 통해 앱에 연결합니다. 이 프로그램의 경로를 어시스턴트에 지정하십시오.

Claude Code에서는 터미널에 다음 명령을 입력하십시오.

```sh
claude mcp add logic-analyze -- "/Applications/Logic Analyze.app/Contents/Library/LoginItems/Logic Analyze Agent.app/Contents/MacOS/logic-analyze-mcp"
```

Claude Desktop과 Cursor에서는 어시스턴트의 MCP 구성 파일에 다음 텍스트를 추가하십시오.

```json
{
  "mcpServers": {
    "logic-analyze": {
      "command": "/Applications/Logic Analyze.app/Contents/Library/LoginItems/Logic Analyze Agent.app/Contents/MacOS/logic-analyze-mcp"
    }
  }
}
```

Claude Desktop의 구성 파일은 `~/Library/Application Support/Claude/claude_desktop_config.json`입니다. Cursor의 구성 파일은 `~/.cursor/mcp.json`입니다. 파일에 `mcpServers` 객체가 이미 있으면 `logic-analyze` 항목만 추가하십시오.

> [!NOTE]
> 어시스턴트를 시작한 후에 MCP를 켜면 어시스턴트에서 `logic-analyze` 서버를 다시 연결하십시오. Claude Code에서는 `/mcp`를 입력하고 서버를 다시 연결하십시오. Claude Desktop은 종료하고 다시 여십시오.
<!-- end edition -->
