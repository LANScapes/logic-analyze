<!-- edition: appstore -->
# Een AI-assistent verbinden (MCP)

## Over MCP

MCP (Model Context Protocol) verbindt een AI-assistent op uw Mac met Logic Analyze. Claude Code, Claude Desktop en Cursor zijn voorbeelden van AI-assistenten. Met MCP kan de assistent via de app gegevens opnemen, meten en decoderen.

MCP is uit totdat u het inschakelt. Deze voorwaarden zijn nodig voordat de assistent de app kan gebruiken:

- Logic Analyze is geopend.
- MCP is aan.
- Een DSLogic-apparaat is aangesloten op de Mac. Zonder het apparaat kan de assistent geen gegevens opnemen.

De app toont elke opname van de assistent in het hoofdvenster. U kunt een opname van de assistent stoppen.

## MCP in- of uitschakelen

1. Klik op de werkbalk op **MCP**. Het venster **MCP** wordt geopend.
2. Klik op **Inschakelen**.

De stip op de knop **MCP** is groen als MCP aan is. De stip is oranje als de assistent gegevens opneemt.

Klik in het venster **MCP** op **Uitschakelen** om MCP uit te schakelen.

## Een opname van de assistent stoppen

1. Klik op de werkbalk op **MCP**.
2. Klik in het venster **MCP** op **Stoppen**.

Als de assistent geen gegevens opneemt, toont de regel **Nu:** de waarde **Inactief**.

## De assistent met de app verbinden

De assistent maakt verbinding met de app via het programma **Logic Analyze Agent** in de app-bundel. Geef de assistent het pad van dit programma.

Typ voor Claude Code deze opdracht in Terminal:

```sh
claude mcp add logic-analyze -- "/Applications/Logic Analyze.app/Contents/Library/LoginItems/Logic Analyze Agent.app/Contents/MacOS/logic-analyze-mcp"
```

Voeg voor Claude Desktop en Cursor deze tekst toe aan het MCP-configuratiebestand van de assistent:

```json
{
  "mcpServers": {
    "logic-analyze": {
      "command": "/Applications/Logic Analyze.app/Contents/Library/LoginItems/Logic Analyze Agent.app/Contents/MacOS/logic-analyze-mcp"
    }
  }
}
```

Het configuratiebestand van Claude Desktop is `~/Library/Application Support/Claude/claude_desktop_config.json`. Het configuratiebestand van Cursor is `~/.cursor/mcp.json`. Als het bestand al een object `mcpServers` heeft, voeg dan alleen het item `logic-analyze` toe.

> [!NOTE]
> Als u MCP inschakelt nadat de assistent is gestart, verbind de server `logic-analyze` dan opnieuw in de assistent. Typ in Claude Code `/mcp` en verbind de server opnieuw. Stop Claude Desktop en open het opnieuw.
<!-- end edition -->
