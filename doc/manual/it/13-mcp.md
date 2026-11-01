<!-- edition: appstore -->
# Collegare un assistente IA (MCP)

## Informazioni su MCP

MCP (Model Context Protocol) collega un assistente IA del Mac a Logic Analyze. Claude Code, Claude Desktop e Cursor sono esempi di assistenti IA. Con MCP, l'assistente può acquisire, misurare e decodificare dati tramite l'applicazione.

MCP è disattivato finché non lo si attiva. Queste condizioni sono necessarie perché l'assistente possa usare l'applicazione:

- Logic Analyze è aperta.
- MCP è attivato.
- Un dispositivo DSLogic è collegato al Mac. Senza il dispositivo, l'assistente non può acquisire dati.

L'applicazione mostra ogni acquisizione dell'assistente nella finestra principale. È possibile fermare un'acquisizione dell'assistente.

## Attivare o disattivare MCP

1. Fare clic su **MCP** nella barra degli strumenti. Si apre la finestra **MCP**.
2. Fare clic su **Attiva**.

Il punto sul pulsante **MCP** è verde quando MCP è attivato. Il punto è arancione quando l'assistente acquisisce dati.

Per disattivare MCP, fare clic su **Disattiva** nella finestra **MCP**.

## Fermare un'acquisizione dell'assistente

1. Fare clic su **MCP** nella barra degli strumenti.
2. Nella finestra **MCP**, fare clic su **Ferma**.

Quando l'assistente non acquisisce dati, la riga **Ora:** mostra **Inattivo**.

## Collegare l'assistente all'applicazione

L'assistente si collega all'applicazione tramite il programma **Logic Analyze Agent**, che si trova nel pacchetto dell'applicazione. Dare all'assistente il percorso di questo programma.

Per Claude Code, digitare questo comando nel Terminale:

```sh
claude mcp add logic-analyze -- "/Applications/Logic Analyze.app/Contents/Library/LoginItems/Logic Analyze Agent.app/Contents/MacOS/logic-analyze-mcp"
```

Per Claude Desktop e Cursor, aggiungere questo testo al file di configurazione MCP dell'assistente:

```json
{
  "mcpServers": {
    "logic-analyze": {
      "command": "/Applications/Logic Analyze.app/Contents/Library/LoginItems/Logic Analyze Agent.app/Contents/MacOS/logic-analyze-mcp"
    }
  }
}
```

Il file di configurazione di Claude Desktop è `~/Library/Application Support/Claude/claude_desktop_config.json`. Il file di configurazione di Cursor è `~/.cursor/mcp.json`. Se il file ha già un oggetto `mcpServers`, aggiungere solo la voce `logic-analyze`.

> [!NOTE]
> Se si attiva MCP dopo l'avvio dell'assistente, collegare di nuovo il server `logic-analyze` nell'assistente. In Claude Code, digitare `/mcp` e collegare di nuovo il server. Uscire da Claude Desktop e aprirlo di nuovo.
<!-- end edition -->
