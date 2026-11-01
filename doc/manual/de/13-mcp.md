<!-- edition: appstore -->
# KI-Assistenten verbinden (MCP)

## Über MCP

MCP (Model Context Protocol) verbindet einen KI-Assistenten auf Ihrem Mac mit Logic Analyze. Claude Code, Claude Desktop und Cursor sind Beispiele für KI-Assistenten. Mit MCP kann der Assistent über die App Daten erfassen, messen und dekodieren.

MCP ist aus, bis Sie es einschalten. Diese Bedingungen sind notwendig, damit der Assistent die App verwenden kann:

- Logic Analyze ist geöffnet.
- MCP ist an.
- Ein DSLogic-Gerät ist mit dem Mac verbunden. Ohne das Gerät kann der Assistent keine Daten erfassen.

Die App zeigt jede Erfassung des Assistenten im Hauptfenster. Sie können eine Erfassung des Assistenten stoppen.

## MCP ein- oder ausschalten

1. Klicken Sie in der Symbolleiste auf **MCP**. Das Fenster **MCP** öffnet sich.
2. Klicken Sie auf **Einschalten**.

Der Punkt auf der Schaltfläche **MCP** ist grün, wenn MCP an ist. Der Punkt ist orange, wenn der Assistent Daten erfasst.

Um MCP auszuschalten, klicken Sie im Fenster **MCP** auf **Ausschalten**.

## Eine Erfassung des Assistenten stoppen

1. Klicken Sie in der Symbolleiste auf **MCP**.
2. Klicken Sie im Fenster **MCP** auf **Stopp**.

Wenn der Assistent keine Daten erfasst, zeigt die Zeile **Jetzt:** den Wert **Bereit**.

## Den Assistenten mit der App verbinden

Der Assistent verbindet sich über das Programm **Logic Analyze Agent** im App-Bundle mit der App. Geben Sie dem Assistenten den Pfad dieses Programms.

Für Claude Code geben Sie diesen Befehl im Terminal ein:

```sh
claude mcp add logic-analyze -- "/Applications/Logic Analyze.app/Contents/Library/LoginItems/Logic Analyze Agent.app/Contents/MacOS/logic-analyze-mcp"
```

Für Claude Desktop und Cursor fügen Sie diesen Text in die MCP-Konfigurationsdatei des Assistenten ein:

```json
{
  "mcpServers": {
    "logic-analyze": {
      "command": "/Applications/Logic Analyze.app/Contents/Library/LoginItems/Logic Analyze Agent.app/Contents/MacOS/logic-analyze-mcp"
    }
  }
}
```

Die Konfigurationsdatei von Claude Desktop ist `~/Library/Application Support/Claude/claude_desktop_config.json`. Die Konfigurationsdatei von Cursor ist `~/.cursor/mcp.json`. Wenn die Datei schon ein Objekt `mcpServers` hat, fügen Sie nur den Eintrag `logic-analyze` hinzu.

> [!NOTE]
> Wenn Sie MCP einschalten, nachdem der Assistent gestartet ist, verbinden Sie den Server `logic-analyze` im Assistenten erneut. Geben Sie in Claude Code `/mcp` ein und verbinden Sie den Server erneut. Beenden Sie Claude Desktop und öffnen Sie es erneut.
<!-- end edition -->
