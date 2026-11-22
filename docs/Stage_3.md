# STAGE 3 – IMPLEMENTATION SPECIFICATION REPORT
## WEB-BASED SMART GPS STUDENT ATTENDANCE SYSTEM
### Capstone Project: Software Engineering (Group_01)

* **Course Module:** Online Module 5 (Implementation & Testing)
* **Technology Stack:** C++17 (Core Backend & REST API), HTML5 / Tailwind CSS / Vanilla JS (Frontend Client), Docker.
* **Architecture Pattern:** Clean Multi-tier Layered Architecture (Domain Models, Services, Repositories, Presentation/API).

---

## 1. ARCHITECTURAL OVERVIEW

The Smart GPS Attendance System is implemented using a decoupled **Clean Layered Architecture** with high modularity and strict boundary enforcement:

```text
[ Presentation Layer: Winsock2 / POSIX HTTP Server & ApiRouter ]
                          │
[ Business Domain Services: GeoService | AttendanceGradingService | AntiFraudService | AuthService ]
                          │
[ Data Access Layer: Thread-Safe In-Memory DatabaseRepository (std::mutex) ]
                          │
[ Entity Domain Layer: 10 Normalized 3NF Models (Models.hpp) ]
```

### 1.1. Technology Decisions
1. **Programming Language:** Standard C++17 chosen for high throughput, predictable memory footprint, and native spatial calculation speed (sub-1.5ms latency).
2. **Zero Bloat Dependencies:** Standard library sockets (`winsock2.h` on Windows, POSIX `<sys/socket.h>` on Linux/Docker) without heavy third-party framework overhead.
3. **Database Layer:** Thread-safe in-memory 3NF relational data store utilizing `std::mutex` and `std::unordered_map` for O(1) primary key lookups, fully mimicking a PostGIS/MySQL spatial schema.

---

## 2. DETAILED SUBSYSTEM IMPLEMENTATION

### 2.1. Dual-Role Authentication & Smart Admin Auto-Routing (FR-01, FR-02)
* Implemented in `AuthService.hpp` and exposed via `POST /api/login`.
* Students log in with Student Code (MSSV) and password; Lecturers log in with Lecturer Code and password.
* **Smart Auto-Routing:** When an account has role `ADMIN`, the authentication dispatcher automatically overrides any client tab selection and returns a redirect token to `/admin-dashboard`.

### 2.2. Haversine Spherical Distance Calculation (NFR-04)
* Implemented in `GeoService.hpp`:
  $$\Delta \phi = \text{radians}(\text{lat}_2 - \text{lat}_1), \quad \Delta \lambda = \text{radians}(\text{lon}_2 - \text{lon}_1)$$
  $$a = \sin^2\left(\frac{\Delta \phi}{2}\right) + \cos(\phi_1)\cos(\phi_2)\sin^2\left(\frac{\Delta \lambda}{2}\right)$$
  $$d = 2 \cdot R_{\text{earth}} \cdot \arcsin\left(\sqrt{a}\right), \quad R_{\text{earth}} = 6,371,000\text{ m}$$
* Verified against classroom coordinate anchors (e.g., Room B204: `10.823098, 106.629663`).

### 2.3. Core Grading Engine (Section II.2, FR-04, FR-05)
* Implemented in `AttendanceGradingService.hpp`:
  * $[T_{\text{start}} - 15', T_{\text{start}} + 15']$ inside geofence ($d \le R$): **Present (On-time), 10/10 points**.
  * $[T_{\text{start}} + 16', T_{\text{start}} + 45']$ inside geofence ($d \le R$): **Late, 5/10 points**.
  * $> T_{\text{start}} + 45'$ or no check-in: **Absent (Unexcused), 0/10 points** (Auto-locked).
  * $d > R$ (Outside geofence): **Rejected, 0/10 points**.
  * Leave request approved: **Excused Absence, 2/10 points**.
  * Leave request declined: **Unexcused Absence, 0/10 points**.

### 2.4. Check-in Zone (Geofence) Selection — Lecturer / Admin only (FR-05, UC22 - UC24)
* Implemented in `DatabaseRepository::getGeofence()` / `updateSessionGeofence()`, exposed via:
  * `GET /api/session/geofence` — current circle centre (latitude, longitude) and radius.
  * `POST /api/lecturer/geofence` — moves the circle and/or resizes it.
* **RBAC:** the write endpoint accepts only `role = LECTURER` or `role = ADMIN` (or a login token `TOKEN_<ROLE>_<id>`). Any other role receives **HTTP 403**.
* Validation: latitude $\in [-90, 90]$, longitude $\in [-180, 180]$, radius $\in [5, 500]\text{ m}$.
* Every change is appended to the security audit trail as a `RADIUS_CHANGE` event.
* The UI exposes the editor as a modal with an interactive circular picker (tap/drag to move the centre, slider for the radius), plus numeric latitude/longitude inputs and a "Use my GPS" button. It is available from Screen 6 (lecturer) and Screen 8 (admin).
* Check-in enforcement is unchanged in the grading engine: a student is `PRESENT`/`LATE` only when $d \le R$; otherwise the attempt is `REJECTED (0/10)` with the remark *"Outside classroom range"*.

### 2.5. Anti-Fraud & Anti-Proxy Security (FR-03, NFR-09, NFR-10)
* Implemented in `AntiFraudService.hpp`:
  * **Accuracy Filter:** Requires GPS accuracy radius $\le 20\text{ m}$; coordinates with poor accuracy are rejected with `ERR_INSUFFICIENT_GPS_ACCURACY`.
  * **Mock Location Detection:** Detects client developer mock flags or virtual GPS injectors.
  * **Anti-Proxy Device Binding:** Binds each student account to one hardware `Device_ID`. If a device that checked in for Student A attempts to check in for Student B in the same session, it is immediately blocked and flagged in the security audit trail.

### 2.6. Interactive 8-Screen Frontend Client (Section VIII)
* Implemented in `WebClient.hpp` using HTML5, Tailwind CSS, and Vanilla JavaScript.
* Seamlessly renders all 8 UI wireframes specified in the SRS:
  * Screen 1: Dual-Role Login Screen
  * Screen 2: Today's Schedule & 15-Minute Early Trigger Banner
  * Screen 3: GPS Geofence Radar Scanner & Attendance Confirmation
  * Screen 4: Leave Application & Proof Upload Modal
  * Screen 5: Academic Attendance Summary (All Courses & Course History)
  * Screen 6: Lecturer Live Geofence Radar Dashboard (with check-in zone editor)
  * Screen 7: Lecturer Leave Review & Adjudication Panel
  * Screen 8: Administrator Master Control Dashboard
