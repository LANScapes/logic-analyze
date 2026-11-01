<!-- edition: appstore -->
# Conectar um assistente de IA (MCP)

## Sobre o MCP

O MCP (Model Context Protocol) conecta um assistente de IA do seu Mac ao Logic Analyze. Claude Code, Claude Desktop e Cursor são exemplos de assistentes de IA. Com o MCP, o assistente pode capturar, medir e decodificar dados por meio do aplicativo.

O MCP fica desativado até você ativá-lo. Estas condições são necessárias para que o assistente possa usar o aplicativo:

- O Logic Analyze está aberto.
- O MCP está ativado.
- Um dispositivo DSLogic está conectado ao Mac. Sem o dispositivo, o assistente não pode capturar dados.

O aplicativo mostra cada captura do assistente na janela principal. Você pode parar uma captura do assistente.

## Ativar ou desativar o MCP

1. Clique em **MCP** na barra de ferramentas. A janela **MCP** abre.
2. Clique em **Ativar**.

O ponto no botão **MCP** fica verde quando o MCP está ativado. O ponto fica laranja quando o assistente captura dados.

Para desativar o MCP, clique em **Desativar** na janela **MCP**.

## Parar uma captura do assistente

1. Clique em **MCP** na barra de ferramentas.
2. Na janela **MCP**, clique em **Parar**.

Quando o assistente não captura dados, a linha **Agora:** mostra **Ocioso**.

## Conectar o assistente ao aplicativo

O assistente se conecta ao aplicativo por meio do programa **Logic Analyze Agent**, que fica no pacote do aplicativo. Dê ao assistente o caminho deste programa.

Para o Claude Code, digite este comando no Terminal:

```sh
claude mcp add logic-analyze -- "/Applications/Logic Analyze.app/Contents/Library/LoginItems/Logic Analyze Agent.app/Contents/MacOS/logic-analyze-mcp"
```

Para o Claude Desktop e o Cursor, adicione este texto ao arquivo de configuração MCP do assistente:

```json
{
  "mcpServers": {
    "logic-analyze": {
      "command": "/Applications/Logic Analyze.app/Contents/Library/LoginItems/Logic Analyze Agent.app/Contents/MacOS/logic-analyze-mcp"
    }
  }
}
```

O arquivo de configuração do Claude Desktop é `~/Library/Application Support/Claude/claude_desktop_config.json`. O arquivo de configuração do Cursor é `~/.cursor/mcp.json`. Se o arquivo já tiver um objeto `mcpServers`, adicione somente o item `logic-analyze`.

> [!NOTE]
> Se você ativar o MCP depois de iniciar o assistente, conecte de novo o servidor `logic-analyze` no assistente. No Claude Code, digite `/mcp` e conecte o servidor de novo. Feche o Claude Desktop e abra-o de novo.
<!-- end edition -->
