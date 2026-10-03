# Nối DSLogic Plus

## Nối cáp USB

> [!NOTE]
> Dùng cáp USB đi kèm thiết bị hoặc cáp USB ngắn, chất lượng tốt. Nối cáp trực tiếp với cổng trên máy tính. Hub USB hoặc cáp dài có thể gây lỗi khi thu.

1. Nối cáp USB với DSLogic Plus.
2. Nối đầu kia của cáp USB với cổng USB trên máy tính.
3. Kiểm tra đèn báo trên DSLogic Plus sáng. Trước khi ứng dụng khởi động, đèn báo màu đỏ.
4. Khởi động Logic Analyze.
5. Kiểm tra đèn báo đổi sang màu xanh lục.
6. Kiểm tra danh sách thiết bị trên thanh công cụ hiển thị **DSLogic Plus**.

![Kết nối USB](../figures/usb-connection.png)

Nếu danh sách thiết bị không hiển thị thiết bị, làm các bước sau:

1. Rút cáp USB khỏi máy tính.
2. Chờ 5 giây.
3. Nối cáp USB với cổng USB khác.
4. Nếu sau bước 3 danh sách thiết bị vẫn không hiển thị thiết bị, thoát ứng dụng rồi khởi động lại.

> [!NOTE]
> Mỗi lần chỉ một chương trình dùng được thiết bị. Nếu `dslcap` hoặc chương trình khác đang dùng thiết bị, ứng dụng không tìm thấy thiết bị.

## Nối cáp đầu đo

Cáp đầu đo có 16 dây kênh. Mỗi dây kênh có lớp chống nhiễu, đầu tín hiệu và đầu nối đất. Màu dây phân biệt các kênh từ 0 đến 15. Thêm một dây có các tín hiệu sau:

- **CK**: Đầu vào xung nhịp ngoài. Chỉ dùng với cài đặt **Dùng xung nhịp ngoài**.
- **TI**: Đầu vào tín hiệu kích hoạt ngoài.
- **TO**: Đầu ra tín hiệu kích hoạt. Thiết bị gửi một xung trên TO khi kích hoạt xảy ra.

Thông thường, bạn không nối các dây CK, TI và TO.

![Cáp đầu đo và các kênh](../figures/probe-cable-channels.png)

1. Nối cáp đầu đo với đầu nối vào của DSLogic Plus.
2. Đẩy đầu nối vào hết thiết bị.

## Nối các kênh với mạch

> [!WARNING]
> Không nối đầu đo với điện áp lưới. Không nối đầu đo với mạch có kết nối điện với điện áp lưới. Điện áp này có thể gây thương tích hoặc tử vong.

> [!CAUTION]
> Trước khi nối dây đất, kiểm tra đất của mạch và đất của máy tính có cùng điện áp. Chênh lệch điện áp có thể tạo dòng điện lớn và làm hỏng thiết bị.

1. Ngắt nguồn điện của mạch bạn đo.
2. Nối ít nhất một dây đất với đất của mạch.
3. Nối mỗi dây kênh bạn dùng với một tín hiệu trong mạch.
4. Kiểm tra không có đầu đo nào chạm vào điểm tiếp xúc khác.
5. Cấp nguồn điện cho mạch.

![Nối đất: một đất chung (trái) hoặc một đất cho mỗi kênh (phải)](../figures/probe-grounding.png)

Với tín hiệu có tần số dưới 5 MHz, một dây đất cho tất cả các kênh là đủ. Với tín hiệu có tần số cao hơn, nối đầu nối đất của mỗi dây kênh với đất gần tín hiệu của nó. Đường nối đất ngắn cho sườn tín hiệu sạch.

## Ngắt kết nối DSLogic Plus

> [!CAUTION]
> Không rút cáp USB trong khi thu. Nếu bạn rút cáp, dữ liệu của lần thu có thể bị lỗi.

1. Dừng lần thu. Nhấp **Dừng** nếu nút này hiện trên thanh công cụ.
2. Ngắt nguồn điện của mạch.
3. Tháo đầu đo khỏi mạch.
4. Rút cáp USB.
