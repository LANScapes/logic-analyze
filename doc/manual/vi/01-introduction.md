# Giới thiệu

## Về Logic Analyze

Logic Analyze là ứng dụng macOS cho máy phân tích logic của DreamSourceLab. LANScapes cung cấp ứng dụng này. Ứng dụng này có nguồn gốc từ DSView, một chương trình của DreamSourceLab. DSView dùng phần mềm của dự án sigrok.

Ứng dụng ghi tín hiệu số từ máy phân tích logic DSLogic. Sau đó ứng dụng hiển thị tín hiệu dưới dạng dạng sóng. Bạn có thể đo dạng sóng và giải mã giao thức nối tiếp. Bạn cũng có thể lưu dữ liệu và xuất dữ liệu sang định dạng khác.

Hướng dẫn này dùng DSLogic Plus làm ví dụ. Các mẫu DSLogic khác hoạt động theo cùng quy trình. Giới hạn về kênh, bộ nhớ và tốc độ lấy mẫu của chúng khác nhau.

Ứng dụng cũng có công cụ `dslcap`. Công cụ này thu dữ liệu mà không cần cửa sổ chính. Xem [Công cụ dslcap](13-dslcap.md).

## Về hướng dẫn này

Hướng dẫn này theo ASD-STE100 Simplified Technical English. Mỗi câu đều ngắn. Mỗi bước trong quy trình chỉ có một chỉ dẫn. Mỗi thuật ngữ kỹ thuật trong hướng dẫn này chỉ có một nghĩa. [Thuật ngữ và động từ kỹ thuật](15-terms.md) có danh sách các thuật ngữ kỹ thuật.

Hướng dẫn này dùng các định dạng văn bản sau:

- **Chữ đậm** chỉ nhãn trong ứng dụng, ví dụ nút, mục menu hoặc trường.
- `Chữ mã` chỉ phím trên bàn phím, lệnh, tên tệp hoặc giá trị bạn nhập.
- Đường dẫn qua menu dùng dấu ›, ví dụ **Tệp** › **Lưu...**.
- Danh sách bước có đánh số là một quy trình. Làm các bước theo đúng thứ tự.

## Chỉ dẫn an toàn

Hướng dẫn này dùng các nhãn sau cho chỉ dẫn an toàn:

- **CẢNH BÁO** chỉ nguy cơ gây thương tích hoặc tử vong.
- **THẬN TRỌNG** chỉ nguy cơ hư hỏng thiết bị hoặc nguy cơ cho dữ liệu của bạn.
- **LƯU Ý** cung cấp thông tin giúp bạn. Thông tin sau **LƯU Ý** không phải là chỉ dẫn.

Chỉ dẫn an toàn đứng trước bước mà nó áp dụng. Đọc tất cả chỉ dẫn an toàn trước khi bắt đầu quy trình.

> [!WARNING]
> Không nối đầu đo với điện áp lưới. Không nối đầu đo với mạch có kết nối điện với điện áp lưới. Đầu đo có kết nối điện với máy tính. Điện áp này có thể gây thương tích hoặc tử vong.

> [!CAUTION]
> Không đặt lên đầu vào kênh một điện áp cao hơn giới hạn trong thông số kỹ thuật của thiết bị. Điện áp quá cao có thể làm hỏng máy phân tích logic.

> [!CAUTION]
> Dây đất của máy phân tích logic nối với đất của máy tính qua cáp USB. Chỉ nối dây đất với đất của mạch bạn đo. Nếu hai đất có điện áp khác nhau, dòng điện lớn có thể chạy qua và làm hỏng mạch, máy phân tích logic và máy tính.

## Yêu cầu hệ thống

Cần có thiết bị sau:

- Máy Mac chạy macOS. Ghi chú phát hành cho biết phiên bản macOS tối thiểu.
- Cổng USB. Cổng USB 3.0 cho tốc độ cao nhất. Cổng USB 2.0 cũng hoạt động.
- Máy phân tích logic DSLogic, cáp USB và cáp đầu đo của nó.

Bạn có thể dùng ứng dụng mà không cần máy phân tích logic. Thiết bị **Mô phỏng** tạo tín hiệu thử. Bạn cũng có thể mở tệp dữ liệu từ lần thu trước.
