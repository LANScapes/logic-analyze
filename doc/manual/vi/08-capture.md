# Thu dữ liệu

Trước khi bắt đầu lần thu, đặt các mục sau:

1. Tùy chọn thiết bị. Xem [Tùy chọn thiết bị](05-device-options.md).
2. Tốc độ lấy mẫu và thời lượng lấy mẫu. Xem [Tốc độ lấy mẫu và thời lượng lấy mẫu](06-sample-rate.md).
3. Kích hoạt, nếu cần. Xem [Kích hoạt](07-trigger.md).
4. Chế độ thu. Xem [Chế độ thu](#capture-modes).

## Bắt đầu lần thu

Có hai loại thu:

- **Chạy** bắt đầu lần thu chuẩn. Nếu bạn đặt kích hoạt, thiết bị chờ kích hoạt.
- **Tức thì** bắt đầu lần thu ngay. Thiết bị không dùng cài đặt kích hoạt.

Để bắt đầu lần thu chuẩn, nhấp **Chạy** hoặc nhấn `S`. Để bắt đầu lần thu tức thì, nhấp **Tức thì** hoặc nhấn `I`. Trong lần thu, nút đổi thành **Dừng**. Nhấp **Dừng** để dừng lần thu.

### Trình tự lần thu chuẩn ở chế độ bộ đệm

1. Bạn nhấp **Chạy**.
2. Ứng dụng gửi cài đặt đến thiết bị.
3. Nếu không có kích hoạt, thiết bị bắt đầu ghi ngay. Nếu có kích hoạt, thiết bị chờ kích hoạt.
4. Thiết bị ghi đến hết thời lượng lấy mẫu hoặc đến khi bộ nhớ đầy.
5. Thiết bị gửi dữ liệu đến máy tính.
6. Ứng dụng hiển thị dạng sóng trong vùng dạng sóng.

### Trình tự lần thu chuẩn ở chế độ luồng

1. Bạn nhấp **Chạy**.
2. Ứng dụng gửi cài đặt đến thiết bị.
3. Nếu có kích hoạt, thiết bị chờ kích hoạt. Ở chế độ vòng lặp, thiết bị không dùng kích hoạt.
4. Thiết bị gửi dữ liệu đến máy tính trong khi thu.
5. Ứng dụng hiển thị dạng sóng trong khi thu.
6. Lần thu dừng khi hết thời lượng lấy mẫu. Ở chế độ vòng lặp, lần thu tiếp tục đến khi bạn nhấp **Dừng**.

## Dùng lần thu tức thì

Lần thu tức thì giống lần thu chuẩn, nhưng không dùng cài đặt kích hoạt. Dùng nó trong các trường hợp sau:

- Lần thu chuẩn chờ lâu vì điều kiện kích hoạt không xảy ra.
- Bạn muốn xem tín hiệu tại thời điểm này.
- Bạn muốn kiểm tra tín hiệu trước khi đổi kích hoạt.

Nếu không có tín hiệu, lần thu chuẩn chờ tại vị trí kích hoạt. Trạng thái hiển thị **Đang chờ kích hoạt!**. Lần thu tức thì ghi tín hiệu ngay.

## Chế độ thu {#capture-modes}

Để chọn chế độ thu, nhấp **Chế độ** trên thanh công cụ. Sau đó chọn một trong các mục sau:

| Chế độ thu | Chế độ bộ đệm | Chế độ luồng |
| --- | --- | --- |
| **Một lần** | Có | Có |
| **Lặp lại** | Có | Có |
| **Vòng lặp** | Không | Có |

![Menu chế độ thu](../figures/capture-mode-menu.png)
<!-- TODO: new screenshot -->

### Một lần

Thiết bị thu một lần. Sau đó lần thu dừng.

Ở chế độ bộ đệm, ứng dụng hiển thị dạng sóng sau lần thu. Ở chế độ luồng, ứng dụng hiển thị dạng sóng trong khi thu.

Dùng chế độ này để thu một điều kiện tín hiệu hoặc dạng sóng tại thời điểm này.

### Lặp lại

Thiết bị thu một lần. Sau đó thiết bị tự động bắt đầu lần thu tiếp theo. Việc này tiếp tục đến khi bạn nhấp **Dừng**.

Ở chế độ bộ đệm, ứng dụng hiện một cửa sổ để đặt khoảng thời gian giữa các lần thu. Bạn có thể đặt giá trị từ 0.1 s đến 10 s.

Dùng chế độ này để xem một điều kiện tín hiệu xảy ra nhiều lần. Ví dụ, dùng nó để xem tín hiệu sau mỗi lần đặt lại mạch hoặc sau mỗi lần nhấn nút. Dùng nó cùng với kích hoạt.

### Vòng lặp

Chế độ này chỉ có ở chế độ luồng. Lần thu tiếp tục đến khi bạn nhấp **Dừng**. Khi dữ liệu dài hơn thời lượng lấy mẫu, dữ liệu đầu tiên ra khỏi cửa sổ ở bên trái. Dữ liệu mới nhất vào từ bên phải. Ứng dụng bỏ dữ liệu đã ra khỏi cửa sổ.

Dùng chế độ này khi bạn không biết thời điểm của điều kiện tín hiệu. Quan sát dạng sóng trong khi thu. Khi bạn thấy điều kiện đó, nhấp **Dừng**.

> [!NOTE]
> Ở chế độ vòng lặp, thiết bị không dùng cài đặt kích hoạt.

## Trạng thái thu

Trong khi thu, vùng dạng sóng hiển thị trạng thái:

- **Đang chờ kích hoạt!**: Thiết bị chờ điều kiện kích hoạt.
- **Đã kích hoạt!**: Kích hoạt đã xảy ra.
- **% đã thu**: Phần trăm lần thu đã hoàn tất.

Sau lần thu, phía dưới vùng dạng sóng hiển thị **Thời điểm kích hoạt**.
