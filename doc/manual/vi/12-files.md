# Tệp và phiên

Nhấp **Tệp** trên thanh công cụ để mở menu tệp. Menu có các mục sau:

- **Cấu hình**: menu để tải và lưu phiên.
- **Mở...**: mở tệp dữ liệu.
- **Lưu...**: lưu dữ liệu của lần thu.
- **Xuất...**: xuất dữ liệu sang định dạng khác.
- **Chụp ảnh...**: lưu ảnh của cửa sổ.

## Phiên

Tệp phiên chứa cài đặt, nhưng không chứa dữ liệu của lần thu. Phiên gồm tùy chọn thiết bị, các kênh đã bật, tên và màu kênh, và cài đặt kích hoạt. Tệp phiên có phần mở rộng `.dsc`.

### Lưu phiên

1. Nhấp **Tệp** › **Cấu hình** › **Lưu phiên**.
2. Chọn thư mục và nhập tên tệp.
3. Nhấp **Lưu**.

### Tải phiên

1. Nhấp **Tệp** › **Cấu hình** › **Tải phiên**.
2. Chọn tệp phiên.
3. Nhấp **Mở**.

### Trở về cài đặt ban đầu

Nhấp **Tệp** › **Cấu hình** › **Tải phiên mặc định**. Ứng dụng đặt tất cả cài đặt của thiết bị về giá trị ban đầu.

Ứng dụng tự động lưu cài đặt khi bạn thoát. Khi bạn khởi động lại ứng dụng, ứng dụng tải cài đặt của phiên cuối cùng.

## Lưu dữ liệu

1. Nhấp **Tệp** › **Lưu...**.
2. Chọn thư mục và nhập tên tệp.
3. Nhấp **Lưu**.

Ứng dụng lưu dữ liệu và cài đặt vào tệp có phần mở rộng `.dsl`. Bạn có thể mở lại tệp này trong Logic Analyze.

> [!CAUTION]
> Ứng dụng không tự động lưu dữ liệu. Lưu dữ liệu trước khi bắt đầu lần thu mới hoặc thoát ứng dụng. Lần thu mới thay thế dữ liệu của lần thu trước.

## Mở tệp dữ liệu

1. Nhấp **Tệp** › **Mở...**.
2. Chọn tệp có phần mở rộng `.dsl`.
3. Nhấp **Mở**.

Ứng dụng hiển thị dữ liệu trong vùng dạng sóng. Nhãn loại thiết bị hiển thị **Tệp**.

## Xuất dữ liệu

Xuất tạo một tệp mà các chương trình khác có thể đọc.

1. Nhấp **Tệp** › **Xuất...**. Cửa sổ **Xuất** mở ra.
2. Nhấp **đường dẫn**.
3. Chọn thư mục, nhập tên tệp và chọn định dạng.
4. Nhấp **Lưu**.
5. Nếu định dạng là CSV, chọn **Dữ liệu gốc** hoặc **Dữ liệu nén**. Dữ liệu nén chỉ có một hàng cho mỗi lần giá trị thay đổi.
6. Nhấp **OK**.

Ở chế độ máy phân tích logic, có các định dạng sau:

| Định dạng | Phần mở rộng | Công dụng |
| --- | --- | --- |
| CSV | `.csv` | Chương trình bảng tính và script. |
| VCD | `.vcd` | Chương trình xem dạng sóng, ví dụ GTKWave. |
| Gnuplot | `.gnuplot` | Chương trình Gnuplot. |
| srzip | `.srzip` | Chương trình sigrok, ví dụ PulseView. |

Ở chế độ máy hiện sóng và chế độ thu thập dữ liệu, chỉ có CSV.

![Cửa sổ xuất CSV](../figures/export-csv.png)
<!-- TODO: new screenshot -->

## Lưu ảnh cửa sổ

1. Nhấp **Tệp** › **Chụp ảnh...**.
2. Chọn thư mục và nhập tên tệp.
3. Chọn PNG hoặc JPEG.
4. Nhấp **Lưu**.
