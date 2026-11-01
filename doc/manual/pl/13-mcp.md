<!-- edition: appstore -->
# Połączenie asystenta AI (MCP)

## Informacje o MCP

MCP (Model Context Protocol) łączy asystenta AI na Twoim Macu z aplikacją Logic Analyze. Claude Code, Claude Desktop i Cursor to przykłady asystentów AI. Dzięki MCP asystent może rejestrować, mierzyć i dekodować dane przez aplikację.

MCP jest wyłączony, dopóki go nie włączysz. Te warunki są konieczne, aby asystent mógł używać aplikacji:

- Aplikacja Logic Analyze jest otwarta.
- MCP jest włączony.
- Urządzenie DSLogic jest podłączone do Maca. Bez urządzenia asystent nie może rejestrować danych.

Aplikacja pokazuje każdą rejestrację asystenta w oknie głównym. Możesz zatrzymać rejestrację asystenta.

## Włączanie i wyłączanie MCP

1. Kliknij na pasku narzędzi **MCP**. Otworzy się okno **MCP**.
2. Kliknij **Włącz**.

Kropka na przycisku **MCP** jest zielona, gdy MCP jest włączony. Kropka jest pomarańczowa, gdy asystent rejestruje dane.

Aby wyłączyć MCP, kliknij **Wyłącz** w oknie **MCP**.

## Zatrzymanie rejestracji asystenta

1. Kliknij na pasku narzędzi **MCP**.
2. W oknie **MCP** kliknij **Zatrzymaj**.

Gdy asystent nie rejestruje danych, wiersz **Teraz:** pokazuje **Bezczynny**.

## Połączenie asystenta z aplikacją

Asystent łączy się z aplikacją przez program **Logic Analyze Agent** w pakiecie aplikacji. Podaj asystentowi ścieżkę tego programu.

Dla Claude Code wpisz to polecenie w Terminalu:

```sh
claude mcp add logic-analyze -- "/Applications/Logic Analyze.app/Contents/Library/LoginItems/Logic Analyze Agent.app/Contents/MacOS/logic-analyze-mcp"
```

Dla Claude Desktop i Cursor dodaj ten tekst do pliku konfiguracji MCP asystenta:

```json
{
  "mcpServers": {
    "logic-analyze": {
      "command": "/Applications/Logic Analyze.app/Contents/Library/LoginItems/Logic Analyze Agent.app/Contents/MacOS/logic-analyze-mcp"
    }
  }
}
```

Plik konfiguracji Claude Desktop to `~/Library/Application Support/Claude/claude_desktop_config.json`. Plik konfiguracji Cursor to `~/.cursor/mcp.json`. Jeśli plik ma już obiekt `mcpServers`, dodaj do niego tylko element `logic-analyze`.

> [!NOTE]
> Jeśli włączysz MCP po uruchomieniu asystenta, połącz ponownie serwer `logic-analyze` w asystencie. W Claude Code wpisz `/mcp` i połącz ponownie serwer. Zakończ Claude Desktop i otwórz go ponownie.
<!-- end edition -->
