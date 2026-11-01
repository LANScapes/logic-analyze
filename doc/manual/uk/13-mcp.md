<!-- edition: appstore -->
# Підключення ШІ-асистента (MCP)

## Про MCP

MCP (Model Context Protocol) підключає ШІ-асистента на вашому Mac до Logic Analyze. Claude Code, Claude Desktop і Cursor — приклади ШІ-асистентів. З MCP асистент може захоплювати, вимірювати й декодувати дані через застосунок.

MCP вимкнено, доки ви його не ввімкнете. Щоб асистент міг використовувати застосунок, потрібні ці умови:

- Logic Analyze відкрито.
- MCP увімкнено.
- Пристрій DSLogic підключено до Mac. Без пристрою асистент не може захоплювати дані.

Застосунок показує кожне захоплення асистента в головному вікні. Ви можете зупинити захоплення асистента.

## Увімкнення та вимкнення MCP

1. Клацніть **MCP** на панелі інструментів. Відкривається вікно **MCP**.
2. Клацніть **Увімкнути**.

Крапка на кнопці **MCP** зелена, коли MCP увімкнено. Крапка помаранчева, коли асистент захоплює дані.

Щоб вимкнути MCP, клацніть **Вимкнути** у вікні **MCP**.

## Зупинення захоплення асистента

1. Клацніть **MCP** на панелі інструментів.
2. У вікні **MCP** клацніть **Стоп**.

Коли асистент не захоплює дані, рядок **Зараз:** показує **Очікування**.

## Підключення асистента до застосунку

Асистент підключається до застосунку через програму **Logic Analyze Agent** у пакеті застосунку. Укажіть асистенту шлях до цієї програми.

Для Claude Code введіть цю команду в Терміналі:

```sh
claude mcp add logic-analyze -- "/Applications/Logic Analyze.app/Contents/Library/LoginItems/Logic Analyze Agent.app/Contents/MacOS/logic-analyze-mcp"
```

Для Claude Desktop і Cursor додайте цей текст у файл конфігурації MCP асистента:

```json
{
  "mcpServers": {
    "logic-analyze": {
      "command": "/Applications/Logic Analyze.app/Contents/Library/LoginItems/Logic Analyze Agent.app/Contents/MacOS/logic-analyze-mcp"
    }
  }
}
```

Файл конфігурації Claude Desktop: `~/Library/Application Support/Claude/claude_desktop_config.json`. Файл конфігурації Cursor: `~/.cursor/mcp.json`. Якщо у файлі вже є об'єкт `mcpServers`, додайте до нього тільки елемент `logic-analyze`.

> [!NOTE]
> Якщо ви ввімкнули MCP після запуску асистента, знову підключіть сервер `logic-analyze` в асистенті. У Claude Code введіть `/mcp` і знову підключіть сервер. Закрийте Claude Desktop і відкрийте його знову.
<!-- end edition -->
