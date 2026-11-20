# GÓI NỘP BÀI DỰ ÁN NHÓM: ProgAndTest_Group01
## WEB-BASED SMART GPS STUDENT ATTENDANCE SYSTEM (C++17)

* **Môn học:** Kỹ thuật Phần mềm (Software Engineering Capstone Project)
* **Khóa học trực tuyến 5 (17/11 - 23/11/2026):** Thực hiện các giai đoạn lập trình và kiểm thử của dự án nhóm.
* **Đơn vị thực hiện:** Nhóm sinh viên thực hiện đề tài (**Group_01**)
* **Ngôn ngữ triển khai:** **C++ (tiêu chuẩn C++17)** và Web Standards (HTML5 / Tailwind CSS / Vanilla JS).
* **Phiên bản đặc tả:** SRS & SDS v2.0 (Dual-Role Authentication, 15' Early Check-in Trigger, 10/5/2/0 Grading Scale, Anti-Proxy Device Binding, Haversine Spherical Geofence, 8 Wireframe UI Screens).

---

## 1. CẤU TRÚC THƯ MỤC NỘP BÀI (PROGANDTEST_GROUP01)

Thư mục nộp bài được tổ chức đầy đủ và chuẩn mực theo đúng 4 thành phần yêu cầu của giảng viên:

```text
ProgAndTest_Group01/
├── Dockerfile                         # [Yêu cầu 3] Tệp Docker đóng gói ứng dụng (Multi-stage build)
├── docker-compose.yml                 # [Yêu cầu 3] Cấu hình chạy Server và Test Runner bằng Docker
├── .dockerignore                      # Tệp cấu hình loại trừ cho Docker
├── build.bat                          # Script tự động biên dịch toàn bộ dự án trên Windows (MinGW g++)
├── build.sh                           # Script tự động biên dịch toàn bộ dự án trên Linux / macOS
├── CMakeLists.txt                     # Cấu hình biên dịch chuẩn CMake
├── server.exe                         # Ứng dụng Máy chủ Web & REST API (Đã biên dịch sẵn, chạy ngay)
├── tests.exe                          # Bộ kiểm thử đơn vị tự động 26/26 tests (Đã biên dịch sẵn)
│
├── Tai_Lieu_Kiem_Thu.md               # [Yêu cầu 2] Tài liệu kiểm thử chi tiết & Ma trận 26 ca kiểm thử
├── Bang_Chung_Cong_Cu_Va_Phan_Cong.md # [Yêu cầu 4] Bằng chứng công cụ (Git, Figma, Jira...) & RACI
├── README.md                          # Tài liệu hướng dẫn sử dụng và tổng quan dự án
│
├── src/                               # [Yêu cầu 1] MÃ NGUỒN C++ (.cpp, .hpp)
│   ├── main.cpp                       # Điểm khởi động hệ thống
│   ├── models/
│   │   └── Models.hpp                 # 10 thực thể dữ liệu ánh xạ chuẩn ERD 3NF và Class Diagram
│   ├── services/
│   │   ├── GeoService.hpp             # Tính khoảng cách hình cầu Haversine & Geofence
│   │   ├── AttendanceGradingService.hpp # Động cơ tính điểm nghiệp vụ 10/5/2/0 theo mốc thời gian
│   │   ├── AntiFraudService.hpp       # Kiểm tra Fake GPS, ngưỡng accuracy <= 20m & khóa thiết bị
│   │   └── AuthService.hpp            # Xác thực người dùng & Smart Admin Auto-Routing
│   ├── repository/
│   │   └── DatabaseRepository.hpp     # Kho dữ liệu quan hệ in-memory đa luồng an toàn (std::mutex)
│   ├── server/
│   │   ├── ApiRouter.hpp              # Phân tuyến REST API JSON (UC01 đến UC21)
│   │   └── HttpServer.hpp             # Máy chủ HTTP đa luồng C++ (Winsock2 / POSIX sockets)
│   └── ui/
│       └── WebClient.hpp              # Giao diện Web tương tác hoàn chỉnh bao phủ đủ 8 màn hình wireframe
│
├── tests/                             # [Yêu cầu 1 & 2] MÃ NGUỒN BỘ KIỂM THỬ ĐƠN VỊ C++
│   └── test_suite.cpp                 # Bộ 26 ca kiểm thử đơn vị tự động (100% Pass)
│
└── docs/                              # TÀI LIỆU BÁO CÁO KỸ THUẬT VÀ QUẢN TRỊ
    ├── Tai_Lieu_Kiem_Thu.md           # Bản lưu tài liệu kiểm thử trong thư mục docs
    ├── Bang_Chung_Cong_Cu_Va_Phan_Cong.md # Bản lưu báo cáo phân công và bằng chứng công cụ
    ├── GIAI_DOAN_3_TRIEN_KHAI.md      # Báo cáo kỹ thuật chi tiết Giai đoạn 3 (Triển khai phần mềm)
    └── GIAI_DOAN_4_THU_NGHIEM.md      # Báo cáo kỹ thuật chi tiết Giai đoạn 4 (Thử nghiệm & Đánh giá)
```

---

## 2. HƯỚNG DẪN BIÊN DỊCH VÀ CHẠY HỆ THỐNG

### Cách 1: Chạy trực tiếp trên Windows (Đã có sẵn file .exe)
Không cần cài đặt thêm bất kỳ thư viện nào, bạn có thể thực thi ngay:
1. **Chạy bộ kiểm thử tự động 26 ca kiểm thử:**
   ```cmd
   .\tests.exe
   ```
2. **Khởi động Máy chủ Web & REST API:**
   ```cmd
   .\server.exe
   ```
   Sau khi khởi động, mở trình duyệt web bất kỳ tại địa chỉ: **`http://127.0.0.1:8080`** để trải nghiệm đầy đủ 8 màn hình wireframe tương tác trực tiếp!

### Cách 2: Tự biên dịch lại từ mã nguồn (Rebuild)
* **Trên Windows (MinGW-w64 GCC):**
  ```cmd
  .\build.bat
  ```
* **Trên Linux / macOS:**
  ```bash
  chmod +x build.sh
  ./build.sh
  ```

### Cách 3: Chạy bằng Docker & Docker Compose
* **Biên dịch và khởi chạy Web Server:**
  ```bash
  docker compose up -d
  ```
  Truy cập ứng dụng tại `http://localhost:8080`.
* **Chạy bộ kiểm thử tự động trong môi trường Container:**
  ```bash
  docker compose --profile test run attendance-tests
  ```

---

## 3. TÀI KHOẢN TRẢI NGHIỆM HỆ THỐNG (DEMO CREDENTIALS)

Hệ thống đã nạp sẵn (seed data) các tài khoản mẫu theo đặc tả SRS:
* **Sinh viên 1:** MSSV: `B22DCCN001` | Mật khẩu: `123456`
* **Sinh viên 2:** MSSV: `B22DCCN045` | Mật khẩu: `123456`
* **Giảng viên:** Mã GV: `GV001` | Mật khẩu: `123456`
* **Quản trị viên (Admin):** Tài khoản: `admin` | Mật khẩu: `admin123`
  *(Lưu ý: Admin có thể nhập vào bất kỳ tab nào [ Sinh viên ] hoặc [ Giảng viên ], hệ thống sẽ tự động nhận diện và chuyển hướng vào trang Quản trị viên).*

---

## 4. TỔNG KẾT KẾT QUẢ ĐẠT ĐƯỢC

1. **Giai đoạn 3 (Triển khai):**
    * Hiện thực hóa đầy đủ 21 Use Case (UC01 -> UC21).
    * Áp dụng kiến trúc Clean Multi-tier Architecture phân tầng chặt chẽ.
    * Triển khai công thức Haversine $d = 2 R \arcsin(\dots)$ với $R = 6,371,000\text{m}$.
    * Cơ chế chống gian lận đa tầng: Anti-mock GPS, GPS accuracy <= 20m, Anti-proxy Device Fingerprint.
2. **Giai đoạn 4 (Thử nghiệm):**
    * 26 ca kiểm thử đơn vị và tích hợp được tự động hóa hoàn toàn bằng C++.
    * **Tỷ lệ vượt qua: 26 / 26 PASSED (100%)**.
3. **Đóng gói nộp bài:**
    * Tệp lưu trữ `.zip`: `ProgAndTest_Group01.zip` sẵn sàng nộp lên cổng trường.
