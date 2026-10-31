#                  TÀI LIỆU ĐẶC TẢ YÊU CẦU PHẦN MỀM (SRS)

### HỆ THỐNG ĐIỂM DANH SINH VIÊN BẰNG ĐỊNH VỊ GPS VÀ HÀNG RÀO ĐỊA LÝ (GEOFENCING)

* **Tên đề tài:** Hệ thống điểm danh sinh viên thông minh dựa trên định vị GPS 
* **Phiên bản:** 2.0 (Official Draft)
* **Ngày lập:** 31/10/2026
* **Đơn vị thực hiện:** Group_01

---

### BẢNG GHI NHẬN THAY ĐỔI TÀI LIỆU (DOCUMENT REVISION HISTORY)

| Ngày | Phiên bản | Mô tả chi tiết thay đổi | Tác giả |
| :---: | :---: | :--- | :---: |
| 20/10/2026 | 0.1 | Khảo sát bài toán, phân tích hệ thống điểm danh hiện tại của nhà trường | Nhóm đồ án |
| 25/10/2026 | 0.5 | Hoàn thiện mô tả yêu cầu chức năng và phân rã các nhóm người dùng | Nhóm đồ án |
| 28/10/2026 | 0.8 | Bổ sung quy tắc hàng rào địa lý GPS Geofence và thuật toán Haversine | Nhóm đồ án |
| 31/10/2026 | 2.0 | Bổ sung chức năng Đăng nhập + Ràng buộc thiết bị, Lịch học hôm nay, quy tắc nghiệp vụ thời gian (Đúng giờ / Đi trễ / Vắng), hoàn thiện toàn bộ đặc tả chức năng | Nhóm đồ án |

---

### I.BẢNG CHÚ GIẢI THUẬT NGỮ (GLOSSARY)

1. **GPS (Global Positioning System):** Hệ thống định vị toàn cầu cho phép thiết bị di động xác định tọa độ kinh độ (Longitude) và vĩ độ (Latitude) theo thời gian thực.
2. **Geofence (Hàng rào địa lý ảo):** Vùng ranh giới địa lý xung quanh một điểm tọa độ trung tâm (tâm phòng học) với bán kính R xác định. Khi thiết bị nằm trong vùng này, hệ thống xác nhận sinh viên đang có mặt tại phòng học.
3. **Haversine Formula:** Thuật toán tính khoảng cách chính xác theo đường cong mặt cầu giữa hai điểm trên Trái Đất dựa trên tọa độ kinh độ và vĩ độ.
4. **Mock Location / Fake GPS:** Các phần mềm hoặc kỹ thuật giả mạo tọa độ vị trí nhằm đánh lừa hệ thống định vị của thiết bị.
5. **NTP (Network Time Protocol):** Giao thức đồng bộ giờ chuẩn Internet, đảm bảo việc xác định đúng giờ hay đi trễ hoàn toàn căn cứ vào đồng hồ máy chủ, ngăn chặn gian lận chỉnh giờ trên điện thoại.
6. **Self-recorded:** Cơ chế sinh viên tự thao tác điểm danh thông qua thiết bị cá nhân đã được hệ thống xác thực.
7. **LMS (Learning Management System):** Hệ thống quản trị học tập trực tuyến của nhà trường.
8. **Device Fingerprint (Dấu vân tay thiết bị):** Tập hợp thông tin phần cứng và phần mềm đặc trưng của một thiết bị di động (số IMEI, model, hệ điều hành...) dùng để xác định duy nhất thiết bị đó.
9. **SSO Login (Single Sign-On):** Đăng nhập một lần dùng tài khoản nhà trường để truy cập nhiều hệ thống liên quan mà không cần đăng nhập lại.

---


---

## II. YÊU CẦU CHỨC NĂNG (MÔ TẢ BẰNG NGÔN NGỮ TỰ NHIÊN)

### 1. Bối cảnh và mục tiêu của hệ thống

Hệ thống ra đời nhằm tự động hóa quy trình điểm danh truyền thống, tiết kiệm thời gian giảng dạy và giải quyết triệt để vấn đề gian lận điểm danh hộ. Kế thừa cấu trúc giao diện quen thuộc từ hệ thống LMS hiện tại của nhà trường (các bảng theo dõi môn học `All courses`, chi tiết từng ca `This course`, tỷ lệ chuyên cần `%`, điểm số buổi học `Points: 10/10` và trạng thái `Remarks: Self-recorded`), hệ thống tích hợp công nghệ **Định vị GPS** và **Hàng rào địa lý (Geofencing)** để xác thực sự hiện diện thực tế của sinh viên trong giảng đường, đồng thời phân loại kết quả điểm danh dựa trên mốc thời gian có mặt.

---

### 2. Quy tắc nghiệp vụ về thời gian và vị trí điểm danh (Business Rules)

Mỗi ca học có giờ bắt đầu chính thức là T_start (Ví dụ: 07:00 AM). Quy trình phân loại điểm danh được hệ thống tự động đánh giá như sau:

| Khung thời gian bấm điểm danh (T_checkin) | Điều kiện vị trí (d) | Trạng thái ghi nhận (Status) | Điểm số buổi học (Points) | Ghi chú trên hệ thống (Remarks) |
| :--- | :---: | :---: | :---: | :--- |
| **Từ T_start đến T_start + 10 phút** *(5 - 10 phút đầu ca)* | d ≤ R (Trong phòng) | **Present (Có mặt)** | **10 / 10** | `Self-recorded via GPS (On-time)` |
| **Từ T_start + 11 phút đến T_start + 30 phút** | d ≤ R (Trong phòng) | **Late (Đi trễ)** | **5 / 10** *(50% điểm)* | `Self-recorded via GPS (Late: trễ [X] phút)` |
| **Sau T_start + 30 phút hoặc hết ca học** | Mọi trường hợp | **Absent (Vắng mặt)** | **0 / 10** | `Unexcused absence (Khóa tự động)` |
| **Trong giờ mở phiên** | d > R (Ngoài phòng) | **Từ chối điểm danh** | 0 / 10 | Báo lỗi: Ngoài phạm vi phòng học |

> **Giải thích ký hiệu:**
> - **T_start:** Giờ bắt đầu chính thức của ca học (ví dụ: 07:00 AM)
> - **T_checkin:** Thời điểm sinh viên bấm nút điểm danh trên hệ thống
> - **d:** Khoảng cách thực tế giữa vị trí GPS của sinh viên và tọa độ trung tâm phòng học (tính bằng mét, sử dụng công thức Haversine)
> - **R:** Bán kính hàng rào địa lý (Geofence) do giảng viên thiết lập cho phòng học đó (mặc định 50m, cho phép tùy chỉnh từ 20m đến 100m)

---

### 3. Mô tả chi tiết chức năng theo phân hệ

#### 3.1. Phân hệ Sinh viên

**Chức năng 1 — Đăng nhập và ràng buộc thiết bị (Login + Device Binding):**

Sinh viên đăng nhập vào hệ thống bằng tài khoản và mật khẩu do nhà trường cấp (hỗ trợ đăng nhập SSO nếu trường đã triển khai). Khi đăng nhập lần đầu tiên trên một thiết bị di động, hệ thống sẽ ghi nhận **dấu vân tay thiết bị (Device Fingerprint)** bao gồm thông tin phần cứng đặc trưng của thiết bị đó.

Nhằm ngăn chặn triệt để tình trạng một sinh viên cầm nhiều điện thoại điểm danh hộ cho bạn, mỗi tài khoản sinh viên chỉ được phép liên kết và điểm danh trên **một thiết bị di động duy nhất** tại một thời điểm. Nếu sinh viên cần đổi thiết bị mới (do mất điện thoại, hỏng máy...), sinh viên phải liên hệ Quản trị viên để được đặt lại liên kết thiết bị.

---

**Chức năng 2 — Xem lịch học hôm nay và điểm danh nhanh (Today's Schedule):**

Ngay sau khi đăng nhập thành công, màn hình chính hiển thị một **banner nổi bật** thông báo lịch học của ngày hôm nay. Banner này cho sinh viên biết:

- Hôm nay học **môn gì** (Ví dụ: "Công nghệ phần mềm — Nhóm 01")
- Học tại **phòng nào**, **tòa nhà nào** (Ví dụ: "Phòng A305, Cơ sở Nguyễn Văn Cừ")
- **Khung giờ học** (Ví dụ: "07:00 — 09:30")
- **Trạng thái phiên điểm danh** hiện tại:
  - 🟢 `ĐANG MỞ` — Giảng viên đã kích hoạt phiên, sinh viên có thể bấm điểm danh ngay
  - 🔴 `CHƯA MỞ` — Chưa đến giờ hoặc giảng viên chưa mở phiên
  - ⚫ `ĐÃ ĐÓNG` — Phiên điểm danh đã kết thúc

Nếu hôm nay sinh viên có nhiều ca học (sáng và chiều), hệ thống sẽ liệt kê tất cả các ca và tự động đánh dấu ca nào đang trong khung giờ hiện tại. Nếu hôm nay không có lịch học, banner sẽ hiển thị thông báo: "Hôm nay bạn không có lịch học nào. Nghỉ ngơi nhé! 🎉"

---

**Chức năng 3 — Xem tổng quan điểm danh tất cả môn học (All courses):**

Hiển thị bảng tổng hợp toàn bộ các môn học sinh viên đã đăng ký trong học kỳ (như *Lập trình hướng đối tượng, Mạng máy tính, Công nghệ phần mềm...*). Tại mỗi môn học, sinh viên theo dõi được:

- **Tên môn và mã môn** (Course)
- **Hình thức điểm danh** (Attendance type): GPS Self-recorded
- **Số buổi học đã diễn ra** (Taken sessions)
- **Tổng số điểm chuyên cần tích lũy được** (Points over taken sessions)
- **Tỷ lệ phần trăm chuyên cần** (Percentage over taken sessions)

Hệ thống hiển thị **điểm chuyên cần trung bình cộng** của tất cả các môn học (Average attendance) ở cuối bảng.

---

**Chức năng 4 — Xem chi tiết lịch sử từng ca học của một môn (This course):**

Cho phép sinh viên bấm vào từng môn học cụ thể trong bảng `All courses` để xem danh sách tất cả các ngày học đã diễn ra. Bảng chi tiết từng buổi học thể hiện:

| Cột hiển thị | Mô tả | Ví dụ |
| :--- | :--- | :--- |
| **Date** | Ngày học và khung giờ học | 31/10/2026 (07:00 - 09:30) |
| **Description** | Tên buổi học / nội dung giảng dạy | Buổi 8: Kiểm thử phần mềm |
| **Status** | Trạng thái điểm danh, hiển thị bằng màu sắc | 🟢 Present / 🟡 Late / 🔴 Absent |
| **Points** | Số điểm đạt được trong buổi học đó | 10/10, 5/10, hoặc 0/10 |
| **Remarks** | Ghi chú hệ thống về phương thức điểm danh | Self-recorded via GPS (On-time) |

---

**Chức năng 5 — Quy trình sinh viên tự thực hiện điểm danh bằng GPS (GPS Self Check-in):**

Đây là chức năng cốt lõi của toàn bộ hệ thống. Khi phiên điểm danh đang ở trạng thái `ĐANG MỞ`, sinh viên thực hiện các bước sau:

**Bước 1:** Sinh viên mở ứng dụng / trang web hệ thống điểm danh trên điện thoại.

**Bước 2:** Từ màn hình `Today's Schedule` hoặc `This course`, sinh viên nhấn nút **[📍 Điểm danh GPS]**.

**Bước 3:** Hệ thống yêu cầu thiết bị di động cấp quyền truy cập vị trí (Location Permission) và tự động lấy tọa độ kinh độ, vĩ độ hiện tại của sinh viên.

**Bước 4:** Hệ thống thực hiện 3 bước kiểm tra tự động:

- **Kiểm tra thiết bị:** Xác minh thiết bị hiện tại khớp với thiết bị đã đăng ký (Device Fingerprint). Nếu không khớp → Từ chối.
- **Kiểm tra vị trí:** Tính khoảng cách d giữa tọa độ GPS của sinh viên và tọa độ trung tâm phòng học bằng công thức Haversine. Nếu d > R (bán kính Geofence) → Từ chối, thông báo khoảng cách cách phòng học bao nhiêu mét.
- **Kiểm tra giả lập vị trí:** Phát hiện ứng dụng Fake GPS / Mock Location đang chạy trên thiết bị. Nếu phát hiện → Chặn phiên điểm danh, ghi nhận cảnh báo gian lận.

**Bước 5:** Nếu cả 3 bước kiểm tra đều hợp lệ, hệ thống đánh giá thời gian bấm điểm danh:

- **Bấm trong 5 đến 10 phút đầu ca học:** Ghi nhận **Có mặt (Present)**, đạt trọn vẹn **10/10 điểm**. Màn hình hiển thị ✅ kèm thông báo chúc mừng.
- **Bấm từ phút thứ 11 đến phút thứ 30:** Ghi nhận **Đi trễ (Late)**, chỉ được tính **5/10 điểm**. Màn hình hiển thị ⚠️ kèm thông báo nhắc nhở đi học đúng giờ và số phút trễ.
- **Bấm sau phút thứ 30:** Hệ thống thông báo phiên điểm danh đã đóng, tự động ghi nhận **Vắng mặt (Absent)**, nhận **0/10 điểm**.

---

**Chức năng 6 — Gửi khiếu nại điểm danh:**

Trong trường hợp thiết bị gặp lỗi phần cứng GPS, mạng chập chờn, tín hiệu yếu trong tòa nhà, hoặc sinh viên có giấy phép nghỉ học hợp lệ, sinh viên có thể tạo **đơn giải trình** trực tiếp trên hệ thống. Đơn giải trình bao gồm:

- Chọn buổi học cần giải trình
- Lý do giải trình (chọn từ danh sách hoặc nhập tự do)
- Đính kèm hình ảnh minh chứng (ảnh chụp lỗi GPS, giấy phép nghỉ...)
- Trạng thái đơn: `Đang chờ duyệt` → `Được duyệt` / `Bị từ chối`

Giảng viên sẽ nhận thông báo và xem xét duyệt bù điểm danh cho sinh viên.

---

#### 3.2. Phân hệ Giảng viên

**Chức năng 1 — Thiết lập và mở phiên điểm danh GPS cho ca học:**

Giảng viên truy cập vào lớp học phần trên lịch giảng dạy và bấm **[Mở phiên điểm danh GPS]**. Hệ thống tự động điền thông tin:

- Tên môn học, mã lớp, phòng học
- Tọa độ kinh độ / vĩ độ chuẩn của phòng học đó (lấy từ danh mục phòng học do Admin quản lý)
- Thời gian bắt đầu ca học theo thời khóa biểu

Giảng viên có thể tùy chỉnh:

- **Bán kính Geofence** cho phép quét (từ 20m đến 100m, mặc định 50m)
- **Thời gian chốt đúng giờ** (mặc định 10 phút đầu ca)
- **Thời gian chốt đi trễ** (mặc định 30 phút)

Sau khi kiểm tra thông tin, giảng viên bấm **[Kích hoạt phiên]** để bắt đầu mở cổng nhận dữ liệu điểm danh. Phiên sẽ tự động đóng sau thời gian chốt hoặc giảng viên có thể đóng thủ công bất kỳ lúc nào.

---

**Chức năng 2 — Giám sát lớp học trực tiếp theo thời gian thực (Màn hình Radar):**

Sau khi kích hoạt phiên, giảng viên theo dõi trực tiếp **màn hình Radar** thể hiện vòng tròn bán kính phòng học trên bản đồ. Các sinh viên check-in thành công sẽ xuất hiện ngay lập tức dưới dạng:

- 🟢 **Chấm xanh** — Sinh viên điểm danh đúng giờ (Present)
- 🟡 **Chấm vàng** — Sinh viên đi trễ (Late)

Hệ thống tự động đếm và cập nhật sĩ số theo thời gian thực:

- Tổng số sinh viên đăng ký lớp
- Số lượng đã điểm danh đúng giờ
- Số lượng đi trễ
- Danh sách sinh viên chưa điểm danh (có thể nhấn để xem chi tiết)

---

**Chức năng 3 — Điều chỉnh và duyệt bù điểm danh thủ công:**

Giảng viên có quyền:

- Sửa đổi trạng thái điểm danh cho sinh viên nếu có lý do chính đáng (ví dụ: sinh viên có mặt nhưng thiết bị hỏng GPS)
- Cập nhật trạng thái **Có phép (Excused)** cho sinh viên có giấy phép nghỉ hợp lệ
- Xem xét và duyệt / từ chối các đơn giải trình khiếu nại từ sinh viên

---

**Chức năng 4 — Xem thống kê và xuất báo cáo chuyên cần:**

Giảng viên xem báo cáo tỷ lệ chuyên cần tổng hợp của toàn lớp, bao gồm:

- Biểu đồ tỷ lệ Present / Late / Absent theo từng buổi học
- Danh sách sinh viên có tỷ lệ vắng quá 20% số buổi (cảnh báo cấm thi)
- Hỗ trợ xuất toàn bộ bảng điểm danh ra file **Excel (.xlsx)** hoặc **PDF** để nộp về khoa hoặc phòng đào tạo

---

**Chức năng 5 — Quản lý thông tin ca học và nội dung buổi dạy:**

Giảng viên có thể cập nhật tên / mô tả nội dung cho từng buổi học (cột `Description` trong bảng `This course`), giúp sinh viên và bộ môn dễ dàng theo dõi tiến trình giảng dạy.

---

#### 3.3. Phân hệ Quản trị viên (Admin)

**Chức năng 1 — Quản lý danh mục phòng học và tọa độ GPS chuẩn:**

Quản trị viên cập nhật và duy trì danh sách toàn bộ cơ sở hạ tầng phòng học của trường:

- **Cơ sở** (Campus): Ví dụ — Cơ sở Nguyễn Văn Cừ, Cơ sở Linh Trung
- **Tòa nhà** (Building): Ví dụ — Tòa A, Tòa B, Tòa C
- **Phòng học** (Classroom): Ví dụ — A305, B201, C102
- **Tọa độ GPS** chính xác của từng phòng học: Kinh độ (Longitude) và Vĩ độ (Latitude)
- **Bán kính chuẩn** (Default Geofence Radius): Giá trị mặc định cho bán kính hàng rào địa lý của phòng học đó

---

**Chức năng 2 — Quản lý tài khoản và phân quyền:**

- Tạo mới, chỉnh sửa, vô hiệu hóa (khóa) tài khoản của giảng viên và sinh viên
- Phân quyền truy cập: Sinh viên, Giảng viên, Quản trị viên
- **Đặt lại liên kết thiết bị (Reset Device Binding):** Khi sinh viên đổi điện thoại mới, Admin xác minh danh tính và thực hiện đặt lại để sinh viên có thể đăng ký thiết bị mới
- Xem lịch sử đăng nhập và lịch sử thay đổi thiết bị của từng tài khoản

---

**Chức năng 3 — Đồng bộ thời khóa biểu và lớp học:**

- Nhập dữ liệu danh sách môn học, lớp tín chỉ và lịch học từ cổng đào tạo chung của trường
- Tự động phân bổ sinh viên vào đúng lớp học phần đã đăng ký
- Đồng bộ thông tin phòng học, khung giờ học để hệ thống tự động hiển thị đúng lịch học hôm nay (`Today's Schedule`) cho sinh viên và giảng viên

---

