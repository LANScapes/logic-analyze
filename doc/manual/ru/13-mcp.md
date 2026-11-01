<!-- edition: appstore -->
# Подключение ИИ-ассистента (MCP)

## О MCP

MCP (Model Context Protocol) подключает ИИ-ассистента на вашем Mac к Logic Analyze. Claude Code, Claude Desktop и Cursor — примеры ИИ-ассистентов. С MCP ассистент может захватывать, измерять и декодировать данные через приложение.

MCP выключен, пока вы его не включите. Чтобы ассистент мог использовать приложение, необходимы эти условия:

- Logic Analyze открыт.
- MCP включен.
- Устройство DSLogic подключено к Mac. Без устройства ассистент не может захватывать данные.

Приложение показывает каждый захват ассистента в главном окне. Вы можете остановить захват ассистента.

## Включение и выключение MCP

1. Щелкните **MCP** на панели инструментов. Открывается окно **MCP**.
2. Щелкните **Включить**.

Точка на кнопке **MCP** зеленая, когда MCP включен. Точка оранжевая, когда ассистент захватывает данные.

Чтобы выключить MCP, щелкните **Выключить** в окне **MCP**.

## Остановка захвата ассистента

1. Щелкните **MCP** на панели инструментов.
2. В окне **MCP** щелкните **Стоп**.

Когда ассистент не захватывает данные, строка **Сейчас:** показывает **Ожидание**.

## Подключение ассистента к приложению

Ассистент подключается к приложению через программу **Logic Analyze Agent** в пакете приложения. Укажите ассистенту путь к этой программе.

Для Claude Code введите эту команду в Терминале:

```sh
claude mcp add logic-analyze -- "/Applications/Logic Analyze.app/Contents/Library/LoginItems/Logic Analyze Agent.app/Contents/MacOS/logic-analyze-mcp"
```

Для Claude Desktop и Cursor добавьте этот текст в файл конфигурации MCP ассистента:

```json
{
  "mcpServers": {
    "logic-analyze": {
      "command": "/Applications/Logic Analyze.app/Contents/Library/LoginItems/Logic Analyze Agent.app/Contents/MacOS/logic-analyze-mcp"
    }
  }
}
```

Файл конфигурации Claude Desktop: `~/Library/Application Support/Claude/claude_desktop_config.json`. Файл конфигурации Cursor: `~/.cursor/mcp.json`. Если в файле уже есть объект `mcpServers`, добавьте в него только элемент `logic-analyze`.

> [!NOTE]
> Если вы включили MCP после запуска ассистента, снова подключите сервер `logic-analyze` в ассистенте. В Claude Code введите `/mcp` и снова подключите сервер. Закройте Claude Desktop и откройте его снова.
<!-- end edition -->
