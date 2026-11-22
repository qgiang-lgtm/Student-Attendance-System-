# PROJECT DELIVERABLE PACKAGE: ProgAndTest_Group01
## WEB-BASED SMART GPS STUDENT ATTENDANCE SYSTEM (C++17)

* **Course:** Software Engineering (CS / SE Capstone Project)
* **Online Course Module 5 (Nov 17 - Nov 23, 2026):** Program Implementation and Testing Phases.
* **Author / Implementation Unit:** Student Project Team (**Group_01**)
* **Programming Language:** **C++ (Standard C++17)** and Modern Web Standards (HTML5 / Tailwind CSS / Vanilla JS).
* **Specification Standards:** SRS & System Design Specification v2.0 (Dual-Role Authentication, 15-Minute Early Check-in Trigger, 10/5/2/0 Points Scale, Anti-Proxy Hardware Binding, Haversine Spherical Geofence, 8 Wireframe UI Screens).

---

## 1. SUBMISSION PACKAGE STRUCTURE (PROGANDTEST_GROUP01)

The deliverable package is organized strictly in compliance with course instructor requirements across all 4 mandatory components:

```text
ProgAndTest_Group01/
├── docker/
│   ├── Dockerfile                     # [Requirement 3] Multi-stage build Dockerfile (gcc:13 -> debian-slim)
│   └── docker-compose.yml             # [Requirement 3] Docker Compose setup for Web Server & Test Runner
├── .dockerignore                      # Docker ignore rules
├── build.bat                          # Automated compilation script for Windows (MinGW g++)
├── build.sh                           # Automated compilation script for Linux / macOS
├── run_server.bat                     # 1-Click launcher for Web Server & Browser
├── CMakeLists.txt                     # Standard CMake build configuration
├── server.exe                         # Compiled Web Application & REST API Server (Ready to run)
├── tests.exe                          # Compiled Automated Unit Test Runner (31/31 Tests Passed)
│
├── Testing_Document_Group01.xlsx      # [Requirement 2] 100-test-case matrix in the course Excel template format
├── Tai_Lieu_Kiem_Thu.md               # [Requirement 2] Comprehensive Software Testing Documentation & 31 Test Cases
├── Bang_Chung_Cong_Cu_Va_Phan_Cong.docx # [Requirement 4] Tool Usage Evidence & Team Contribution Evaluation Table
├── PHONE.md                           # How to use the app on a real phone (LAN + Add to Home Screen)
├── README.md                          # Project overview and execution guide
│
├── scripts/
│   └── phone-url.ps1                  # Prints the LAN URL and opens Windows Firewall port 8080
│
├── src/                               # [Requirement 1] C++ SOURCE CODE (.cpp, .hpp)
│   ├── main.cpp                       # Server bootstrap and entry point
│   ├── models/
│   │   └── Models.hpp                 # 10 Domain entities strictly mapped to 3NF ERD & UML Class Diagram
│   ├── services/
│   │   ├── GeoService.hpp             # Spherical Haversine distance & geofence validation (NFR-04)
│   │   ├── AttendanceGradingService.hpp # Core grading engine for 10/5/2/0 points calculation
│   │   ├── AntiFraudService.hpp       # Mock location detection, accuracy filtering (<=20m) & device binding
│   │   └── AuthService.hpp            # Dual-role authentication & Smart Admin auto-routing (FR-01, FR-02)
│   ├── repository/
│   │   └── DatabaseRepository.hpp     # Thread-safe in-memory 3NF relational repository (std::mutex)
│   ├── server/
│   │   ├── ApiRouter.hpp              # JSON REST API router, PWA assets, UC01 through UC24
│   │   └── HttpServer.hpp             # Cross-platform multi-threaded HTTP Server (Winsock2 / POSIX sockets)
│   └── ui/
│       └── WebClient.hpp              # Responsive interactive Web client rendering all 8 wireframe screens
│
├── tests/                             # [Requirements 1 & 2] AUTOMATED UNIT TEST SUITE
│   └── test_suite.cpp                 # 31 Google Test (gtest) unit tests covering NFRs and Use Cases (100% Pass)
│
└── docs/                              # TECHNICAL SPECIFICATIONS & EVALUATION REPORTS
    ├── Software_Requirements_Specification.md # Full SRS (v2.0)
    ├── Testing.md                     # Software Testing Documentation
    ├── Bang_Chung_Cong_Cu_Va_Phan_Cong.docx # Tool usage evidence & Team member evaluation table
    ├── Stage_3.md                     # Detailed Stage 3 Implementation Report
    └── Stage_4.md                     # Detailed Stage 4 Testing & Verification Report
```

---

## 2. QUICK START & EXECUTION GUIDE

### Method 1: Direct Execution on Windows (Pre-compiled Binaries)
No extra libraries or installations required:
1. **Execute Automated Unit Test Suite:**
   ```cmd
   .\tests.exe
   ```
   *Expected Output: All 31 test cases across 7 test suites pass with 100% success rate.*

2. **Launch Web Server & Interactive Dashboard:**
   ```cmd
   .\run_server.bat
   ```
   Or run `.\server.exe`. Open your browser and navigate to: **`http://localhost:8080`** to interact with all 8 wireframe screens!

### Method 2: Recompile from Source
* **On Windows (MinGW-w64 GCC):**
  ```cmd
  .\build.bat
  ```
* **On Linux / macOS:**
  ```bash
  chmod +x build.sh
  ./build.sh
  ```

### Method 3: Containerized Execution with Docker
* **Build and run Web Server Container:**
  ```bash
  docker compose -f docker/docker-compose.yml up -d --build
  ```
  Access the web application at `http://localhost:8080`.
* **Execute Test Suite inside Container:**
  ```bash
  docker compose -f docker/docker-compose.yml --profile test run --rm attendance-tests
  ```

### Method 4: Use it on a phone (same Wi-Fi)
The server binds to `0.0.0.0`, so any phone on the same network can open it.
```powershell
powershell -ExecutionPolicy Bypass -File scripts\phone-url.ps1
```
The script prints `http://<PC-IPv4>:8080` and opens Windows Firewall TCP 8080 (admin prompt).
Then on the phone: open that URL → browser menu → **Add to Home Screen**.
The app ships a `manifest.webmanifest`, SVG icon and service worker, so it installs and runs full-screen like a native app.
See **`PHONE.md`** for details and troubleshooting.

---

## 3. DEMO ACCOUNTS & CREDENTIALS

Pre-seeded authentication accounts in accordance with SRS specifications:
* **Student 1:** ID: `B22DCCN001` | Password: `password123` *(Check-in active, countdown timer, leave request)*
* **Student 2:** ID: `B22DCCN045` | Password: `password123`
* **Lecturer:** Code: `GV101` | Password: `password123` *(Live classroom radar, leave adjudication panel)*
* **System Administrator:** Username: `ADM001` | Password: `admin123` *(Universal auto-routing from either login tab into Admin Master Portal)*

---

## 4. SUMMARY OF DELIVERABLES

1. **Stage 3 (Implementation):**
   * Realization of all 24 functional use cases (UC01 - UC24), including the lecturer/admin-only **circular check-in zone picker**.
   * Clean Multi-tier Architecture enforcing clear separation of concerns.
   * Server-side spherical Haversine formula calculation ($R_{earth} = 6,371,000\text{ m}$).
   * Multi-layered anti-fraud defense: GPS accuracy filtering ($\le 20\text{ m}$), Mock Location detection, and Anti-proxy Device ID binding. Khi bấm điểm danh lần đầu, sinh viên phải xác nhận "Cho phép ghi nhớ thiết bị phần cứng"; trình duyệt sau đó giữ một mã thiết bị ẩn danh ổn định (1 tài khoản = 1 thiết bị). Dùng thiết bị đó cho tài khoản thứ hai sẽ bị chặn, hiển thị cảnh báo **CẢNH CÁO: Phát hiện điểm danh hộ** và ghi `PROXY_ATTEMPT` vào `data/audit.txt`.
   * Admin màn hình 8 §2 hiển thị thiết bị thật đang gắn với từng sinh viên; sinh viên gửi đơn xin mở khóa kèm ảnh chứng minh tại màn hình GPS, Admin duyệt ở §6 (`data/device_reset_requests.txt`).
2. **Stage 4 (Testing):**
   * 31 Google Test (gtest 1.17.0) unit and integration tests (**31 / 31 PASSED, 100% Success Rate**), run automatically during the Docker build.
   * Testing document `Testing_Document_Group01.xlsx` uses the teacher's **exact** template (sheet `Test Cases`, 5 columns: Test Case ID / Function / Test Steps / Input Data / Expected Result) with **100 test cases = 5 cases for each of 20 functions**.
3. **Packaging:**
   * Compressed deliverable `ProgAndTest_Group01.zip` ready for official submission.




