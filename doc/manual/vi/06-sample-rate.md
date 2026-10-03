# Tốc độ lấy mẫu và thời lượng lấy mẫu

Thanh công cụ có hai danh sách cho độ dài lần thu. Danh sách trên là thời lượng lấy mẫu. Danh sách dưới là tốc độ lấy mẫu.

- **Thời lượng lấy mẫu** là độ dài thời gian của lần thu.
- **Tốc độ lấy mẫu** là số mẫu trong mỗi giây, cho mỗi kênh.

Các giá trị có sẵn thay đổi theo thiết bị, kết nối USB, chế độ hoạt động và chế độ kênh.

## Thời lượng lấy mẫu tối đa

**Chế độ bộ đệm.** Bộ nhớ của thiết bị giới hạn thời lượng lấy mẫu. Dùng công thức sau:

```text
maximum duration = memory size / (sample rate × number of enabled channels)
```

DSLogic Plus có bộ nhớ 256 Mbit. Đây là hai ví dụ:

- Ở 100 MHz với 16 kênh, thời lượng lấy mẫu tối đa khoảng 167.77 ms.
- Ở 400 MHz với 1 kênh, thời lượng lấy mẫu tối đa khoảng 671.09 ms.

**Chế độ luồng.** Bộ nhớ của máy tính giới hạn thời lượng lấy mẫu. Ứng dụng có thể giữ 16 G mẫu cho mỗi kênh. Đây là hai ví dụ:

- Ở 1 MHz, thời lượng lấy mẫu tối đa khoảng 4.77 giờ.
- Ở 100 MHz, thời lượng lấy mẫu tối đa khoảng 2.86 phút.

## Chọn tốc độ lấy mẫu

Đặt tốc độ lấy mẫu bằng 4 đến 10 lần tần số cao nhất trong tín hiệu.

Ở 4 lần tần số tín hiệu, ứng dụng ghi được mỗi sườn. Nhưng thời điểm của mỗi sườn có sai số đến 25% chu kỳ tín hiệu. Ở 10 lần tần số tín hiệu, sai số giảm còn 10%.

Sai số thời gian của một sườn bằng một chu kỳ lấy mẫu hoặc nhỏ hơn. Ví dụ, ở 100 MHz chu kỳ lấy mẫu là 10 ns. Vì vậy, sai số của mỗi sườn là ±10 ns hoặc nhỏ hơn.

![Ảnh hưởng của tốc độ lấy mẫu đến dạng sóng đã ghi](../figures/sample-rate-effect.png)

Đây là các giá trị thông dụng:

| Tín hiệu | Tốc độ lấy mẫu thông dụng |
| --- | --- |
| UART ở 115200 baud | 2 MHz |
| I2C ở 400 kHz | 4 MHz đến 10 MHz |
| SPI ở 40 MHz | 400 MHz |

## Không dùng tốc độ lấy mẫu quá cao

Tốc độ lấy mẫu cao hơn cho dạng sóng chính xác hơn. Nhưng tốc độ lấy mẫu cao cũng có các vấn đề sau:

1. Ứng dụng ghi nhiều dữ liệu hơn trong mỗi giây. Vì vậy, thời lượng lấy mẫu tối đa giảm. Ứng dụng cũng cần nhiều thời gian hơn để hiển thị và giải mã dữ liệu.
2. Tín hiệu chậm có thể có sườn chậm. Ở tốc độ lấy mẫu cao, ứng dụng có thể ghi các xung nhỏ tại ngưỡng trong mỗi sườn chậm. Các xung này có thể gây lỗi trong bộ giải mã.

Nếu bạn thấy xung ngắn không mong muốn trên tín hiệu chậm, giảm tốc độ lấy mẫu. Bạn cũng có thể đặt **Đối tượng lọc** thành **1 chu kỳ lấy mẫu**. Xem [Tùy chọn thiết bị](05-device-options.md).

## Đặt tốc độ lấy mẫu và thời lượng

1. Đặt chế độ hoạt động và chế độ kênh. Xem [Tùy chọn thiết bị](05-device-options.md).
2. Trong danh sách dưới trên thanh công cụ, chọn tốc độ lấy mẫu.
3. Trong danh sách trên trên thanh công cụ, chọn thời lượng lấy mẫu.

> [!NOTE]
> Khi bạn đổi chế độ kênh, ứng dụng có thể đổi tốc độ lấy mẫu. Kiểm tra lại tốc độ lấy mẫu sau mỗi lần đổi tùy chọn thiết bị.
