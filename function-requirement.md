# TÀI LIỆU ĐẶC TẢ YÊU CẦU PHẦN MỀM (SRS)
## ĐỒ ÁN MÔN HỌC: CÔNG NGHỆ PHẦN MỀM
### HỆ THỐNG ĐIỂM DANH SINH VIÊN BẰNG ĐỊNH VỊ GPS (SMART GPS ATTENDANCE SYSTEM)

* **Tên đề tài:** Hệ thống điểm danh sinh viên thông minh dựa trên định vị GPS 
* **Phiên bản:** 1.0 (Official Draft)
* **Ngày lập:** 31/10/2026
* **Đơn vị thực hiện:** Group_01

---

### BẢNG GHI NHẬN THAY ĐỔI TÀI LIỆU (DOCUMENT REVISION HISTORY)

| Ngày | Phiên bản | Mô tả chi tiết thay đổi | Tác giả |
| :---: | :---: | :--- | :---: |
| 20/10/2026 | 0.1 | Khảo sát bài toán, phân tích hệ thống điểm danh hiện tại của nhà trường | Nhóm đồ án |
| 25/10/2026 | 0.5 | Hoàn thiện mô tả yêu cầu chức năng và phân rã các nhóm người dùng | Nhóm đồ án |
| 28/10/2026 | 0.8 | Bổ sung quy tắc hàng rào địa lý GPS Geofence và thuật toán Haversine | Nhóm đồ án |
| 31/10/2026 | 1.0 | Tích hợp quy tắc nghiệp vụ thời gian (Đúng giờ 5-10 phút đầu ca, Đi trễ Late), hoàn thiện đặc tả chức năng | Nhóm đồ án |

---

### BẢNG CHÚ GIẢI THUẬT NGỮ (GLOSSARY)

1. **GPS (Global Positioning System):** Hệ thống định vị toàn cầu cho phép thiết bị di động xác định tọa độ kinh độ (Longitude) và vĩ độ (Latitude) theo thời gian thực.
2. **Geofence (Hàng rào địa lý ảo):** Vùng ranh giới địa lý xung quanh một điểm tọa độ trung tâm (tâm phòng học) với bán kính $R$ xác định.
3. **Haversine Formula:** Thuật toán tính khoảng cách chính xác theo đường cong mặt cầu giữa hai điểm trên Trái Đất dựa trên tọa độ kinh độ và vĩ độ.
4. **Mock Location / Fake GPS:** Các phần mềm hoặc kỹ thuật giả mạo tọa độ vị trí nhằm đánh lừa hệ thống định vị của thiết bị.
5. **NTP (Network Time Protocol):** Giao thức đồng bộ giờ chuẩn Internet, đảm bảo việc xác định đúng giờ hay đi trễ hoàn toàn căn cứ vào đồng hồ máy chủ, ngăn chặn gian lận chỉnh giờ trên điện thoại.
6. **Self-recorded:** Cơ chế sinh viên tự thao tác điểm danh thông qua thiết bị cá nhân đã được hệ thống xác thực.
7. **LMS (Learning Management System):** Hệ thống quản trị học tập trực tuyến của nhà trường.

---

## I. YÊU CẦU CHỨC NĂNG (MÔ TẢ BẰNG NGÔN NGỮ TỰ NHIÊN)

### 1. Bối cảnh và mục tiêu của hệ thống
Hệ thống ra đời nhằm tự động hóa quy trình điểm danh truyền thống, tiết kiệm thời gian giảng dạy và giải quyết triệt để vấn đề gian lận điểm danh hộ. Kế thừa cấu trúc giao diện quen thuộc từ hệ thống LMS hiện tại của nhà trường (các bảng theo dõi môn học `All courses`, chi tiết từng ca `This course`, tỷ lệ chuyên cần `%`, điểm số buổi học `Points: 10/10` và trạng thái `Remarks: Self-recorded`), hệ thống tích hợp công nghệ **Định vị GPS** và **Hàng rào địa lý (Geofencing)** để xác thực sự hiện diện thực tế của sinh viên trong giảng đường, đồng thời phân loại kết quả điểm danh dựa trên mốc thời gian có mặt.

---

### 2. Quy tắc nghiệp vụ về thời gian và vị trí điểm danh (Business Rules)
Mỗi ca học có giờ bắt đầu chính thức là $T_{start}$ (Ví dụ: 07:00 AM). Quy trình phân loại điểm danh được hệ thống tự động đánh giá như sau:

| Khung thời gian bấm điểm danh ($T_{checkin}$) | Điều kiện vị trí ($d$) | Trạng thái ghi nhận (`Status`) | Điểm số buổi học (`Points`) | Ghi chú trên hệ thống (`Remarks`) |
| :--- | :---: | :---: | :---: | :--- |
| **Từ $T_{start}$ đến $T_{start} + 10$ phút** *(5 - 10 phút đầu ca)* | $d \le R$ (Trong phòng) | **`Present` (Có mặt)** | **10 / 10** | `Self-recorded via GPS (On-time)` |
| **Từ $T_{start} + 11$ phút đến $T_{start} + 30$ phút** | $d \le R$ (Trong phòng) | **`Late` (Đi trễ)** | **5 / 10** *(50% điểm)* | `Self-recorded via GPS (Late: trễ [X] phút)` |
| **Sau $T_{start} + 30$ phút hoặc hết ca học** | Mọi trường hợp | **`Absent` (Vắng mặt)** | **0 / 10** | `Unexcused absence (Khóa tự động)` |
| **Trong giờ mở phiên** | $d > R$ (Ngoài phòng) | **Từ chối điểm danh** | 0 / 10 | Báo lỗi: Ngoài phạm vi phòng học |

---

### 3. Mô tả chi tiết chức năng theo phân hệ

#### 3.1. Phân hệ Sinh viên
1. **Đăng nhập và quản lý thiết bị sử dụng:**
   * Sinh viên đăng nhập vào hệ thống bằng tài khoản và mật khẩu do nhà trường cấp.
   * Nhằm ngăn chặn triệt để tình trạng một sinh viên cầm nhiều điện thoại điểm danh hộ cho bạn, mỗi tài khoản sinh viên chỉ được phép liên kết và điểm danh trên **một thiết bị di động duy nhất** trong suốt ca học đó.
2. **Xem tổng quan điểm danh tất cả môn học (Mục `All courses`):**
   * Hiển thị bảng tổng hợp toàn bộ các môn học sinh viên đã đăng ký trong học kỳ (như *Lập trình hướng đối tượng, Mạng máy tính, Công nghệ phần mềm...*).
   * Tại mỗi môn học, sinh viên theo dõi được: Tên môn và mã môn, số buổi học đã diễn ra (`Taken sessions`), tổng số điểm chuyên cần tích lũy được (`Points over taken sessions`), và tỷ lệ phần trăm chuyên cần (`Percentage`).
   * Hệ thống hiển thị điểm chuyên cần trung bình cộng của tất cả các môn học (`Average attendance`).
3. **Xem chi tiết lịch sử từng ca học của một môn (Mục `This course`):**
   * Cho phép sinh viên bấm vào từng môn học cụ thể để xem danh sách tất cả các ngày học trong tháng hoặc trong kỳ.
   * Bảng chi tiết từng buổi học thể hiện: Ngày học và khung giờ học (`Date`), tên buổi học (`Description`), trạng thái điểm danh (`Status`: Present - Xanh lá, Late - Vàng cam, Absent - Đỏ), số điểm đạt được (`Points`: 10/10, 5/10, hoặc 0/10), và ghi chú hệ thống (`Remarks`: Tự điểm danh bằng GPS, thời gian bấm điểm danh).
4. **Quy trình sinh viên tự thực hiện điểm danh bằng GPS (Self Check-in):**
   * Khi đến giờ học, sinh viên vào môn học tương ứng và nhấn nút **[Điểm danh GPS]**.
   * Hệ thống yêu cầu thiết bị di động cấp quyền truy cập vị trí và tự động lấy tọa độ kinh độ, vĩ độ hiện tại.
   * Hệ thống tự động so khớp tọa độ của sinh viên với tọa độ phòng học và kiểm tra thời gian lúc bấm điểm danh:
     * **Nếu bấm trong 5 đến 10 phút đầu ca học** và sinh viên đang ở trong phòng học (khoảng cách $d \le R$): Ghi nhận **Có mặt (`Present`)**, đạt trọn vẹn **10/10 điểm**.
     * **Nếu bấm từ phút thứ 11 đến phút thứ 30** và ở trong phòng học: Ghi nhận **Đi trễ (`Late`)**, chỉ được tính **5/10 điểm**, kèm thông báo nhắc nhở đi học đúng giờ.
     * **Nếu bấm sau phút thứ 30:** Hệ thống thông báo phiên điểm danh tự động đã kết thúc, tự động ghi nhận **Vắng mặt (`Absent`)**, nhận **0/10 điểm**.
     * **Nếu sinh viên đang ở ngoài phòng học** ($d > R$): Hệ thống từ chối điểm danh và báo rõ khoảng cách sinh viên đang cách giảng đường bao nhiêu mét.
     * **Nếu sinh viên bật phần mềm giả lập vị trí (Fake GPS):** Hệ thống phát hiện và lập tức chặn phiên điểm danh, ghi nhận cảnh báo vi phạm.
5. **Gửi khiếu nại điểm danh:**
   * Trong trường hợp thiết bị gặp lỗi phần cứng GPS, mạng chập chờn, hoặc sinh viên có giấy phép nghỉ học hợp lệ, sinh viên có thể tạo đơn giải trình trực tiếp trên hệ thống kèm hình ảnh minh chứng để giảng viên xem xét duyệt bù.

#### 3.2. Phân hệ Giảng viên
1. **Thiết lập và mở phiên điểm danh GPS cho ca học:**
   * Giảng viên truy cập vào lớp học phần trên lịch giảng dạy và bấm **[Mở phiên điểm danh GPS]**.
   * Hệ thống tự động điền thông tin phòng học và tọa độ kinh độ/vĩ độ chuẩn của phòng học đó theo thời khóa biểu.
   * Giảng viên có thể tùy chỉnh bán kính cho phép quét (từ 20m đến 100m) và cấu hình thời gian chốt đúng giờ (mặc định 10 phút đầu) và thời gian chốt đi trễ (mặc định 30 phút).
   * Giảng viên bấm **Kích hoạt phiên** để bắt đầu mở cổng nhận dữ liệu điểm danh của sinh viên.
2. **Giám sát lớp học trực tiếp theo thời gian thực (Màn hình Radar):**
   * Giảng viên theo dõi trực tiếp màn hình radar thể hiện vòng tròn bán kính phòng học. Các sinh viên check-in thành công sẽ xuất hiện ngay lập tức dưới dạng chấm tròn xanh (đúng giờ) hoặc chấm tròn vàng (đi trễ).
   * Hệ thống tự động đếm sĩ số lớp: Tổng số sinh viên, số lượng đúng giờ, số lượng đi trễ, và danh sách những sinh viên chưa điểm danh.
3. **Điều chỉnh và duyệt bù điểm danh thủ công:**
   * Giảng viên có quyền sửa đổi trạng thái điểm danh cho sinh viên nếu có lý do chính đáng hoặc cập nhật trạng thái **Có phép (`Excused`)**.
4. **Xem thống kê và xuất báo cáo chuyên cần:**
   * Giảng viên xem báo cáo tỷ lệ chuyên cần của toàn lớp, lọc danh sách sinh viên vắng quá 20% số buổi để nhắc nhở hoặc cấm thi.
   * Hỗ trợ xuất toàn bộ bảng điểm danh của lớp ra file Excel hoặc PDF để nộp về khoa hoặc phòng đào tạo.

#### 3.3. Phân hệ Quản trị viên (Admin)
1. **Quản lý danh mục phòng học và tọa độ GPS chuẩn:**
   * Quản trị viên cập nhật danh sách các cơ sở, tòa nhà và từng phòng học của trường.
   * Nhập chính xác tọa độ vĩ độ và kinh độ của từng phòng học cùng bán kính chuẩn.
2. **Quản lý tài khoản và phân quyền:**
   * Tạo mới, chỉnh sửa, khóa tài khoản của giảng viên và sinh viên.
   * Hỗ trợ đặt lại liên kết thiết bị khi sinh viên đổi điện thoại mới.
3. **Đồng bộ thời khóa biểu và lớp học:**
   * Nhập dữ liệu danh sách môn học, lớp tín chỉ và lịch học từ cổng đào tạo chung của trường.
