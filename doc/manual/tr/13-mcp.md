<!-- edition: appstore -->
# Bir yapay zekâ asistanı bağlama (MCP)

## MCP hakkında

MCP (Model Context Protocol), Mac'inizdeki bir yapay zekâ asistanını Logic Analyze'a bağlar. Claude Code, Claude Desktop ve Cursor, yapay zekâ asistanlarına örnektir. MCP ile asistan, uygulama üzerinden veri yakalayabilir, ölçebilir ve verinin kodunu çözebilir.

MCP, siz açana kadar kapalıdır. Asistanın uygulamayı kullanabilmesi için şu koşullar gereklidir:

- Logic Analyze açıktır.
- MCP açıktır.
- Bir DSLogic cihazı Mac'e bağlıdır. Cihaz olmadan asistan veri yakalayamaz.

Uygulama, asistanın her yakalamasını ana pencerede gösterir. Asistanın bir yakalamasını durdurabilirsiniz.

## MCP'yi açma veya kapatma

1. Araç çubuğunda **MCP** öğesine tıklayın. **MCP** penceresi açılır.
2. **Aç** düğmesine tıklayın.

MCP açıkken **MCP** düğmesindeki nokta yeşildir. Asistan veri yakalarken nokta turuncudur.

MCP'yi kapatmak için **MCP** penceresinde **Kapat** düğmesine tıklayın.

## Asistanın bir yakalamasını durdurma

1. Araç çubuğunda **MCP** öğesine tıklayın.
2. **MCP** penceresinde **Durdur** düğmesine tıklayın.

Asistan veri yakalamıyorsa **Şu an:** satırı **Boşta** gösterir.

## Asistanı uygulamaya bağlama

Asistan, uygulama paketindeki **Logic Analyze Agent** programı üzerinden uygulamaya bağlanır. Bu programın yolunu asistana verin.

Claude Code için Terminal'de şu komutu yazın:

```sh
claude mcp add logic-analyze -- "/Applications/Logic Analyze.app/Contents/Library/LoginItems/Logic Analyze Agent.app/Contents/MacOS/logic-analyze-mcp"
```

Claude Desktop ve Cursor için bu metni asistanın MCP yapılandırma dosyasına ekleyin:

```json
{
  "mcpServers": {
    "logic-analyze": {
      "command": "/Applications/Logic Analyze.app/Contents/Library/LoginItems/Logic Analyze Agent.app/Contents/MacOS/logic-analyze-mcp"
    }
  }
}
```

Claude Desktop'ın yapılandırma dosyası `~/Library/Application Support/Claude/claude_desktop_config.json` dosyasıdır. Cursor'ın yapılandırma dosyası `~/.cursor/mcp.json` dosyasıdır. Dosyada zaten bir `mcpServers` nesnesi varsa yalnızca `logic-analyze` öğesini ekleyin.

> [!NOTE]
> MCP'yi asistan başladıktan sonra açarsanız `logic-analyze` sunucusunu asistanda yeniden bağlayın. Claude Code'da `/mcp` yazın ve sunucuyu yeniden bağlayın. Claude Desktop'tan çıkın ve onu yeniden açın.
<!-- end edition -->
