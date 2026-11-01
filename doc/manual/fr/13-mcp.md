<!-- edition: appstore -->
# Connecter un assistant IA (MCP)

## À propos de MCP

MCP (Model Context Protocol) connecte un assistant IA de votre Mac à Logic Analyze. Claude Code, Claude Desktop et Cursor sont des exemples d'assistants IA. Avec MCP, l'assistant peut capturer, mesurer et décoder des données par l'application.

MCP est désactivé jusqu'à ce que vous l'activiez. Ces conditions sont nécessaires pour que l'assistant puisse utiliser l'application :

- Logic Analyze est ouvert.
- MCP est activé.
- Un appareil DSLogic est connecté au Mac. Sans l'appareil, l'assistant ne peut pas capturer de données.

L'application affiche chaque capture de l'assistant dans la fenêtre principale. Vous pouvez arrêter une capture de l'assistant.

## Activer ou désactiver MCP

1. Cliquez sur **MCP** dans la barre d'outils. La fenêtre **MCP** s'ouvre.
2. Cliquez sur **Activer**.

Le point du bouton **MCP** est vert quand MCP est activé. Le point est orange quand l'assistant capture des données.

Pour désactiver MCP, cliquez sur **Désactiver** dans la fenêtre **MCP**.

## Arrêter une capture de l'assistant

1. Cliquez sur **MCP** dans la barre d'outils.
2. Dans la fenêtre **MCP**, cliquez sur **Arrêter**.

Quand l'assistant ne capture pas de données, la ligne **En cours :** affiche **Inactif**.

## Connecter l'assistant à l'application

L'assistant se connecte à l'application par le programme **Logic Analyze Agent**, qui se trouve dans le paquet de l'application. Donnez le chemin de ce programme à l'assistant.

Pour Claude Code, saisissez cette commande dans Terminal :

```sh
claude mcp add logic-analyze -- "/Applications/Logic Analyze.app/Contents/Library/LoginItems/Logic Analyze Agent.app/Contents/MacOS/logic-analyze-mcp"
```

Pour Claude Desktop et Cursor, ajoutez ce texte au fichier de configuration MCP de l'assistant :

```json
{
  "mcpServers": {
    "logic-analyze": {
      "command": "/Applications/Logic Analyze.app/Contents/Library/LoginItems/Logic Analyze Agent.app/Contents/MacOS/logic-analyze-mcp"
    }
  }
}
```

Le fichier de configuration de Claude Desktop est `~/Library/Application Support/Claude/claude_desktop_config.json`. Le fichier de configuration de Cursor est `~/.cursor/mcp.json`. Si le fichier contient déjà un objet `mcpServers`, ajoutez seulement l'élément `logic-analyze`.

> [!NOTE]
> Si vous activez MCP après le démarrage de l'assistant, connectez de nouveau le serveur `logic-analyze` dans l'assistant. Dans Claude Code, saisissez `/mcp` et connectez de nouveau le serveur. Quittez Claude Desktop et ouvrez-le de nouveau.
<!-- end edition -->
