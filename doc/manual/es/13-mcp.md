<!-- edition: appstore -->
# Conectar un asistente de IA (MCP)

## Acerca de MCP

MCP (Model Context Protocol) conecta un asistente de IA de su Mac con Logic Analyze. Claude Code, Claude Desktop y Cursor son ejemplos de asistentes de IA. Con MCP, el asistente puede capturar, medir y decodificar datos a través de la aplicación.

MCP está desactivado hasta que usted lo activa. Estas condiciones son necesarias para que el asistente pueda usar la aplicación:

- Logic Analyze está abierta.
- MCP está activado.
- Un dispositivo DSLogic está conectado al Mac. Sin el dispositivo, el asistente no puede capturar datos.

La aplicación muestra cada captura del asistente en la ventana principal. Puede detener una captura del asistente.

## Activar o desactivar MCP

1. Haga clic en **MCP** en la barra de herramientas. Se abre la ventana **MCP**.
2. Haga clic en **Activar**.

El punto del botón **MCP** es verde cuando MCP está activado. El punto es naranja cuando el asistente captura datos.

Para desactivar MCP, haga clic en **Desactivar** en la ventana **MCP**.

## Detener una captura del asistente

1. Haga clic en **MCP** en la barra de herramientas.
2. En la ventana **MCP**, haga clic en **Detener**.

Cuando el asistente no captura datos, la línea **Ahora:** muestra **Inactivo**.

## Conectar el asistente con la aplicación

El asistente se conecta con la aplicación a través del programa **Logic Analyze Agent**, que está en el paquete de la aplicación. Dé al asistente la ruta de este programa.

Para Claude Code, escriba este comando en Terminal:

```sh
claude mcp add logic-analyze -- "/Applications/Logic Analyze.app/Contents/Library/LoginItems/Logic Analyze Agent.app/Contents/MacOS/logic-analyze-mcp"
```

Para Claude Desktop y Cursor, añada este texto al archivo de configuración MCP del asistente:

```json
{
  "mcpServers": {
    "logic-analyze": {
      "command": "/Applications/Logic Analyze.app/Contents/Library/LoginItems/Logic Analyze Agent.app/Contents/MacOS/logic-analyze-mcp"
    }
  }
}
```

El archivo de configuración de Claude Desktop es `~/Library/Application Support/Claude/claude_desktop_config.json`. El archivo de configuración de Cursor es `~/.cursor/mcp.json`. Si el archivo ya tiene un objeto `mcpServers`, añada solo el elemento `logic-analyze`.

> [!NOTE]
> Si activa MCP después de iniciar el asistente, conecte otra vez el servidor `logic-analyze` en el asistente. En Claude Code, escriba `/mcp` y conecte el servidor otra vez. Salga de Claude Desktop y ábralo otra vez.
<!-- end edition -->
