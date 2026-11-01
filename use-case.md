## V. USE CASE DIAGRAM & DETAILED SPECIFICATIONS

### 2. Detailed Use Case Specifications

#### 2.1. Use Case UC01: Universal Authentication & Smart Admin Auto-Routing
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

#### 2.2. Use Case UC04: Perform 1-Tap GPS Attendance Check-in
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

#### 2.3. Use Case UC05: Submit Leave Request & Upload Digital Evidence
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

#### 2.4. Use Case UC10: Adjudicate Leave Request & Award Points
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
#### 2.3. Use Case UC05: Submit Leave Request & Upload Digital Evidence
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

#### 2.4. Use Case UC10: Adjudicate Leave Request & Award Points
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