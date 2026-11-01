<!-- edition: appstore -->
# Kết nối trợ lý AI (MCP)

## Về MCP

MCP (Model Context Protocol) kết nối một trợ lý AI trên máy Mac của bạn với Logic Analyze. Claude Code, Claude Desktop và Cursor là ví dụ về trợ lý AI. Với MCP, trợ lý có thể thu, đo và giải mã dữ liệu qua ứng dụng.

MCP tắt cho đến khi bạn bật nó. Trợ lý cần các điều kiện sau để dùng ứng dụng:

- Logic Analyze đang mở.
- MCP đang bật.
- Một thiết bị DSLogic được kết nối với máy Mac. Không có thiết bị, trợ lý không thể thu dữ liệu.

Ứng dụng hiển thị mỗi lần thu của trợ lý trong cửa sổ chính. Bạn có thể dừng một lần thu của trợ lý.

## Bật hoặc tắt MCP

1. Nhấp **MCP** trên thanh công cụ. Cửa sổ **MCP** mở ra.
2. Nhấp **Bật**.

Chấm trên nút **MCP** có màu xanh lá khi MCP bật. Chấm có màu cam khi trợ lý thu dữ liệu.

Để tắt MCP, nhấp **Tắt** trong cửa sổ **MCP**.

## Dừng một lần thu của trợ lý

1. Nhấp **MCP** trên thanh công cụ.
2. Trong cửa sổ **MCP**, nhấp **Dừng**.

Khi trợ lý không thu dữ liệu, dòng **Hiện tại:** hiển thị **Rảnh**.

## Kết nối trợ lý với ứng dụng

Trợ lý kết nối với ứng dụng qua chương trình **Logic Analyze Agent** trong gói ứng dụng. Cung cấp đường dẫn của chương trình này cho trợ lý.

Với Claude Code, nhập lệnh sau trong Terminal:

```sh
claude mcp add logic-analyze -- "/Applications/Logic Analyze.app/Contents/Library/LoginItems/Logic Analyze Agent.app/Contents/MacOS/logic-analyze-mcp"
```

Với Claude Desktop và Cursor, thêm văn bản sau vào tệp cấu hình MCP của trợ lý:

```json
{
  "mcpServers": {
    "logic-analyze": {
      "command": "/Applications/Logic Analyze.app/Contents/Library/LoginItems/Logic Analyze Agent.app/Contents/MacOS/logic-analyze-mcp"
    }
  }
}
```

Tệp cấu hình của Claude Desktop là `~/Library/Application Support/Claude/claude_desktop_config.json`. Tệp cấu hình của Cursor là `~/.cursor/mcp.json`. Nếu tệp đã có đối tượng `mcpServers`, chỉ thêm mục `logic-analyze` vào đó.

> [!NOTE]
> Nếu bạn bật MCP sau khi trợ lý khởi động, hãy kết nối lại máy chủ `logic-analyze` trong trợ lý. Trong Claude Code, nhập `/mcp` và kết nối lại máy chủ. Thoát Claude Desktop rồi mở lại.
<!-- end edition -->
