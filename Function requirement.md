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

### I.GLOSSARY OF TERMS

1. **GPS (Global Positioning System):** Satellite-based radio navigation system providing geolocation coordinates (Latitude and Longitude) to the user's client device.
2. **Geofence (Virtual Geographical Boundary):** A circular virtual perimeter established around a target classroom anchor point $(Lat_0, Long_0)$ with a defined radius $R$ (default: $30\text{m} - 50\text{m}$).
3. **Haversine Formula:** A mathematical spherical trigonometry equation calculating the great-circle arc distance between two geographical coordinate pairs across the Earth's curved surface.
4. **Device Fingerprint / Device ID:** A unique cryptographic hash generated from client hardware/browser attributes (Canvas, WebGL, AudioContext, Screen specs) used to bind one student account to one physical device per class session, preventing proxy attendance.
5. **NTP Time (Network Time Protocol):** Authoritative server-side synchronized clock used to validate all timestamps, completely preventing local clock tampering on mobile devices.
6. **Self-recorded via GPS:** Attendance status automatically verified and recorded through legitimate client GPS coordinate submission within the active geofence.
7. **Excused Absence:** Student absence validated through formal online submission of a leave request accompanied by verified medical/official documentation approved by the lecturer (awarded 2/10 attendance points).
8. **Unexcused Absence:** Student absence resulting from failure to check in without submitting an excuse request, or when the submitted request is rejected by the lecturer (awarded 0/10 attendance points).

---

## II. FUNCTIONAL REQUIREMENTS (NATURAL LANGUAGE SPECIFICATION)

### 1. System Context, Objectives, and Scope
In higher education institutions, traditional attendance methods—such as roll calls, paper signature rosters, or static printed QR codes—suffer from severe integrity vulnerabilities: students sign rosters on behalf of absent peers or transmit photographed QR codes across instant messaging apps (Zalo, Telegram, Messenger) for remote check-ins.

The **Web-Based GPS Student Attendance System** provides an automated, tamper-resistant solution:
* Enforces physical presence by verifying client real-time GPS coordinates against an active classroom Geofence boundary.
* Employs hardware-level device binding to ensure that one physical smartphone can only record attendance for a single student profile per session.
* Provides a transparent attendance scoring model (scale of 10 points), an integrated leave request system with digital evidence submission (medical slips, official notices), and real-time monitoring radars for instructors.

---

### 2. Core Business Rules (BR)

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

### 3. Detailed Subsystem Functionalities

#### 3.1. Authentication & Universal Role Dispatcher Subsystem
1. **Initial Dual-Role Portal on Landing Page:**
    * When opening the web application, the root URL immediately presents a unified login screen with two explicit role tabs: `[ Student ]` and `[ Lecturer ]`.
    * Students authenticate via Student ID (MSSV) and password.
    * Lecturers authenticate via Lecturer Code (MGV) and password.
2. **Universal Admin Auto-Routing (Smart Privileged Access):**
    * System Administrators (`Admin`) do **not** require a dedicated, separate login URL.
    * An administrator may enter their credentials into **either the Student tab or the Lecturer tab**.
    * Upon submission, the authentication service inspects the user account record in the database. When `role = 'ADMIN'` is verified, the system bypasses the active tab and automatically redirects the administrator to the **System Administrator Master Dashboard**.

---

#### 3.2. Student Subsystem
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

#### 3.3. Lecturer Subsystem
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

#### 3.4. Administrator (Admin) Subsystem
1. **Universal Access:** Log in via either Student or Lecturer portal tabs; the system automatically authenticates role privileges and routes to the Admin Master Dashboard.
2. **Classroom Directory & Master GPS Geofence Management:**
    * Create and maintain campus buildings, floor plans, and room inventories.
    * Register high-precision GPS coordinate anchors (Latitude & Longitude to 7 decimal digits) and baseline geofence radii.
3. **User Management & Device Unbinding (Reset Device ID):**
    * Manage student and faculty accounts.
    * Adjudicate device reset tickets when a student acquires a new phone, loses a device, or clears browser storage.
4. **System Security Audit Trail:**
    * Comprehensive logging of check-in events, coordinate anomalies, mock location triggers, and duplicate device attempts.
