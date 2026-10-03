# Chế độ máy hiện sóng và chế độ thu thập dữ liệu

Logic Analyze cũng có thể vận hành máy hiện sóng DSCope của DreamSourceLab. DSCope có hai chế độ thiết bị:

- **Máy hiện sóng**: cho tín hiệu có chu kỳ không đổi và cho một điều kiện tín hiệu.
- **Thu thập dữ liệu**: cho tín hiệu chậm trong thời gian dài, ví dụ điện áp nguồn hoặc đầu ra cảm biến.

Các chế độ này không có trên thiết bị DSLogic. Chương này chỉ trình bày các quy trình chính.

## Nối DSCope

> [!WARNING]
> Không nối đầu đo với điện áp lưới. Không nối đầu đo với mạch có kết nối điện với điện áp lưới. Điện áp này có thể gây thương tích hoặc tử vong.

> [!CAUTION]
> Đất của đầu đo, đất của DSCope và đất của máy tính nối với nhau. Chỉ nối đất đầu đo với điểm có cùng điện áp với đất của máy tính. Chênh lệch điện áp có thể làm hỏng thiết bị.

1. Nối DSCope với máy tính bằng cáp USB.
2. Khởi động Logic Analyze. Kiểm tra danh sách thiết bị hiển thị DSCope.
3. Nối đầu đo với đầu vào của DSCope.
4. Đặt công tắc suy giảm trên mỗi đầu đo.
5. Nối kẹp đất của mỗi đầu đo với đất của mạch.
6. Nối đầu nhọn của đầu đo với tín hiệu.

## Tùy chọn thiết bị

Nhấp **Tùy chọn** › **Tùy chọn thiết bị...** hoặc nhấn `O`.

- **Chế độ hoạt động**: **Bình thường** để đo. **Kiểm tra nội bộ** chỉ dùng để kiểm tra thiết bị.
- **Giới hạn băng thông**: **Toàn băng thông** hoặc **20MHz**. Giới hạn 20 MHz làm giảm nhiễu tần số cao.

## Hiệu chuẩn DSCope

Độ lợi và độ lệch của đầu vào thay đổi theo nhiệt độ và độ ẩm. Hiệu chuẩn DSCope để giữ phép đo chính xác.

### Hiệu chuẩn tự động

> [!CAUTION]
> Tháo tất cả đầu đo khỏi đầu vào trước khi hiệu chuẩn. Tín hiệu trên đầu vào trong khi hiệu chuẩn cho giá trị hiệu chuẩn sai.

1. Mở cửa sổ **Tùy chọn thiết bị**.
2. Nhấp **Tự động hiệu chuẩn**.
3. Tháo tất cả đầu đo. Nhấp **OK**. Việc hiệu chuẩn kéo dài vài phút.
4. Khi hiệu chuẩn hoàn tất, nhấp **Lưu** để giữ kết quả.

Để dừng hiệu chuẩn, nhấp **Hủy bỏ**. Khi đó thiết bị dùng giá trị hiệu chuẩn trước.

### Hiệu chuẩn thủ công

1. Mở cửa sổ **Tùy chọn thiết bị**.
2. Nhấp **Hiệu chuẩn thủ công**.
3. Nhấp **Chạy** trên thanh công cụ.
4. Để chỉnh độ lệch, nối đầu đo với đất. Để chỉnh độ lợi, nối đầu đo với tín hiệu có điện áp đã biết.
5. Đặt thang đo dọc bạn muốn hiệu chuẩn.
6. Di chuyển thanh trượt **Độ lệch** hoặc **Độ lợi** của kênh đến khi dạng sóng đúng.
7. Làm lại các bước 5 và 6 cho mỗi thang đo dọc.
8. Nhấp **Lưu**.

Để bỏ các thay đổi, nhấp **Hủy bỏ**. Để chỉ dùng các thay đổi đến khi bạn rút thiết bị, nhấp **Thoát**. Để trở về giá trị ban đầu, nhấp **Đặt lại**. Sau khi đặt lại, hiệu chuẩn tự động lại.

## Cài đặt kênh

Mỗi kênh có các điều khiển sau ở bên trái vùng dạng sóng:

- **Bật**: bật hoặc tắt kênh.
- **Thang đo dọc**: điện áp cho mỗi ô. Cửa sổ có 10 ô. Để đổi thang đo, xoay bánh xe chuột trên núm, hoặc nhấp phần trên hoặc phần dưới của núm. Bạn cũng có thể nhấn `0` hoặc `1` để chọn núm của một kênh, rồi nhấn `↑` hoặc `↓`.
- **Ghép nối**: **DC** hoặc **AC**.
- **Suy giảm đầu đo**: đặt **x1** hoặc **x10** cho khớp với công tắc trên đầu đo.
- **Tự động**: đặt thang đo dọc, thang đo ngang và mức kích hoạt cho tín hiệu đang có trên đầu vào.

Để di chuyển dạng sóng của một kênh lên hoặc xuống, kéo nhãn kênh.

## Thang đo ngang

Chọn thời gian cho mỗi ô trong danh sách trên thanh công cụ. Bạn cũng có thể xoay bánh xe chuột trong vùng dạng sóng.

## Chạy và dừng

- Nhấp **Chạy** hoặc nhấn `S` để bắt đầu thu liên tục. Nhấp **Dừng** để dừng.
- Nhấp **Một lần** hoặc nhấn `I` để thu một dạng sóng rồi dừng.

## Kích hoạt

Nhấp **Kích hoạt** hoặc nhấn `T` để mở khung neo kích hoạt. Khung neo có các cài đặt sau:

- **Vị trí kích hoạt**: vị trí của điểm kích hoạt trong lần thu, tính theo phần trăm.
- **Thời gian chặn**: thời gian sau một lần kích hoạt mà thiết bị bỏ qua kích hoạt mới. Dùng nó để có dạng sóng ổn định từ các nhóm xung.
- **Độ nhạy kích hoạt**: thay đổi điện áp cần thiết cho một lần kích hoạt. Giá trị lớn hơn bỏ qua nhiều nhiễu hơn.
- **Nguồn kích hoạt**: **Tự động**, **Kênh 0**, **Kênh 1**, **Kênh 0 && 1** hoặc **Kênh 0 | 1**.
- **Kiểu kích hoạt**: **Sườn lên** hoặc **Sườn xuống**.

Để đặt mức kích hoạt, nhấp nhãn mức kích hoạt của kênh. Di chuyển chuột. Nhấp lại để đặt mức.

## Phép đo

### Phép đo tự động

Phía dưới vùng dạng sóng có 10 ô cho phép đo tự động.

1. Nhấp một ô đo.
2. Chọn kênh.
3. Chọn phép đo. Để xóa ô, nhấp **Đặt lại**.

Ứng dụng giữ các cài đặt này cho lần khởi động sau.

### Con trỏ

- Để thêm con trỏ thời gian, nhấp vào thước thời gian. Bạn cũng có thể nhấp nút chuột phải trong vùng dạng sóng và chọn **Thêm con trỏ Y**.
- Để thêm con trỏ điện áp, nhấp nút chuột phải trong vùng dạng sóng và chọn **Thêm con trỏ X**. Mỗi con trỏ điện áp có hai đường ngang. Nhãn giữa hai đường hiển thị chênh lệch điện áp.
- Để đo thời gian giữa hai con trỏ, dùng nhóm **Khoảng cách con trỏ** trong khung neo phép đo.

### Đo bằng con trỏ chuột

Sau khi bạn dừng lần thu, đặt con trỏ chuột lên dạng sóng. Ứng dụng hiển thị điện áp của mẫu tại con trỏ chuột.

Để đo thời gian, nhấp đúp vào một vùng trống của dạng sóng. Nhấp vào điểm thứ hai. Nhấp vào điểm thứ ba để xem tần số, chu kỳ và chu kỳ nhiệm vụ. Nhấp nút chuột phải để hủy.

## Phổ (FFT)

1. Nhấp **Hàm** › **FFT**.
2. Chọn **Bật FFT**.
3. Đặt **Độ dài FFT**, **Khoảng lấy mẫu**, **Nguồn FFT** và **Cửa sổ FFT**.
4. Đặt **Chế độ trục Y** và **Dải DBV**.
5. Nhấp **OK**.

Phổ hiển thị bên dưới dạng sóng. Xoay bánh xe chuột trong phổ để phóng thang tần số. Kéo phổ để di chuyển nó. Đặt con trỏ chuột lên phổ để xem tần số và biên độ.

## Kênh toán

1. Nhấp **Hàm** › **Toán**.
2. Chọn **Bật**.
3. Chọn **Loại phép toán**: **Cộng**, **Trừ**, **Nhân** hoặc **Chia**.
4. Chọn **Nguồn thứ 1** và **Nguồn thứ 2**.
5. Nhấp **OK**.

## Hình Lissajous

1. Nhấp **Tùy chọn** › **Hiển thị** › **Lissajous**.
2. Chọn **Bật**.
3. Chọn kênh cho **Trục X** và **Trục Y**.
4. Nhấp **OK**.

## Chế độ thu thập dữ liệu

1. Trong danh sách chế độ thiết bị trên thanh công cụ, chọn **Thu thập dữ liệu**.
2. Mở cửa sổ **Tùy chọn thiết bị**.
3. Với mỗi kênh, đặt **Bật**, **Ghép nối** và **Volt/ô**.
4. Để hiển thị một đơn vị khác, đặt **Đơn vị ánh xạ**, **Ánh xạ tối thiểu** và **Ánh xạ tối đa**. Ví dụ, hiển thị đầu ra của cảm biến nhiệt độ bằng °C.
5. Nhấp **OK**.
6. Chọn tốc độ lấy mẫu và thời lượng lấy mẫu trên thanh công cụ.
7. Nhấp **Chạy** hoặc nhấn `S`.

Bạn không thể đổi cài đặt kênh trong khi thu. Ở tốc độ lấy mẫu cao nhất 10 MHz, thời lượng lấy mẫu tối đa khoảng 10 giây. Ở 1 kHz, lần thu có thể kéo dài một ngày.

Chế độ thu thập dữ liệu dùng hiệu chuẩn của chế độ máy hiện sóng. Nếu một kênh có độ lệch, hiệu chuẩn thiết bị ở chế độ máy hiện sóng.
