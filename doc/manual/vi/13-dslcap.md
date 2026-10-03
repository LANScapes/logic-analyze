# Công cụ dslcap

Công cụ `dslcap` thu dữ liệu từ thiết bị DSLogic mà không cần cửa sổ chính. Dùng nó trong script và kiểm thử tự động. Công cụ ghi các mẫu vào một tệp nhị phân. Công cụ ghi một đối tượng JSON chứa kết quả ra đầu ra chuẩn.

Công cụ nằm trong gói ứng dụng:

```sh
"/Applications/Logic Analyze.app/Contents/MacOS/dslcap"
```

> [!NOTE]
> Mỗi lần chỉ một chương trình dùng được thiết bị. Thoát Logic Analyze trước khi dùng `dslcap`.

## Liệt kê thiết bị

Để liệt kê các thiết bị mà thư viện tìm thấy, nhập lệnh sau:

```sh
dslcap --list
```

Để liệt kê mã định danh USB của mỗi thiết bị DSLogic đã nối, nhập lệnh sau:

```sh
dslcap --list-ids
```

Lệnh `--list-ids` chỉ đọc thông tin mà macOS lưu về các thiết bị USB. Lệnh này không gửi dữ liệu đến thiết bị. Đầu ra cho biết mẫu thiết bị, vị trí USB và mã định danh registry của mỗi thiết bị.

## Thu dữ liệu

Lệnh sau thu 1000000 mẫu trên kênh 0 và 1 ở 10 MHz:

```sh
dslcap --channels 0,1 --samplerate 10000000 --samples 1000000 --out /tmp/capture
```

Công cụ ghi các mẫu vào `/tmp/capture.bin`. Nếu đã có tệp với tên này, công cụ dừng và báo lỗi. Công cụ không thay thế tệp.

Đây là các tùy chọn thu:

| Tùy chọn | Chức năng | Giá trị ban đầu |
| --- | --- | --- |
| `--channels LIST` | Các kênh cần ghi, ví dụ `0,1,2`. | `0` |
| `--samplerate HZ` | Tốc độ lấy mẫu tính bằng Hz. | `10000000` |
| `--samples N` | Số mẫu cho mỗi kênh. | `1000000` |
| `--vth VOLTS` | Điện áp ngưỡng. | `1.6` |
| `--mode MODE` | `buffer` hoặc `stream`. | `buffer` |
| `--trigger CH[:T]` | Kích hoạt trên kênh CH. Dùng `R` (sườn lên), `F` (sườn xuống), `C` (sườn lên hoặc sườn xuống), `1` (mức cao) hoặc `0` (mức thấp) cho T. | Không kích hoạt. `R` nếu bạn chỉ cho CH. |
| `--trigpos PERCENT` | Vị trí kích hoạt tính theo phần trăm số mẫu. | `10` |
| `--timeout SEC` | Thời gian thu tối đa tính bằng giây. | `30` |
| `--out PATH` | Đường dẫn tệp đầu ra, không có phần mở rộng `.bin`. | Tùy chọn này là bắt buộc. |
| `--log-level N` | Lượng thông báo thư viện trên đầu ra lỗi chuẩn, từ 0 (không có) đến 5 (tất cả). | `1` |

Công cụ kiểm tra tất cả tùy chọn trước khi dùng thiết bị. Nếu một tùy chọn không đúng, công cụ dừng và báo lỗi.

## Tệp đầu ra

Tệp `.bin` chứa các kênh theo thứ tự số kênh, từ số nhỏ nhất. Với mỗi kênh, tệp chứa tất cả mẫu của kênh đó. Mỗi byte chứa 8 mẫu. Mẫu đầu tiên là bit có trọng số thấp nhất. Dữ liệu của mỗi kênh chiếm một số nguyên đơn vị 8 byte. Vì vậy, mỗi kênh dùng `ceil(samples / 64) × 8` byte.

## Kết quả JSON

Công cụ ghi một đối tượng JSON trên một dòng. Sau một lần thu đúng, đối tượng cho biết tên thiết bị, tốc độ lấy mẫu, số mẫu và các kênh. Đối tượng cũng cho biết điện áp ngưỡng, chế độ, kích hoạt, thời gian thu và đường dẫn tệp `.bin`. Nếu lần thu không đúng, đối tượng chứa khóa `error`. Khi đó công cụ không tạo tệp `.bin`.

Chỉ dùng kết quả khi trạng thái thoát là 0 và đối tượng JSON đầy đủ.

## Trạng thái thoát

| Trạng thái | Ý nghĩa |
| --- | --- |
| 0 | Lần thu đã hoàn tất. |
| 1 | Đã xảy ra lỗi trong khi hoạt động, ví dụ lỗi I/O. |
| 2 | Một tùy chọn không đúng, hoặc thiết bị không có một cài đặt. |
| 3 | Lần thu không hoàn tất. |

## Tùy chọn cho chương trình khởi động dslcap

- `--parent-fd N`: Công cụ dừng khi chương trình đã khởi động nó đóng ống dẫn có mô tả N. Sau đó công cụ xóa tệp đầu ra nếu lần thu chưa hoàn tất.
- `--res DIR`: Thư mục chứa các tệp firmware. Thông thường, công cụ tự động tìm thư mục này. Bạn cũng có thể đặt biến môi trường `DSLCAP_RES`.
- `--res-manifest FD`: Công cụ kiểm tra giá trị SHA-256 của mỗi tệp firmware trước khi gửi tệp đến thiết bị.

Tệp `tools/dslcap/README.md` trong mã nguồn có tất cả thông tin về các tùy chọn này.
