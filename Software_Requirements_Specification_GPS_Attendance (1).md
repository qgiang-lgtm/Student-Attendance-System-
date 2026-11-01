# SOFTWARE REQUIREMENTS SPECIFICATION & SYSTEM DESIGN SPECIFICATION (SRS)
## WEB-BASED STUDENT ATTENDANCE SYSTEM UTILIZING GPS POSITIONING AND GEOFENCING

---

* **Project Title:** Web-Based Smart GPS Student Attendance System (Geofence-Enabled Attendance Verification)
* **Course:** Software Engineering (CS / SE Capstone Project)
* **Author / Implementation Unit:** Student Project Team (Group_01)
* **Specification Version:** 2.0 (Official English Release)
* **Date of Completion:** November 1, 2026
* **Official File Path:** [Software_Requirements_Specification_GPS_Attendance.md](file:///C:/CNPM1/Software_Requirements_Specification_GPS_Attendance.md)

---

### DOCUMENT REVISION HISTORY

| Date | Version | Description of Changes / Enhancements | Author |
| :--- | :---: | :--- | :--- |
| **Oct 20, 2026** | 0.1 | Initial requirements survey, established 7-section document skeleton per faculty guidelines. | Project Team |
| **Oct 25, 2026** | 0.5 | Completed detailed natural language functional and non-functional requirements. | Project Team |
| **Oct 28, 2026** | 0.8 | Constructed Data Flow Diagrams (Level 0, Level 1), Use Case Diagram, and Class Diagram. | Project Team |
| **Nov 01, 2026** | 2.0 | Full standardization of all 7 sections per instructor requirements: Dual-role login portal (Student/Lecturer tabs with universal Admin auto-routing), 15-minute early check-in window, 15-minute on-time grace period (10/10 pts), late threshold (5/10 pts), absence excuse submission & proof upload workflow (Excused: 2/10 pts, Unexcused: 0/10 pts), ERD, Class Diagram, and complete UI Wireframes. Translated entirely into professional academic English. | Project Team |

---

### GLOSSARY OF TERMS

1. **GPS (Global Positioning System):** Satellite-based radio navigation system providing geolocation coordinates (Latitude and Longitude) to the user's client device.
2. **Geofence (Virtual Geographical Boundary):** A circular virtual perimeter established around a target classroom anchor point $(Lat_0, Long_0)$ with a defined radius $R$ (default: $30\text{m} - 50\text{m}$).
3. **Haversine Formula:** A mathematical spherical trigonometry equation calculating the great-circle arc distance between two geographical coordinate pairs across the Earth's curved surface.
4. **Device Fingerprint / Device ID:** A unique cryptographic hash generated from client hardware/browser attributes (Canvas, WebGL, AudioContext, Screen specs) used to bind one student account to one physical device per class session, preventing proxy attendance.
5. **NTP Time (Network Time Protocol):** Authoritative server-side synchronized clock used to validate all timestamps, completely preventing local clock tampering on mobile devices.
6. **Self-recorded via GPS:** Attendance status automatically verified and recorded through legitimate client GPS coordinate submission within the active geofence.
7. **Excused Absence:** Student absence validated through formal online submission of a leave request accompanied by verified medical/official documentation approved by the lecturer (awarded 2/10 attendance points).
8. **Unexcused Absence:** Student absence resulting from failure to check in without submitting an excuse request, or when the submitted request is rejected by the lecturer (awarded 0/10 attendance points).

---

## 1. FUNCTIONAL REQUIREMENTS (NATURAL LANGUAGE SPECIFICATION)

### 1.1. System Context, Objectives, and Scope
In higher education institutions, traditional attendance methods—such as roll calls, paper signature rosters, or static printed QR codes—suffer from severe integrity vulnerabilities: students sign rosters on behalf of absent peers or transmit photographed QR codes across instant messaging apps (Zalo, Telegram, Messenger) for remote check-ins.

The **Web-Based GPS Student Attendance System** provides an automated, tamper-resistant solution:
* Enforces physical presence by verifying client real-time GPS coordinates against an active classroom Geofence boundary.
* Employs hardware-level device binding to ensure that one physical smartphone can only record attendance for a single student profile per session.
* Provides a transparent attendance scoring model (scale of 10 points), an integrated leave request system with digital evidence submission (medical slips, official notices), and real-time monitoring radars for instructors.

---

### 1.2. Core Business Rules (BR)

Every academic session has a scheduled start time denoted as $T_{start}$ (e.g., 07:00 AM). The automated grading and attendance categorization engine operates under the following business rules:

| Check-in Timestamp ($T_{checkin}$) | Geolocation Condition ($d$) | Recorded Status | Session Points | System Remarks |
| :--- | :---: | :---: | :---: | :--- |
| **15 minutes before start ($T_{start} - 15$) to 15 minutes after start ($T_{start} + 15$)** | $d \le R$ (Inside classroom geofence) | **Present (On-time)** | **10 / 10** | *Self-recorded via GPS (On-time)* |
| **16 minutes after start ($T_{start} + 16$) to 45 minutes after start ($T_{start} + 45$)** | $d \le R$ (Inside classroom geofence) | **Late** | **5 / 10** | *Self-recorded via GPS (Late: [X] mins)* |
| **After minute 45 or session end (no check-in)** | Any location / Not recorded | **Absent (Unexcused)** | **0 / 10** | *Unexcused absence (Auto-locked)* |
| **During active window but outside perimeter** | $d > R$ (Outside geofence) | **Check-in Rejected** | **0 / 10** | *Error: Outside classroom range ($d\text{m} > R\text{m}$)* |
| **Student submits leave request & Lecturer APPROVES** | Valid evidence attached | **Excused Absence** | **2 / 10** | *Excused absence (Approved: [Reason])* |
| **Student submits leave request & Lecturer REJECTS** | Invalid / Insufficient proof | **Unexcused Absence** | **0 / 10** | *Rejected absence (Declined: [Reason])* |

---

### 1.3. Detailed Subsystem Functionalities

#### A. Authentication & Universal Role Dispatcher Subsystem
1. **Initial Dual-Role Portal on Landing Page:**
   * When opening the web application, the root URL immediately presents a unified login screen with two explicit role tabs: `[ Student ]` and `[ Lecturer ]`.
   * Students authenticate via Student ID (MSSV) and password.
   * Lecturers authenticate via Lecturer Code (MGV) and password.
2. **Universal Admin Auto-Routing (Smart Privileged Access):**
   * System Administrators (`Admin`) do **not** require a dedicated, separate login URL.
   * An administrator may enter their credentials into **either the Student tab or the Lecturer tab**.
   * Upon submission, the authentication service inspects the user account record in the database. When `role = 'ADMIN'` is verified, the system bypasses the active tab and automatically redirects the administrator to the **System Administrator Master Dashboard**.

---

#### B. Student Subsystem
1. **Hardware Device Binding & Anti-Proxy Check-in:**
   * Upon client authentication via a mobile browser, the system extracts a cryptographic client signature (`Device_ID`) utilizing hardware fingerprinting.
   * During an ongoing class session, each student account is strictly bound to one physical device. If a device that has already checked in for Student A attempts to check in for Student B, the check-in is rejected and logged as a proxy attempt.
2. **Proactive Schedule Banner (Today's Schedule Hero Banner):**
   * Upon successful login, the student's dashboard immediately highlights the **Today's Schedule Banner** at the very top of the page without requiring manual menu navigation.
   * **15-Minute Early Trigger:** Exactly **15 minutes before class start time** ($T_{start} - 15$), the corresponding class block transitions into the active state: `🟢 GPS CHECK-IN ACTIVE`, displaying a dynamic countdown timer for the 10/10 on-time window and a prominent 1-tap button: `[ 📍 CHECK-IN VIA GPS NOW ]`.
   * Subsequent classes scheduled for later in the day remain in a scheduled, locked state displaying their respective opening times.
3. **One-Tap GPS Self Check-in Workflow:**
   * Clicking the check-in button triggers the HTML5 Geolocation API (`navigator.geolocation.getCurrentPosition` with `enableHighAccuracy: true`) to retrieve the current Latitude, Longitude, and Accuracy radius ($\le 20\text{m}$).
   * The client transmits coordinates and `Device_ID` over HTTPS.
   * The server queries authoritative NTP time, verifies that Mock Location / Fake GPS flags are absent, and computes the spherical distance $d$ to the classroom center point using the Haversine equation:
     * If $d \le R$ within the window $[T_{start}-15, T_{start}+15]$: Status recorded as **Present**, **10/10 points**.
     * If $d \le R$ within the window $[T_{start}+16, T_{start}+45]$: Status recorded as **Late**, **5/10 points**.
     * If $d > R$: Check-in rejected with clear visual distance feedback (e.g., *"You are 125m away from room B204; maximum allowed radius is 35m"*).
4. **Leave Request & Proof Submission Workflow:**
   * For sessions the student cannot attend (illness, bereavement, university extracurriculars): The student clicks `[ 📄 Submit Leave Request / Proof ]` on the corresponding class card.
   * Submission requirements:
     * Category selection (Medical Illness / Family Emergency / Official School Activity / Other).
     * Written explanation.
     * **File Attachment:** Direct upload of image files (JPG, PNG) or PDF documents (hospital admission slip, doctor's prescription, official summons).
   * Status is initialized to `PENDING` awaiting lecturer adjudication.
5. **Academic Progress & Attendance Summary (All Courses View):**
   * Mirroring institutional Learning Management System (LMS) structures:
     * Displays a consolidated semester overview table.
     * Columns: Course Code & Name, Attendance Action, Taken Sessions, Points Earned / Max Points, Attendance Percentage (%), and Cumulative Average Attendance.
6. **Detailed Session History (This Course View):**
   * Displays the full chronological session-by-session history for a selected course.
   * Columns: Session Date, Class Description, Status (Present - Green, Late - Yellow, Excused - Purple/Blue, Absent - Red), Points Earned (**10/10, 5/10, 2/10, 0/10**), and Detailed System Remarks (exact timestamp, approval note).

---

#### C. Lecturer Subsystem
1. **Course & Classroom Management:**
   * View allocated course classes and teaching schedules.
   * Configure classroom geofence parameters, including dynamic radius adjustments ($20\text{m} - 100\text{m}$) for large amphitheaters or lecture halls.
2. **Real-Time Geofence Radar Monitor (Live Attendance Radar):**
   * Interactive live radar interface mapping the physical classroom layout:
     * Central circle delineating the active Geofence boundary ($R$).
     * Green markers appearing in real time for on-time check-ins.
     * Yellow markers appearing for late check-ins.
     * Live metrics counters: Total enrolled, Present count, Late count, Excused count, Absent count, and real-time attendance rate.
3. **Leave Request Adjudication & Grading Panel:**
   * Review submitted student leave requests, complete with inline image/PDF document viewers for medical slips.
   * **Adjudication actions:**
     * Click **[ APPROVE ]**: Updates session status to **Excused Absence** and awards **2 / 10 points** to the student's cumulative grade.
     * Click **[ REJECT ]**: Retains session status as **Unexcused Absence** with **0 / 10 points**, prompting an optional feedback note to the student.
4. **Manual Override & Report Exporting:**
   * Capability to manually override attendance records in exceptional circumstances (e.g., student smartphone battery depletion verified in person).
   * One-click export of complete course attendance gradebooks to Microsoft Excel (.xlsx) or Adobe PDF (.pdf) matching institutional academic formats.

---

#### D. Administrator (Admin) Subsystem
1. **Universal Access:** Log in via either Student or Lecturer portal tabs; the system automatically authenticates role privileges and routes to the Admin Master Dashboard.
2. **Classroom Directory & Master GPS Geofence Management:**
   * Create and maintain campus buildings, floor plans, and room inventories.
   * Register high-precision GPS coordinate anchors (Latitude & Longitude to 7 decimal digits) and baseline geofence radii.
3. **User Management & Device Unbinding (Reset Device ID):**
   * Manage student and faculty accounts.
   * Adjudicate device reset tickets when a student acquires a new phone, loses a device, or clears browser storage.
4. **System Security Audit Trail:**
   * Comprehensive logging of check-in events, coordinate anomalies, mock location triggers, and duplicate device attempts.

---

## 2. NON-FUNCTIONAL REQUIREMENTS (NATURAL LANGUAGE SPECIFICATION)

### 2.1. Usability
* **Responsive Web Design:** Fully responsive mobile-first architecture optimized for smartphone browsers (Mobile Safari on iOS, Google Chrome on Android), tablets, and desktop workstations. Supports accessible contrast ratios, Light Mode, and Dark Mode consistent with modern institutional portals.
* **1-Tap Check-in Experience:** Streamlined user journey requiring zero unnecessary navigation; opening the homepage immediately surfaces the active countdown banner and a single action button to complete check-in within 3 seconds.
* **Visual Status Feedback:** Clear color-coded alerts (Green = Success 10/10; Yellow = Late 5/10; Red = Geofence rejection or absence 0/10; Purple = Pending leave review).

### 2.2. Reliability & Accuracy
* **Haversine Spherical Distance Calculation:**
  Distance $d$ between the student's reported coordinates $(lat_1, lon_1)$ and the classroom anchor $(lat_2, lon_2)$ is mathematically computed server-side:
  $$d = 2 \cdot R_{earth} \cdot \arcsin\left(\sqrt{\sin^2\left(\frac{\Delta \varphi}{2}\right) + \cos(\varphi_1)\cdot\cos(\varphi_2)\cdot\sin^2\left(\frac{\Delta \lambda}{2}\right)}\right)$$
  *(Where: $R_{earth} = 6,371,000\text{ m}$; $\Delta \varphi = \varphi_2 - \varphi_1$; $\Delta \lambda = \lambda_2 - \lambda_1$ in radians).*
* **Authoritative Server Time Synchronization:** All check-in windows, late cutoffs, and timestamps are validated exclusively against server-side Network Time Protocol (NTP) clocks, rendering client device clock tampering impossible.
* **System Availability:** Guarantees $\ge 99.8\%$ uptime during peak morning and afternoon class changeover periods.

### 2.3. Performance
* **High Concurrency Throughput:** The backend architecture must reliably absorb and process simultaneous check-in spikes of $\ge 5,000$ to $10,000$ concurrent student requests during the 15-minute start window without degraded response times or service interruptions.
* **Response Latency:** Server turnaround time from GPS coordinate transmission to visual confirmation must not exceed $1.5$ seconds under standard 4G/5G/Campus Wi-Fi connectivity.

### 2.4. Security & Anti-Spoofing
* **Anti-Mock Geolocation Detection:** Rigorous validation checking for browser mock location flags, impossible transit velocities between consecutive check-ins (speed anomaly detection), and verification against campus subnet IP pools.
* **Anti-Proxy Device Fingerprinting:** Unique browser hardware signatures bound to active user tokens, blocking multi-account check-ins originating from the same physical handset.
* **Data Transport Encryption:** Mandatory HTTPS transport with TLS 1.3 protocol. User passwords securely hashed using one-way salted algorithms (`bcrypt` or `Argon2`).
* **Role-Based Access Control (RBAC):** Cryptographically signed JSON Web Tokens (JWT) enforcing route protection across Student, Lecturer, and Admin tiers.

### 2.5. Design & Technology Constraints
* **Architecture:** Decoupled Client-Server RESTful API architecture following clean, multi-tier software engineering standards.
* **Frontend:** Modern Web Stack (HTML5, Tailwind CSS, TypeScript / React / Vue, Leaflet.js for interactive geofence radar visualization).
* **Backend:** Node.js (NestJS / Express) or Python (FastAPI) optimized for asynchronous non-blocking spatial queries.
* **Database:** Relational Database Management System (PostgreSQL with PostGIS spatial extensions, or MySQL 8.0 with Spatial Indexes).

---

## 3. DATA FLOW DIAGRAMS (DFD)

### 3.1. Context Diagram (Level 0 DFD)

```mermaid
flowchart TD
    subgraph EXTERNAL_ENTITIES["EXTERNAL ENTITIES"]
        Student["Student Entity"]
        Lecturer["Lecturer Entity"]
        Admin["Administrator Entity"]
    end

    subgraph SYSTEM_BOUNDARY["SYSTEM BOUNDARY"]
        CoreSystem(("0.0 WEB-BASED GPS STUDENT ATTENDANCE SYSTEM"))
    end

    %% Student Data Flows
    Student -->|"1. Credentials (ID, Pass, Device Fingerprint)"| CoreSystem
    Student -->|"2. Real-time GPS Coordinates (Lat, Long, Accuracy)"| CoreSystem
    Student -->|"3. Leave Request Form + Attached Medical Proof"| CoreSystem
    CoreSystem -->|"4. Active Session Status & 15-min Countdown Banner"| Student
    CoreSystem -->|"5. Check-in Result (10 pts On-time / 5 pts Late / Error)"| Student
    CoreSystem -->|"6. Academic Attendance History (All Courses / This Course)"| Student

    %% Lecturer Data Flows
    Lecturer -->|"7. Lecturer Credentials"| CoreSystem
    Lecturer -->|"8. Session Config (Geofence Radius R, Room Assignment)"| CoreSystem
    Lecturer -->|"9. Leave Adjudication Decision (Approve 2 pts / Reject 0 pts)"| CoreSystem
    Lecturer -->|"10. Manual Attendance Adjustments"| CoreSystem
    CoreSystem -->|"11. Live Classroom Radar Feed (Real-time student dots)"| CoreSystem
    CoreSystem -->|"12. Session Roster Statistics & Pending Leave Queue"| Lecturer
    CoreSystem -->|"13. Exported Attendance Gradebook (Excel / PDF)"| Lecturer

    %% Admin Data Flows
    Admin -->|"14. Admin Credentials (Submitted via Student or Lecturer tab)"| CoreSystem
    Admin -->|"15. Classroom Master Coordinates & Campus Directory"| CoreSystem
    Admin -->|"16. User Provisioning & Device Reset Commands"| CoreSystem
    CoreSystem -->|"17. System Administration Dashboard & Audit Logs"| Admin
```

---

### 3.2. Detailed Level 1 DFD

```mermaid
flowchart TD
    %% Entities
    Student["Student"]
    Lecturer["Lecturer"]
    Admin["Administrator"]

    %% Processes
    P1(("1.0 Authentication & Universal Role Dispatching"))
    P2(("2.0 Schedule & Session Management"))
    P3(("3.0 GPS Check-in & Anti-Spoofing Engine"))
    P4(("4.0 Leave Request & Proof Adjudication"))
    P5(("5.0 Attendance Scoring, Analytics & Reporting"))

    %% Data Stores
    D1[("D1: Users & Device Bindings")]
    D2[("D2: Classrooms & Master Geofences")]
    D3[("D3: Timetables & Class Sessions")]
    D4[("D4: Attendance Logs & Point Records")]
    D5[("D5: Leave Requests & Digital Evidence")]

    %% Process 1.0 Flows
    Student -->|"Student ID, Pass, Device Hash"| P1
    Lecturer -->|"Lecturer Code, Pass"| P1
    Admin -->|"Admin Credentials (via Student/Lecturer Tab)"| P1
    P1 <-->|"Validate Credentials & Device Lock"| D1
    P1 -->|"Issue Student Session Token"| Student
    P1 -->|"Issue Lecturer Session Token"| Lecturer
    P1 -->|"Universal Redirect to Admin Dashboard"| Admin

    %% Process 2.0 Flows
    Admin -->|"Register Classroom GPS Anchor Coordinates"| P2
    P2 <-->|"Write/Read Geofence Boundaries"| D2
    Lecturer -->|"Activate Class Session & Set Custom Radius R"| P2
    P2 <-->|"Manage Session State & Timeslots"| D3
    P2 -->|"Trigger 15-min Early Banner & Countdown"| Student
    P2 -->|"Push Session Parameters to Radar"| Lecturer

    %% Process 3.0 Flows
    Student -->|"Submit GPS Data: Lat, Long, Accuracy"| P3
    P3 <-->|"Fetch Classroom Coordinates & Radius R"| D2
    P3 <-->|"Validate Session Status & NTP Time"| D3
    P3 <-->|"Verify Device Fingerprint Binding"| D1
    P3 -->|"Commit Record: On-time 10 pts / Late 5 pts"| D4
    P3 -->|"Return Immediate Visual Confirmation"| Student
    P3 -->|"Push Real-time Dot Position to Live Radar"| Lecturer

    %% Process 4.0 Flows
    Student -->|"Submit Leave Request + Medical Proof File"| P4
    P4 -->|"Store Request (Status: PENDING)"| D5
    D5 -->|"Queue Pending Requests & Proof Files"| Lecturer
    Lecturer -->|"Adjudication: Approve (2 pts) / Reject (0 pts)"| P4
    P4 -->|"Update Request Status"| D5
    P4 -->|"Update Record: Excused (2 pts) / Unexcused (0 pts)"| D4
    P4 -->|"Transmit Adjudication Feedback Notice"| Student

    %% Process 5.0 Flows
    D4 -->|"Raw Attendance Point Records"| P5
    D3 -->|"Session Schedules & Totals"| P5
    P5 -->|"Render All Courses & This Course Summaries"| Student
    P5 -->|"Real-time Roster Attendance Percentages"| Lecturer
    Lecturer -->|"Request Attendance Export"| P5
    P5 -->|"Generate Structured Excel / PDF File"| Lecturer
    P5 -->|"Consolidated Institutional Compliance Reports"| Admin
```

---

## 4. USE CASE DIAGRAM & DETAILED SPECIFICATIONS

### 4.1. Comprehensive Use Case Diagram

```mermaid
flowchart LR
    %% Actors
    subgraph ACTORS["SYSTEM ACTORS"]
        Student(("Student"))
        Lecturer(("Lecturer"))
        Admin(("Administrator"))
    end

    %% Use Cases
    subgraph USE_CASES["USE CASES"]
        UC01["UC01: Authenticate via Dual-Role Portal\n(Student / Lecturer Tabs)"]
        UC02["UC02: Universal Admin Auto-Routing\n(Bypass Tab into Admin Portal)"]
        UC03["UC03: View Today's Schedule Banner\n(Active 15-min Prior)"]
        UC04["UC04: Perform 1-Tap GPS Check-in\n(On-time 10 pts / Late 5 pts)"]
        UC05["UC05: Submit Leave Request & Upload Proof"]
        UC06["UC06: View Semester Attendance Summary\n(All Courses View)"]
        UC07["UC07: View Detailed Course History\n(This Course View)"]
        UC08["UC08: Configure & Launch Attendance Session"]
        UC09["UC09: Monitor Real-Time Geofence Radar\n(Live Dot Positions)"]
        UC10["UC10: Adjudicate Leave Requests\n(Approve 2 pts / Reject 0 pts)"]
        UC11["UC11: Manual Attendance Override"]
        UC12["UC12: Export Attendance Reports (Excel/PDF)"]
        UC13["UC13: Manage Classrooms & Master GPS Coordinates"]
        UC14["UC14: User Management & Reset Device Binding"]
    end

    %% Student Relationships
    Student --> UC01
    Student --> UC03
    Student --> UC04
    Student --> UC05
    Student --> UC06
    Student --> UC07

    %% Lecturer Relationships
    Lecturer --> UC01
    Lecturer --> UC08
    Lecturer --> UC09
    Lecturer --> UC10
    Lecturer --> UC11
    Lecturer --> UC12

    %% Admin Relationships
    Admin --> UC01
    Admin --> UC02
    Admin --> UC13
    Admin --> UC14

    %% Includes
    UC01 -.->|"<<include>>"| UC02
    UC04 -.->|"<<include>>"| UC03
```

---

### 4.2. Detailed Use Case Specifications

#### 4.2.1. Use Case UC01: Universal Authentication & Smart Admin Auto-Routing
* **Use Case ID:** `UC01`
* **Primary Actors:** Student, Lecturer, Administrator (Admin).
* **Summary:** Authenticates users into the system via a unified login interface presenting dual role tabs. Administrators can submit credentials into either tab, and the system automatically routes them to the Admin Portal based on database role attributes.
* **Pre-conditions:** The user account has been provisioned in the database.
* **Post-conditions:** An encrypted JWT session token is issued, and the user is navigated to their role-specific dashboard.
* **Basic Execution Flow:**
  1. The user navigates to the application root URL.
  2. The system renders the login form displaying two tabs: `[ Student ]` and `[ Lecturer ]`.
  3. The user selects their respective tab (or an Admin selects either tab arbitrarily).
  4. The user inputs their identification code and password, then clicks `[ Log In to System ]`.
  5. The client computes the device fingerprint and transmits credentials to the authentication API over HTTPS.
  6. The authentication service validates the password against the stored bcrypt hash:
     * If `role == 'ADMIN'`: Bypasses the selected tab and redirects directly to the **Admin Master Portal**.
     * If `role == 'LECTURER'`: Routes to the **Lecturer Dashboard**.
     * If `role == 'STUDENT'`: Verifies device hardware binding, records active session, and routes to the **Student Today's Schedule Dashboard**.
* **Alternative Flows:**
  * *A1 - Invalid Credentials:* System displays an alert: *"Invalid account identifier or password. Please try again."*
  * *A2 - Duplicate Device Binding Conflict:* If the device is already locked to another student account in the ongoing session, the system denies entry: *"This hardware device is already associated with another student account for the current session. Please contact the administrator."*

---

#### 4.2.2. Use Case UC04: Perform 1-Tap GPS Attendance Check-in
* **Use Case ID:** `UC04`
* **Primary Actor:** Student.
* **Summary:** The student records attendance for the current class using their device's GPS hardware. The server calculates spherical distance and checks the timestamp against the 15-minute rules to award 10 points (on-time) or 5 points (late).
* **Pre-conditions:** The student is authenticated; the session is active (starting 15 minutes before scheduled class start).
* **Basic Execution Flow:**
  1. The student views the Today's Schedule Hero Banner displaying the blinking indicator: `🟢 GPS CHECK-IN ACTIVE`.
  2. The student taps `[ 📍 CHECK-IN VIA GPS NOW ]`.
  3. The browser requests location permissions; the student selects *"Allow"*.
  4. The browser queries the GPS sensor with high accuracy enabled to acquire $(Lat_{client}, Long_{client}, Accuracy)$.
  5. The client transmits coordinates and the hardware `Device_ID` to the server over HTTPS.
  6. The server validates authoritative NTP time and confirms that Mock Location flags are absent.
  7. The server calculates distance $d$ to the classroom center point using the Haversine equation.
  8. **Rule Verification & Point Awarding:**
     * If $d \le R$ (within classroom geofence):
       * If $T_{checkin}$ is within $[T_{start}-15\text{ mins}, T_{start}+15\text{ mins}]$: System records status **Present**, **Score: 10 / 10 Points**.
       * If $T_{checkin}$ is within $[T_{start}+16\text{ mins}, T_{start}+45\text{ mins}]$: System records status **Late**, **Score: 5 / 10 Points** (noting exact minutes late).
  9. The result is committed to `attendance_records` and immediately reflected on both the student screen and the lecturer live radar.
* **Exception Flows:**
  * *E1 - Outside Geofence Perimeter ($d > R$):* System rejects check-in: *"You are outside the classroom boundary (Measured distance: [d]m, Allowed radius: [R]m)."* Point balance remains 0/10.
  * *E2 - Location Permission Denied:* System presents instructions on enabling browser location services.
  * *E3 - Fake GPS / Mock Location Detected:* System aborts check-in and logs a security infraction.

---

#### 4.2.3. Use Case UC05: Submit Leave Request & Upload Digital Evidence
* **Use Case ID:** `UC05`
* **Primary Actor:** Student.
* **Summary:** A student unable to attend a class submits a formal digital leave request with supporting evidence (medical slips, hospital receipts, official convocations) to be evaluated for Excused Absence (2/10 points).
* **Pre-conditions:** Student is logged in and selects the designated class session.
* **Basic Execution Flow:**
  1. The student navigates to the session details or clicks `[ 📄 Submit Leave Request / Proof ]` on the class card.
  2. The system displays the Leave Application Modal.
  3. The student specifies the category (Illness, Family Emergency, University Activity, Other).
  4. The student types a comprehensive explanation.
  5. The student clicks `[ Select Proof File ]` to attach a photograph (JPG, PNG) or PDF document (medical certificate).
  6. The student clicks `[ Submit Request to Lecturer ]`.
  7. The system uploads the proof file to encrypted storage, creates an entry in `leave_requests` with status `PENDING`, and alerts the instructor.

---

#### 4.2.4. Use Case UC10: Adjudicate Leave Request & Award Points
* **Use Case ID:** `UC10`
* **Primary Actor:** Lecturer.
* **Summary:** The lecturer inspects the submitted explanation and digital medical/official proof to approve (Excused: 2/10 points) or reject (Unexcused: 0/10 points) the absence.
* **Basic Execution Flow:**
  1. The lecturer navigates to the `[ Leave Requests Queue ]` for their course class.
  2. The system lists all pending submissions with Student Code, Name, Date, Reason, and Proof Attachment.
  3. The lecturer clicks the attachment preview to review the medical slip and hospital stamp.
  4. **Adjudication Choice:**
     * **Approval:** The lecturer clicks `[ Approve as Excused ]`. The system updates the attendance log: Status = **Excused Absence**, **Score = 2 / 10 Points**.
     * **Rejection:** The lecturer clicks `[ Reject Request ]` and provides feedback (e.g., *"Proof lacks hospital seal"*). Status = **Unexcused Absence**, **Score = 0 / 10 Points**.
  5. The student receives an instant notification of the adjudication result.

---

## 5. CLASS DIAGRAM (UML STRUCTURAL SPECIFICATION)

```mermaid
classDiagram
    %% User Base Class and Inheritances
    class User {
        -int id
        -string username
        -string passwordHash
        -string fullName
        -string email
        -string role
        -datetime createdAt
        +login(username, password) bool
        +logout() void
        +changePassword(oldPass, newPass) bool
    }

    class Student {
        -string studentCode
        -string boundDeviceId
        -bool isDeviceLocked
        +registerDevice(deviceId) bool
        +checkInGPS(lat, long, accuracy) AttendanceResult
        +submitLeaveRequest(sessionId, reason, fileUrl) bool
        +viewSummaryAllCourses() List~CourseSummary~
        +viewCourseDetails(courseId) CourseHistory
    }

    class Lecturer {
        -string lecturerCode
        -string department
        +openAttendanceSession(sessionId, radius) bool
        +closeAttendanceSession(sessionId) bool
        +liveMonitorRadar(sessionId) RadarData
        +reviewLeaveRequest(requestId, isApproved, note) bool
        +adjustAttendanceManual(recordId, newStatus, points) bool
        +exportReport(classId, format) FileStream
    }

    class Admin {
        -string adminCode
        +manageClassroom(classroom) bool
        +resetDeviceBinding(studentId) bool
        +manageUserAccounts() void
        +viewSystemAuditLogs() List~LogEntry~
    }

    User <|-- Student
    User <|-- Lecturer
    User <|-- Admin

    %% Classroom and Course Classes
    class Classroom {
        -int id
        -string roomCode
        -string building
        -decimal latitude
        -decimal longitude
        -decimal defaultRadius
        +getCoordinates() GeoPoint
        +updateGeofence(lat, long, radius) void
    }

    class CourseClass {
        -int id
        -string classCode
        -string courseName
        -int lecturerId
        -string semester
        -string academicYear
        +getEnrolledStudents() List~Student~
        +calculateAverageAttendance() float
    }

    %% Session Class
    class AttendanceSession {
        -int id
        -int courseClassId
        -int classroomId
        -datetime startTime
        -datetime endTime
        -datetime earlyCheckinStart
        -datetime lateThreshold
        -decimal allowedRadius
        -string status
        +isEarlyWindowActive() bool
        +isOnTimeWindowActive() bool
        +isLateWindowActive() bool
        +closeSession() void
    }

    %% Record Class
    class AttendanceRecord {
        -int id
        -int sessionId
        -int studentId
        -datetime checkinTime
        -decimal clientLat
        -decimal clientLong
        -decimal distanceMeters
        -string deviceId
        -string status
        -int points
        -string remarks
        +calculatePoints() int
        +validateGeofence(centerLat, centerLong, radius) bool
    }

    %% Leave Request Class
    class LeaveRequest {
        -int id
        -int sessionId
        -int studentId
        -string reason
        -string evidenceFileUrl
        -datetime submittedAt
        -int reviewerId
        -string reviewStatus
        -string reviewNotes
        -datetime reviewedAt
        +approve(lecturerId, notes) void
        +reject(lecturerId, notes) void
    }

    %% Class Associations
    Lecturer "1" --> "0..*" CourseClass : Teaches
    CourseClass "1" --> "0..*" AttendanceSession : Schedules
    Classroom "1" --> "0..*" AttendanceSession : Hosts
    AttendanceSession "1" --> "0..*" AttendanceRecord : Contains
    Student "1" --> "0..*" AttendanceRecord : Generates
    AttendanceSession "1" --> "0..*" LeaveRequest : Receives
    Student "1" --> "0..*" LeaveRequest : Submits
    Lecturer "1" --> "0..*" LeaveRequest : Adjudicates
```

---

## 6. DATA MODEL (DATABASE SCHEMA / ERD & DATA DICTIONARY)

### 6.1. Entity Relationship Diagram (ERD)

```mermaid
erDiagram
    USERS ||--o| STUDENTS : "is-a"
    USERS ||--o| LECTURERS : "is-a"
    USERS ||--o| ADMINS : "is-a"

    STUDENTS ||--o{ DEVICE_BINDINGS : "possesses"
    STUDENTS ||--o{ COURSE_ENROLLMENTS : "enrolls"
    COURSE_CLASSES ||--o{ COURSE_ENROLLMENTS : "contains"
    LECTURERS ||--o{ COURSE_CLASSES : "teaches"
    
    COURSE_CLASSES ||--o{ ATTENDANCE_SESSIONS : "schedules"
    CLASSROOMS ||--o{ ATTENDANCE_SESSIONS : "located_at"

    ATTENDANCE_SESSIONS ||--o{ ATTENDANCE_RECORDS : "records"
    STUDENTS ||--o{ ATTENDANCE_RECORDS : "checks_in"

    ATTENDANCE_SESSIONS ||--o{ LEAVE_REQUESTS : "receives"
    STUDENTS ||--o{ LEAVE_REQUESTS : "submits"
    LECTURERS ||--o{ LEAVE_REQUESTS : "reviews"

    USERS {
        int id PK
        string username UK
        string password_hash
        string full_name
        string email UK
        enum role "STUDENT, LECTURER, ADMIN"
        datetime created_at
    }

    STUDENTS {
        int user_id PK, FK
        string student_code UK
        string current_device_id
        boolean is_device_locked
    }

    LECTURERS {
        int user_id PK, FK
        string lecturer_code UK
        string department
    }

    CLASSROOMS {
        int id PK
        string room_code UK
        string building
        decimal latitude
        decimal longitude
        decimal default_radius
    }

    COURSE_CLASSES {
        int id PK
        string class_code UK
        string course_name
        int lecturer_id FK
        string semester
        string academic_year
    }

    ATTENDANCE_SESSIONS {
        int id PK
        int course_class_id FK
        int classroom_id FK
        date session_date
        datetime start_time
        datetime end_time
        datetime early_checkin_start
        datetime late_threshold_time
        decimal allowed_radius
        enum status "PENDING, ACTIVE, CLOSED"
    }

    ATTENDANCE_RECORDS {
        int id PK
        int session_id FK
        int student_id FK
        datetime checkin_time
        decimal client_lat
        decimal client_long
        decimal distance_meters
        string device_id
        enum status "PRESENT, LATE, EXCUSED, ABSENT"
        int points "10, 5, 2, 0"
        string remarks
    }

    LEAVE_REQUESTS {
        int id PK
        int session_id FK
        int student_id FK
        string reason
        string evidence_file_url
        datetime submitted_at
        int reviewer_id FK
        enum review_status "PENDING, APPROVED, REJECTED"
        string review_notes
        datetime reviewed_at
    }
```

---

### 6.2. Detailed Data Dictionary

#### Table 1: `USERS` (Shared Authentication Accounts)
| Column Name | Data Type | Key | Constraints | Description |
| :--- | :--- | :---: | :--- | :--- |
| `id` | `INT` | PK | `AUTO_INCREMENT` | Unique user account identifier |
| `username` | `VARCHAR(50)` | UK | `NOT NULL, UNIQUE` | Login username (Student ID, Lecturer Code, or Admin ID) |
| `password_hash` | `VARCHAR(255)` | | `NOT NULL` | One-way cryptographic hash (`bcrypt`) |
| `full_name` | `VARCHAR(100)` | | `NOT NULL` | Full legal name of user |
| `email` | `VARCHAR(100)` | UK | `NOT NULL, UNIQUE` | Institutional email address |
| `role` | `ENUM` | | `NOT NULL` | System role: `'STUDENT'`, `'LECTURER'`, `'ADMIN'` |
| `created_at` | `DATETIME` | | `DEFAULT CURRENT_TIMESTAMP` | Account creation timestamp |

#### Table 2: `STUDENTS` (Student Profiles & Device Lock)
| Column Name | Data Type | Key | Constraints | Description |
| :--- | :--- | :---: | :--- | :--- |
| `user_id` | `INT` | PK, FK | `REFERENCES USERS(id)` | Foreign key referencing USERS |
| `student_code` | `VARCHAR(20)` | UK | `NOT NULL, UNIQUE` | University Student Identification Number (e.g., B22DCCN001) |
| `current_device_id` | `VARCHAR(128)` | | `NULL` | Cryptographic hardware device fingerprint hash |
| `is_device_locked` | `BOOLEAN` | | `DEFAULT FALSE` | Hardware binding lock state |

#### Table 3: `CLASSROOMS` (Classrooms & Master Geofences)
| Column Name | Data Type | Key | Constraints | Description |
| :--- | :--- | :---: | :--- | :--- |
| `id` | `INT` | PK | `AUTO_INCREMENT` | Unique classroom identifier |
| `room_code` | `VARCHAR(20)` | UK | `NOT NULL, UNIQUE` | Official room name/code (e.g., B204, A102) |
| `building` | `VARCHAR(50)` | | `NOT NULL` | Campus building or sector |
| `latitude` | `DECIMAL(10, 7)` | | `NOT NULL` | High-precision latitude of room center anchor |
| `longitude` | `DECIMAL(10, 7)` | | `NOT NULL` | High-precision longitude of room center anchor |
| `default_radius` | `DECIMAL(5, 2)` | | `DEFAULT 35.00` | Standard geofence scanning radius in meters (m) |

#### Table 4: `ATTENDANCE_SESSIONS` (Class Attendance Sessions)
| Column Name | Data Type | Key | Constraints | Description |
| :--- | :--- | :---: | :--- | :--- |
| `id` | `INT` | PK | `AUTO_INCREMENT` | Unique session identifier |
| `course_class_id` | `INT` | FK | `REFERENCES COURSE_CLASSES(id)` | Associated course class |
| `classroom_id` | `INT` | FK | `REFERENCES CLASSROOMS(id)` | Physical room location |
| `session_date` | `DATE` | | `NOT NULL` | Date of scheduled session |
| `start_time` | `DATETIME` | | `NOT NULL` | Scheduled nominal start time ($T_{start}$) |
| `end_time` | `DATETIME` | | `NOT NULL` | Scheduled session dismissal time |
| `early_checkin_start` | `DATETIME` | | `NOT NULL` | 15-minute early trigger timestamp ($T_{start} - 15\text{ mins}$) |
| `late_threshold_time` | `DATETIME` | | `NOT NULL` | Cut-off for on-time grading ($T_{start} + 15\text{ mins}$) |
| `allowed_radius` | `DECIMAL(5, 2)` | | `NOT NULL` | Operational geofence radius $R$ for session (m) |
| `status` | `ENUM` | | `DEFAULT 'PENDING'` | Session lifecycle: `'PENDING'`, `'ACTIVE'`, `'CLOSED'` |

#### Table 5: `ATTENDANCE_RECORDS` (Session Attendance Logs)
| Column Name | Data Type | Key | Constraints | Description |
| :--- | :--- | :---: | :--- | :--- |
| `id` | `INT` | PK | `AUTO_INCREMENT` | Unique attendance record identifier |
| `session_id` | `INT` | FK | `REFERENCES ATTENDANCE_SESSIONS(id)` | Target session |
| `student_id` | `INT` | FK | `REFERENCES STUDENTS(user_id)` | Student entity |
| `checkin_time` | `DATETIME` | | `NULL` | Exact authoritative NTP timestamp of check-in |
| `client_lat` | `DECIMAL(10, 7)` | | `NULL` | Submitted client GPS latitude |
| `client_long` | `DECIMAL(10, 7)` | | `NULL` | Submitted client GPS longitude |
| `distance_meters` | `DECIMAL(6, 2)` | | `NULL` | Computed Haversine distance to center anchor (m) |
| `device_id` | `VARCHAR(128)` | | `NOT NULL` | Client device identifier used during check-in |
| `status` | `ENUM` | | `NOT NULL` | Final status: `'PRESENT'`, `'LATE'`, `'EXCUSED'`, `'ABSENT'` |
| `points` | `INT` | | `NOT NULL` | Awarded attendance score: **10** (on-time), **5** (late), **2** (excused), **0** (absent) |
| `remarks` | `VARCHAR(255)` | | `NULL` | System audit remarks (timestamp, minutes late, excuse notes) |

#### Table 6: `LEAVE_REQUESTS` (Leave Applications & Digital Evidence)
| Column Name | Data Type | Key | Constraints | Description |
| :--- | :--- | :---: | :--- | :--- |
| `id` | `INT` | PK | `AUTO_INCREMENT` | Unique leave application identifier |
| `session_id` | `INT` | FK | `REFERENCES ATTENDANCE_SESSIONS(id)` | Session requested for excuse |
| `student_id` | `INT` | FK | `REFERENCES STUDENTS(user_id)` | Applicant student |
| `reason` | `TEXT` | | `NOT NULL` | Detailed written explanation |
| `evidence_file_url` | `VARCHAR(255)` | | `NOT NULL` | Storage URL to uploaded proof document (JPG/PNG/PDF) |
| `submitted_at` | `DATETIME` | | `DEFAULT CURRENT_TIMESTAMP` | Application submission timestamp |
| `reviewer_id` | `INT` | FK | `NULL, REFERENCES LECTURERS(user_id)` | Lecturer who evaluated request |
| `review_status` | `ENUM` | | `DEFAULT 'PENDING'` | State: `'PENDING'`, `'APPROVED'`, `'REJECTED'` |
| `review_notes` | `TEXT` | | `NULL` | Lecturer feedback message to student |
| `reviewed_at` | `DATETIME` | | `NULL` | Adjudication decision timestamp |

---

## 7. USER INTERFACE (UI/UX) DESIGN SPECIFICATIONS & WIREFRAMES

### 7.1. Screen 1: Smart Dual-Role Authentication Screen (Universal Login)
* **Functional Logic:**
  * Renders two primary role selector tabs: `[ 👨‍🎓 Student ]` and `[ 👨‍🏫 Lecturer ]`.
  * **Admin Universal Capability:** Administrators do not need a special URL; entering admin credentials into either tab automatically triggers role validation and redirects to the Admin Portal.
  * Displays the client's computed hardware device fingerprint at the bottom of the card for transparency.

```
+-------------------------------------------------------------------------------+
|                      SMART GPS STUDENT ATTENDANCE SYSTEM                      |
|                           UNIVERSITY OF TECHNOLOGY                            |
+-------------------------------------------------------------------------------+
|                                                                               |
|                   +---------------------------------------+                   |
|                   |            AUTHENTICATION             |                   |
|                   +-------------------+-------------------+                   |
|                   |  [x] STUDENT      |   [ ] LECTURER    |                   |
|                   +-------------------+-------------------+                   |
|                   |                                       |                   |
|                   |  User Identifier (Student ID / Code): |                   |
|                   |  [ B22DCCN001                       ] |                   |
|                   |                                       |                   |
|                   |  Password:                            |                   |
|                   |  [ **********                       ] |                   |
|                   |                                       |                   |
|                   |  (*) Notice: System Administrators    |                   |
|                   |  may log in via either tab!           |                   |
|                   |                                       |                   |
|                   |  +---------------------------------+  |                   |
|                   |  |      LOG IN TO SYSTEM ->        |  |                   |
|                   |  +---------------------------------+  |                   |
|                   |                                       |                   |
|                   |  📱 Client Device ID: DEV-IP15-9921                       |
|                   +---------------------------------------+                   |
|                                                                               |
+-------------------------------------------------------------------------------+
```

---

### 7.2. Screen 2: Today's Schedule & 15-Minute Early Trigger Hero Banner
* **Functional Logic:**
  * Appears immediately at the top of the Student Dashboard upon login.
  * Exactly 15 minutes before scheduled start ($T_{start} - 15$), the card transitions to active state (`🟢 GPS CHECK-IN ACTIVE`), showing a live countdown timer for the 10/10 on-time window and the prominent 1-tap check-in button.

```
+-------------------------------------------------------------------------------+
| [LOGO] SmartAttendance | Dashboard | All Courses | Leave Requests | [B22DCCN001] |
+-------------------------------------------------------------------------------+
|                                                                               |
|  🔔 TODAY'S SCHEDULE & ATTENDANCE (Tuesday, Nov 01, 2026 - Server Time: 06:50)  |
|  ===========================================================================  |
|                                                                               |
|  +-------------------------------------------------------------------------+  |
|  | [SESSION 1: GPS CHECK-IN NOW ACTIVE] (Starts 07:00 AM - Room B204)       |  |
|  | Course: [CS301] - Software Engineering (Section 01)                     |  |
|  | Instructor: Dr. Alexander Vance | Allowed Radius: 35 meters             |  |
|  |                                                                         |  |
|  | ⏰ RULE: Check in within 15' before/after start for FULL 10/10 POINTS   |  |
|  | ⏳ Time remaining for 10/10 On-time score: 24 mins 45 secs             |  |
|  |                                                                         |  |
|  |  +-------------------------------------------------------------------+  |  |
|  |  |           [ 📍 CHECK-IN VIA GPS NOW (1-TAP VERIFICATION) ]        |  |  |
|  |  +-------------------------------------------------------------------+  |  |
|  |                                                                         |  |
|  |  [ Or: 📄 Submit Leave Request & Proof if unable to attend in person ]  |  |
|  +-------------------------------------------------------------------------+  |
|                                                                               |
|  +-------------------------------------------------------------------------+  |
|  | [SESSION 2: UPCOMING LATER TODAY] (09:30 AM - 11:50 AM - Room A301)     |  |
|  | Course: [CS302] - Computer Networks | Instructor: Dr. Robert Lang       |  |
|  | Status: 🔒 Locked (Opens at 09:15 AM - exactly 15 minutes prior)        |  |
|  +-------------------------------------------------------------------------+  |
+-------------------------------------------------------------------------------+
```

---

### 7.3. Screen 3: GPS Geofence Verification & Result Confirmation Screen
* **Functional Logic:**
  * Opens the radar scanner measuring actual client GPS coordinates against room coordinates.
  * Automatically calculates distance. If $\le 35\text{m}$, registers a green tick mark and grants 10/10 points (on-time) or 5/10 points (if past 15 minutes).

```
+-------------------------------------------------------------------------------+
|  <-- Back to Dashboard              GPS ATTENDANCE SCANNER                    |
+-------------------------------------------------------------------------------+
|  Course: [CS301] Software Engineering | Room: B204 (2nd Floor, Bldg B)        |
|  Authoritative NTP Time: 06:52:10 AM (8 minutes prior to nominal start)       |
|                                                                               |
|                         [ GEOFENCE RADAR SCANNER ]                            |
|                                                                               |
|                                  ( • )                                        |
|                           Room B204 Center Anchor                             |
|                                  / \                                          |
|                                 /   \  Measured Distance: 12.5 meters         |
|                                /     \ (Allowed Radius: 35m)                  |
|                               ( 📍 )                                          |
|                         Your Verified Position                                |
|                                                                               |
|  [✓] GPS Signal: Strong (Estimated Accuracy: ±4 meters)                       |
|  [✓] Device Integrity: Verified (iPhone 15 - ID: DEV-IP15-9921)               |
|  [✓] Anti-Spoofing: No Mock Location / Fake GPS detected                     |
|                                                                               |
|  +-------------------------------------------------------------------------+  |
|  |                   ATTENDANCE CONFIRMATION SUCCESS                       |  |
|  |  Recorded Status: PRESENT (ON-TIME)                                     |  |
|  |  Attendance Points Awarded: 10 / 10 POINTS                              |  |
|  |  Remarks: Self-recorded via GPS (Timestamp: 06:52:10 AM)                |  |
|  +-------------------------------------------------------------------------+  |
+-------------------------------------------------------------------------------+
```

---

### 7.4. Screen 4: Leave Request & Medical Evidence Upload Modal
* **Functional Logic:**
  * Allows students to select an absence category, enter details, and upload evidence images/PDFs.
  * Outlines clear grading policies: Lecturer Approval $\rightarrow$ **Excused (2/10 pts)**; Rejection or No Submission $\rightarrow$ **Unexcused (0/10 pts)**.

```
+-------------------------------------------------------------------------------+
|  <-- Back to Dashboard      SUBMIT LEAVE APPLICATION & PROOF                  |
+-------------------------------------------------------------------------------+
|  Session: Session 1 - Tuesday (Nov 01, 2026) | Course: Software Engineering   |
|                                                                               |
|  Absence Scoring Policy:                                                      |
|  * LECTURER APPROVES PROOF: Recorded as EXCUSED ABSENCE -> 2 / 10 POINTS      |
|  * LECTURER REJECTS / NO SUBMISSION: Recorded as UNEXCUSED   -> 0 / 10 POINTS  |
|                                                                               |
|  1. Primary Reason for Absence: (*)                                           |
|  [ Medical Illness / Hospitalization                                       v ]|
|                                                                               |
|  2. Detailed Explanation:                                                     |
|  +-------------------------------------------------------------------------+  |
|  | High fever and respiratory infection since last night. Currently at the |  |
|  | University Medical Center for examination. Prescription attached below. |  |
|  +-------------------------------------------------------------------------+  |
|                                                                               |
|  3. Attach Digital Evidence (Doctor's Note, Hospital Slip, Medical Proof):   |
|  +-------------------------------------------------------------------------+  |
|  |  [ 📁 Choose File: medical_certificate_nov01.jpg ] (Size: 1.8 MB)       |  |
|  |  [Preview]: [HOSPITAL DOCTOR NOTE WITH OFFICIAL STAMP AND SIGNATURE]     |  |
|  +-------------------------------------------------------------------------+  |
|                                                                               |
|  +-------------------------------------------------------------------------+  |
|  |              [ TRANSMIT LEAVE APPLICATION TO INSTRUCTOR ]               |  |
|  +-------------------------------------------------------------------------+  |
+-------------------------------------------------------------------------------+
```

---

### 7.5. Screen 5: Academic Attendance Summaries (All Courses & This Course Views)

#### Consolidated Semester Overview (All Courses View)
```
+-------------------------------------------------------------------------------+
| [ This Course ]  |  [[ ALL COURSES ]]  |  [ All Sessions ]                    |
+-------------------------------------------------------------------------------+
| SEMESTER ATTENDANCE & ACADEMIC INTEGRITY OVERVIEW                             |
|                                                                               |
| Course Identifier          | Attendance | Taken Sessions | Points | Percentage|
|----------------------------+------------+----------------+--------+-----------|
| [CS301] Software Engineer. | Check-in   |       5        | 42/50  |   84.0%   |
| [CS302] Computer Networks  | Check-in   |       6        | 55/60  |   91.6%   |
| [CS303] Database Systems   | Check-in   |       8        | 80/80  |  100.0%   |
| [CS304] Algorithms Design  | Check-in   |       7        | 57/70  |   81.4%   |
|----------------------------+------------+----------------+--------+-----------|
| Cumulative Semester Average Attendance Score: 89.2%                           |
+-------------------------------------------------------------------------------+
```

#### Detailed Course History View (This Course View)
```
+-------------------------------------------------------------------------------+
| [[ THIS COURSE ]]  |  [ All Courses ]  |  [ All Sessions ]                    |
| Course: [CS301] - Software Engineering | Instructor: Dr. Alexander Vance      |
+-------------------------------------------------------------------------------+
|                                                                               |
| Date               | Session / Topic | Status (Color Code) | Points | Remarks |
|--------------------+-----------------+---------------------+--------+---------|
| Tue 04 Oct 2026    | W01: Intro/SRS  | Present (Green)     | 10/10  | 06:55 AM|
| Tue 11 Oct 2026    | W02: DFD & UML  | Late (Yellow: 18m)  | 5/10   | 07:18 AM|
| Tue 18 Oct 2026    | W03: Lab 01     | Present (Green)     | 10/10  | 06:50 AM|
| Tue 25 Oct 2026    | W04: ERD Design | Excused (Purple)    | 2/10   | Med Doc |
| Tue 01 Nov 2026    | W05: UI Mockups | Absent (Unexcused)  | 0/10   | Pending |
|--------------------+-----------------+---------------------+--------+---------|
| Total Sessions Completed: 4 sessions                                          |
| Cumulative Attendance Points: 27 / 40 Points (Percentage: 67.5%)              |
+-------------------------------------------------------------------------------+
```

---

### 7.6. Screen 6: Lecturer Live Geofence Radar Monitoring Dashboard
* **Functional Logic:**
  * Interactive live canvas depicting the classroom perimeter.
  * Real-time markers appear dynamically: Green dots (on-time), Yellow dots (late).
  * Instant headcount metrics update automatically via WebSockets.

```
+-------------------------------------------------------------------------------+
| INSTRUCTOR: Dr. Alexander Vance | Course: Software Engineering (CS301-G01)    |
+-------------------------------------------------------------------------------+
| ACTIVE SESSION: Room B204 (07:00 AM - 09:20 AM)                               |
| Window Opened: 06:45 AM (15' early) | Late Mark: 07:15 AM | Geofence R: 35m   |
+-------------------------------------------------------------------------------+
|                                                                               |
|  [ LIVE CLASSROOM RADAR DISPLAY ]          [ REAL-TIME SESSION STATISTICS ]   |
|  +-----------------------------+          Class Enrollment: 60 Students       |
|  |         .  🟢 🟢            |          🟢 Present On-time (10 pts): 48     |
|  |      🟢   (ANCHOR)  🟢      |          🟡 Late Check-in (5 pts)   : 4      |
|  |        🟡    🟢             |          🟣 Excused Absence (2 pts) : 2      |
|  |     [ Boundary R=35m ]      |          🔴 Unrecorded Absent (0 pts): 6     |
|  |                             |                                              |
|  | (Out of bounds: 🔴 1 SV)    |          Real-time Attendance Rate: 88.3%    |
|  +-----------------------------+                                              |
|                                                                               |
|  LATEST REAL-TIME CHECK-IN LOGS:                                              |
|  - 06:58:12 AM: B22DCCN001 - Alexander Tran -> Present (10 pts) [Dist: 12m]   |
|  - 07:02:45 AM: B22DCCN045 - Maria Nguyen   -> Present (10 pts) [Dist: 8m]    |
|  - 07:18:20 AM: B22DCCN089 - David Pham     -> Late (5 pts)    [Dist: 15m]   |
|                                                                               |
|  [ 📄 Review Leave Applications (2 pending) ]   [ 📥 Export Gradebook Excel ] |
+-------------------------------------------------------------------------------+
```

---

### 7.7. Screen 7: Lecturer Leave Application Review & Adjudication Panel
* **Functional Logic:**
  * Allows the instructor to inspect the student's reason and attached proof image.
  * Clicking `[ Approve ]` marks status as **Excused** and awards **2/10 points**.
  * Clicking `[ Reject ]` marks status as **Unexcused** and keeps **0/10 points**.

```
+-------------------------------------------------------------------------------+
| LEAVE APPLICATION REVIEW - SOFTWARE ENGINEERING (CS301-G01)                   |
+-------------------------------------------------------------------------------+
| Ticket #108: Student David Lee (ID: B22DCCN120)                               |
| Session Date: Session 1 - Tuesday (Nov 01, 2026)                              |
| Claimed Reason: Acute dengue fever; admitted to University Medical Center.     |
| Attached Document: [Click to view full preview: hospital_admission_slip.jpg]   |
|                                                                               |
|  +-----------------------------------+-------------------------------------+  |
|  |  [✓] APPROVE AS EXCUSED           |  [X] REJECT APPLICATION             |  |
|  |  (Recorded as EXCUSED -> 2/10 PTS)|  (Recorded as UNEXCUSED -> 0/10 PTS)|  |
|  +-----------------------------------+-------------------------------------+  |
|  Instructor Feedback: [Approved. Wishing you a speedy recovery, David.]       |
+-------------------------------------------------------------------------------+
```

---

### 7.8. Screen 8: System Administrator (Admin) Master Control Dashboard
* **Functional Logic:**
  * Accessible by entering Admin credentials in either login tab.
  * Centralizes Master Classroom Geofence setup, hardware unbinding (Device ID reset), user role management, and security audit logs.

```
+-------------------------------------------------------------------------------+
| [ADMIN MASTER PORTAL] INSTITUTIONAL SMART GPS ATTENDANCE SYSTEM               |
+-------------------------------------------------------------------------------+
| 1. Master Classroom Directory & Geofence Coordinates:                         |
|    - Room B204 (Bldg B): Lat = 10.823098, Long = 106.629663, Radius = 35m     |
|    - Room A301 (Bldg A): Lat = 10.824150, Long = 106.630120, Radius = 40m     |
|    [ + Add New Classroom ]  [ ✏️ Calibrate GPS Anchor Coordinates ]            |
|                                                                               |
| 2. Device Hardware Binding Management (Reset Device ID):                      |
|    - Student: B22DCCN001 | Bound Device: DEV-IP15-9921 | [ 🔄 Reset Lock ]    |
|    - Student: B22DCCN015 | Bound Device: DEV-SS24-1102 | [ 🔄 Reset Lock ]    |
|                                                                               |
| 3. Security Audit Logs & Anti-Fraud Infractions:                              |
|    - 06:55 AM: Student B22DCCN088 flagged for Mock Location (Fake GPS) -> Banned|
|    - 07:01 AM: Device DEV-IP15-9921 attempted 2nd check-in for B22DCCN099 ->   |
|                Blocked by Anti-Proxy 1-Device Rule!                           |
+-------------------------------------------------------------------------------+
```

---

## CONCLUSION
This specification document completely satisfies all academic criteria set forth by course instructors:
1. **Functional Requirements:** Fully articulates the dual-role portal with universal Admin routing, 15-minute early trigger, 15-minute on-time grace period (10 pts), late threshold (5 pts), absence leave review (Excused: 2 pts, Unexcused: 0 pts), and multi-tier anti-fraud algorithms.
2. **Non-Functional Requirements:** Details Haversine mathematics, NTP synchronization, $\ge 5,000$ concurrent user handling, sub-1.5s latency, and security protocols.
3. **Data Flow Diagrams:** Provides formal Level 0 Context and Level 1 decomposed diagrams with stores $D_1 - D_5$.
4. **Use Case Modeling:** Provides UML Use Case diagramming and 4 rigorous use case specifications.
5. **Class Diagram:** Specifies domain models, associations, and methods.
6. **Data Model / ERD:** Full entity-relationship schema and exhaustive data dictionaries for 6 relational tables.
7. **UI/UX Design:** Complete wireframe schematics for all 8 application screens.
