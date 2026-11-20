# BÁO CÁO GIAI ĐOẠN 3 – TRIỂN KHAI HỆ THỐNG (IMPLEMENTATION SPECIFICATION)
## HỆ THỐNG ĐIỂM DANH SINH VIÊN BẰNG ĐỊNH VỊ GPS VÀ HÀNG RÀO ĐỊA LÝ (GEOFENCING)
**Môn học:** Kỹ thuật Phần mềm (Software Engineering Capstone Project)  
**Nhóm thực hiện:** Nhóm 01 (Group_01)  
**Ngôn ngữ phát triển:** **C++ (C++17 Standard)** & Web Standards (HTML5 / Tailwind CSS / Vanilla JS)  
**Phiên bản đặc tả:** SRS & SDS v2.0

---

## 1. MỤC TIÊU VÀ PHẠM VI GIAI ĐOẠN 3
Giai đoạn 3 hiện thực hóa toàn bộ các đặc tả chức năng (FR-01 đến FR-17), yêu cầu phi chức năng (NFR-01 đến NFR-16), 21 Use Case (UC01 đến UC21), mô hình lớp (Class Diagram) và mô hình dữ liệu quan hệ (ERD 3NF) bằng ngôn ngữ **C++17**:
1. **Kiến trúc phần mềm phân tầng (Layered Clean Architecture)**:
    - **Tầng Trình diễn (Presentation Layer / API Router)**: Máy chủ HTTP Socket C++ đa luồng (Winsock2), phục vụ trọn vẹn 8 màn hình giao diện (Screens 1 - 8) theo đúng wireframe đặc tả và bộ API RESTful JSON.
    - **Tầng Dịch vụ Nghiệp vụ (Business Logic / Domain Services)**:
        - `GeoService`: Tính khoảng cách vòng cung lớn trên mặt cầu trái đất bằng công thức Haversine chính xác theo NFR-04 ($R_{earth} = 6.371.000\text{ m}$), kiểm tra biên geofence $d \le R$.
        - `AttendanceGradingService`: Bộ máy chấm điểm và phân loại trạng thái điểm danh theo ma trận quy tắc nghiệp vụ Section II.2:
            - $[T_{start}-15, T_{start}+15]$ trong Geofence $\implies$ Đúng giờ (Present), **10 / 10 điểm**.
            - $[T_{start}+16, T_{start}+45]$ trong Geofence $\implies$ Đi trễ (Late), **5 / 10 điểm**.
            - Sau phút 45 hoặc kết thúc phiên không điểm danh $\implies$ Vắng không phép (Absent), **0 / 10 điểm**.
            - Ngoài bán kính lớp học ($d > R$) $\implies$ Từ chối điểm danh, **0 / 10 điểm**.
            - Đơn xin phép được Giảng viên DUYỆT $\implies$ Vắng có phép (Excused), **2 / 10 điểm**.
            - Đơn xin phép bị Giảng viên TỪ CHỐI $\implies$ Vắng không phép (Unexcused), **0 / 10 điểm**.
        - `AntiFraudService`: Chống gian lận vị trí giả (Mock Location), kiểm tra bán kính chính xác GPS ($\le 20\text{ m}$ theo NFR-09), và kiểm tra ràng buộc thiết bị vật lý phần cứng (`Device_ID`) chống điểm danh hộ (FR-03 & UC21: 1 thiết bị chỉ được điểm danh cho 1 tài khoản trong cùng 1 buổi học).
        - `AuthService`: Điều phối thông minh (Universal Authentication & Smart Role Router): Cho phép Quản trị viên (Admin) đăng nhập tại bất kỳ tab nào ([ Sinh viên ] hoặc [ Giảng viên ]) và tự động điều hướng trực tiếp vào Admin Dashboard (FR-02).
    - **Tầng Lưu trữ & Truy xuất Dữ liệu (Repository Layer)**: `DatabaseRepository` quản lý đầy đủ 6 bảng quan hệ chuẩn hóa 3NF và nhật ký an ninh bảo mật (Audit Logs) đảm bảo tính toàn vẹn đa luồng với `std::mutex`.

---

## 2. CẤU TRÚC THƯ MỤC NGUỒN C++ (SOURCE TREE ORGANIZATION)
```
gps_attendance_system/
├── build.bat                    # Script tự động biên dịch C++ (tests.exe & server.exe)
├── CMakeLists.txt               # Cấu hình CMake tiêu chuẩn
├── README.md                    # Tài liệu hướng dẫn khởi chạy và tổng quan
├── src/
│   ├── models/
│   │   └── Models.hpp           # Định nghĩa các cấu trúc thực thể: User, Student, Lecturer, Admin, Classroom, CourseClass, AttendanceSession, AttendanceRecord, LeaveRequest, SecurityAuditLog
│   ├── services/
│   │   ├── GeoService.hpp       # Thuật toán Haversine & Geofence spherical geometry
│   │   ├── AttendanceGradingService.hpp # Bộ quy tắc nghiệp vụ thang điểm 10/5/2/0
│   │   ├── AntiFraudService.hpp # Kiểm soát Mock GPS, độ chính xác <=20m & Anti-proxy Device ID
│   │   └── AuthService.hpp      # Xác thực phân quyền & Smart Admin Auto-Routing
│   ├── repository/
│   │   └── DatabaseRepository.hpp # Quản lý dữ liệu quan hệ in-memory đa luồng, seed dữ liệu mẫu
│   ├── server/
│   │   ├── ApiRouter.hpp        # Bộ điều phối RESTful API & JSON Parser
│   │   └── HttpServer.hpp       # Máy chủ HTTP C++ Socket (Winsock2) hỗ trợ đa luồng
│   ├── ui/
│   │   └── WebClient.hpp        # Mã nguồn giao diện Web tương tác trọn vẹn 8 Screens
│   └── main.cpp                 # Điểm khởi chạy chính của ứng dụng C++
├── tests/
│   └── test_suite.cpp           # Bộ kiểm thử đơn vị tự động (Unit Test Suite)
└── docs/
    ├── GIAI_DOAN_3_TRIEN_KHAI.md # Báo cáo chi tiết Giai đoạn 3
    └── GIAI_DOAN_4_THU_NGHIEM.md # Báo cáo chi tiết Giai đoạn 4
```

---

## 3. HIỆN THỰC CÁC CHỨC NĂNG CỐT LÕI (CORE IMPLEMENTATION DETAILS)

### 3.1. Thuật toán khoảng cách Haversine (NFR-04)
Được cài đặt trong `src/services/GeoService.hpp`:
```cpp
static double calculateDistance(double lat1, double lon1, double lat2, double lon2) {
    double phi1 = toRadians(lat1);
    double phi2 = toRadians(lat2);
    double deltaPhi = toRadians(lat2 - lat1);
    double deltaLambda = toRadians(lon2 - lon1);

    double sinHalfDeltaPhi = std::sin(deltaPhi / 2.0);
    double sinHalfDeltaLambda = std::sin(deltaLambda / 2.0);

    double a = sinHalfDeltaPhi * sinHalfDeltaPhi +
               std::cos(phi1) * std::cos(phi2) *
               sinHalfDeltaLambda * sinHalfDeltaLambda;
    if (a > 1.0) a = 1.0;
    if (a < 0.0) a = 0.0;
    double c = 2.0 * std::asin(std::sqrt(a));
    return EARTH_RADIUS_METERS * c; // R_earth = 6,371,000 m
}
```

### 3.2. Chấm điểm theo ma trận quy tắc nghiệp vụ (Section II.2)
Được cài đặt trong `src/services/AttendanceGradingService.hpp`:
- Kiểm tra khoảng cách: nếu $d > R \implies$ Trạng thái `REJECTED`, 0 điểm.
- Kiểm tra khung thời gian $[T_{start}-15, T_{start}+15] \implies$ Trạng thái `PRESENT`, 10 điểm.
- Kiểm tra khung trễ $[T_{start}+16, T_{start}+45] \implies$ Trạng thái `LATE`, 5 điểm, tính số phút trễ cụ thể.
- Quá 45 phút $\implies$ Trạng thái `ABSENT`, 0 điểm.

### 3.3. Ràng buộc thiết bị vật lý & Chống điểm danh hộ (FR-03 & UC21)
Được cài đặt trong `src/services/AntiFraudService.hpp`:
- Lưu trữ ánh xạ `device_id -> student_code` cho từng buổi học (`session`).
- Nếu cùng một thiết bị (`Device_ID`) đã ghi nhận cho Sinh viên A mà tiếp tục gửi yêu cầu cho Sinh viên B $\implies$ Chặn ngay lập tức và ghi nhật ký vi phạm an ninh (`CRITICAL` Audit Log).

### 3.4. Giao diện người dùng Web tích hợp 8 màn hình (Wireframes 8.1 - 8.8)
Được cài đặt trong `src/ui/WebClient.hpp`:
- **Screen 1**: Đăng nhập 2 tab [ Sinh viên ] / [ Giảng viên ] và tự động điều hướng Admin.
- **Screen 2**: Hero banner "Today's Schedule" đếm ngược thời gian mở điểm danh trước 15 phút, nút 1 chạm "CHECK-IN VIA GPS NOW".
- **Screen 3**: Màn hình radar quét tọa độ thực tế, hiển thị khoảng cách $d = 12.5\text{ m}$ (cho phép $35\text{ m}$), xác thực thiết bị và thông báo thành công `10/10`.
- **Screen 4**: Form gửi đơn xin nghỉ học và đính kèm minh chứng bệnh án y tế.
- **Screen 5**: Bảng tổng kết chuyên cần toàn bộ học kỳ (All Courses) và lịch sử chi tiết từng buổi học (This Course) với các huy hiệu màu sắc.
- **Screen 6**: Radar thời gian thực của Giảng viên hiển thị vòng tròn Geofence, các chấm xanh (đúng giờ), chấm vàng (trễ), chấm đỏ (ngoài ranh giới), thống kê trực tiếp.
- **Screen 7**: Bảng xét duyệt đơn xin phép vắng của Giảng viên, nút duyệt (2 điểm) hoặc từ chối (0 điểm), phản hồi giáo viên.
- **Screen 8**: Bảng điều khiển Quản trị viên quản lý danh mục phòng học, hiệu chỉnh tọa độ mốc GPS, gỡ khóa thiết bị sinh viên (Reset Lock), xem nhật ký bảo mật.

---

## 4. HƯỚNG DẪN BIÊN DỊCH VÀ KHỞI CHẠY
1. **Biên dịch bằng lệnh batch**:
   ```cmd
   cd C:\Users\ttron\.gemini\antigravity\scratch\gps_attendance_system
   .\build.bat
   ```
2. **Khởi chạy ứng dụng máy chủ C++**:
   ```cmd
   .\server.exe
   ```
3. **Mở trình duyệt Web**:
   Truy cập `http://127.0.0.1:8080/` để trải nghiệm trực tiếp hệ thống.
