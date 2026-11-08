# Supplementary Use Case Specifications — UC02, UC03, UC06–UC09, UC11–UC21

**Project:** Group 01 — Student Attendance Management System (GPS-Based Student Attendance Web Application)
**Source of truth:** `RequirementAndDesignDocument_v3.docx` (SRS v2.0; uploaded 8 November 2026).
**Purpose:** Complete the **17 use cases listed but not fully specified** in Section 4 of that document, without editing the document or redefining UC01, UC04, UC05, or UC10.

> **Document status:** *Proposed supplement for team review*, **not** an approved university policy or evidence of working implementation. The actor names, UC numbering, features, and relevant FR references are drawn from the supplied v3 document. Where v3 is silent, this supplement describes a plausible operational flow; such choices are marked **[Proposed]**. Do not present them as settled requirements until approved by Group 01.
>
> **Relation to diagrams:** UC20 (*Validate GPS location*) and UC21 (*Verify device binding*) are mandatory internal behaviours of UC04 through UML `«include»` relationships; students initiate UC04, not those checks as standalone user actions.

## UC02 — Sign out

- **Primary actor:** Student, Lecturer, or Administrator (System User).
- **Related requirement:** Authentication subsystem (FR-01); v3 does **not explicitly state a sign-out FR**, so the traceability link needs team confirmation.
- **Goal / trigger:** The authenticated user selects **Sign out**.
- **Preconditions:** The actor has an authenticated application session.
- **Success postconditions:** The current application session is invalidated and the user is returned to the login screen. No attendance or academic records are modified.

**Main success flow**
1. The actor chooses **Sign out** from the application menu.
2. The application submits a sign-out request with the current authentication context.
3. The server invalidates the current session or token according to the selected authentication implementation **[Proposed: server-side invalidation strategy]**.
4. The client removes local authentication state, clears protected views, and redirects to the dual-tab sign-in screen.
5. A subsequent protected action requires authentication again.

**Alternative / exception flows**
- A1. The session has already expired: the application removes local state and displays the login page without an error.
- E1. The request cannot reach the server: the client clears its local state; whether server-side invalidation is guaranteed depends on the token design **[Proposed; requires security decision]**.

**Special requirements:** No logout operation may delete attendance results. Protect against browser back navigation reopening confidential content after sign-out **[Proposed]**.

## UC03 — View today's schedule

- **Primary actor:** Student.
- **Related requirement:** FR-04; Screen 2 (*Today's Schedule*).
- **Goal / trigger:** The student opens the dashboard after successful sign-in.
- **Preconditions:** The student is authenticated.
- **Success postconditions:** Today’s scheduled classes, locations, check-in opening times and status indicators are presented; no attendance record is created.

**Main success flow**
1. The student visits the dashboard.
2. The system identifies the student’s enrolled class sections and today’s sessions.
3. For each session, it displays the course, section, scheduled start, room, and relevant lecturer information, where available in stored data.
4. It presents the 15-minute-early check-in rule: an eligible session may accept check-ins from `T_start − 15 minutes`, **provided the lecturer has activated the session**.
5. It shows a countdown for the on-time window and an action to start UC04 when check-in is available.
6. It shows upcoming sessions as scheduled/locked and completed sessions with their current attendance outcome.

**Alternative / exception flows**
- A1. No class today: show an explicit empty schedule state.
- A2. A session is within the time window but has not been activated: show *Not open* rather than allowing UC04 **[Proposed resolution of the FR-04/FR-09 interaction]**.
- E1. Schedule data cannot be loaded: display a retry action and do not display a working check-in button for unverified sessions.

**Special requirements:** Displayed countdown is informational; server time determines the result (NFR-05). Device local time cannot authorize a check-in.

## UC06 — View attendance summary

- **Primary actor:** Student.
- **Related requirement:** FR-07; Screen 5 (*All Courses*).
- **Goal / trigger:** The student selects **Attendance Summary / All Courses**.
- **Preconditions:** Student is authenticated; course membership and attendance data are available.
- **Success postconditions:** The student sees the semester attendance summary for their own courses. No scores are modified.

**Main success flow**
1. The student opens **All Courses**.
2. The system retrieves the classes in the selected semester for that student.
3. For each class, it obtains eligible completed sessions and the student's recorded attendance outcomes.
4. It displays course code/name, sessions taken, points earned/possible, and attendance percentage.
5. It shows the cumulative semester indicator and permits navigation to UC07.

**Alternative / exception flows**
- A1. The student has no applicable classes or completed sessions: show a zero-data state rather than divide by zero.
- A2. A leave request is awaiting a decision: show that review is pending alongside the current attendance result **[Proposed]**.
- E1. The results database is temporarily unavailable: show a retry/error state without fabricating percentages.

**Special requirements:** **[Proposed]** Calculate percentages from recorded results; v3 includes examples but does not precisely define the semester aggregation formula or whether pending/open sessions enter its denominator. Team must approve those rules.

## UC07 — View detailed course attendance

- **Primary actor:** Student.
- **Related requirement:** FR-08; Screen 5 (*This Course*).
- **Goal / trigger:** The student opens a specific class from UC06.
- **Preconditions:** Student is authenticated and is entitled to see the selected class.
- **Success postconditions:** The student sees their own session-by-session attendance records without changing data.

**Main success flow**
1. The student selects a class.
2. The system verifies the class is associated with that student.
3. It retrieves sessions in chronological order.
4. For each session, it displays date, topic/description, status, points, and remarks.
5. Status presentation distinguishes Present (10), Late (5), Excused (2), and Absent (0), following the source scoring table.
6. The page summarizes points for sessions included in the detailed history.

**Alternative / exception flows**
- A1. Session has not closed or no final record exists: show an appropriate unfinalized status **[Proposed; v3 does not define the exact label]**.
- A2. A leave request is pending: show its review status separately from attendance status **[Proposed]**.
- E1. A student requests another student's class or records: return an authorization failure.

**Special requirements:** Only the student’s own attendance history may be revealed. Clarify which sessions count toward the displayed totals **[Proposed]**.

## UC08 — View assigned classes

- **Primary actor:** Lecturer.
- **Related requirement:** FR-10.
- **Goal / trigger:** Lecturer selects **My Classes / Assigned Classes**.
- **Preconditions:** Lecturer is authenticated and has an allocation in course records.
- **Success postconditions:** The lecturer sees only their assigned classes and associated schedules.

**Main success flow**
1. The lecturer opens the assigned-classes page.
2. The system resolves the lecturer account against recorded teaching assignments.
3. The system retrieves linked course sections and schedules.
4. It shows course code, section, classroom, relevant class times and session status.
5. The lecturer selects a class to inspect or operate its sessions through UC09, UC11–UC15.

**Alternative / exception flows**
- A1. No assigned classes: display an empty list.
- A2. Lecturer has multiple sections of one course: display sections separately to avoid mixing their rosters.
- E1. Attempt to open an unassigned class: deny access.

**Special requirements:** Lecturer authorization must be based on stored assignments, not only a class identifier supplied by the browser **[Proposed security detail]**.

## UC09 — Open attendance session

- **Primary actor:** Lecturer.
- **Related requirement:** FR-09; may interact with UC12.
- **Goal / trigger:** Lecturer selects **Open / Activate Attendance Session** for an assigned class.
- **Preconditions:** Lecturer is authenticated, assigned to the class, and a scheduled session with room/geofence data exists.
- **Success postconditions:** Session is active; eligible students may attempt UC04 when the permitted time window is open.

**Main success flow**
1. Lecturer selects a class from UC08 and its scheduled meeting.
2. System displays scheduled start/end, room anchor and configured radius.
3. Lecturer verifies or configures geofence parameters through UC12 when required.
4. Lecturer selects **Open Attendance Session**.
5. System confirms lecturer ownership, schedule state, and session eligibility.
6. System marks the session active and presents the permitted check-in window (`T_start − 15` to the configured deadline described in v3).
7. Student dashboards expose the check-in action only when the session is active and within the permitted window.

**Alternative / exception flows**
- A1. A session is already active: display its existing state; do not create duplicate sessions **[Proposed]**.
- E1. Missing classroom coordinates or an invalid radius: block activation and require correction.
- E2. Lecturer is not assigned: reject activation.

**Special requirements:** The source does not define precisely whether activation can precede the early opening time, or how early closure interacts with scheduled end. Record these as team decisions.

## UC11 — Close attendance session

- **Primary actor:** Lecturer.
- **Related requirement:** FR-09.
- **Goal / trigger:** Lecturer selects **Close Attendance Session** after teaching or when the session should stop accepting check-ins.
- **Preconditions:** Lecturer is authorized for the class; the target session exists and is active.
- **Success postconditions:** Further check-ins are rejected; relevant attendance outcomes are finalized subject to leave-review handling.

**Main success flow**
1. Lecturer selects an active session.
2. System shows its roster, recorded attendance and any pending leave requests.
3. Lecturer confirms **Close**.
4. System marks the session closed and stops accepting UC04.
5. For eligible students without successful check-in and without an approved excuse, system determines absence according to FR-09; it retains pending request information separately.
6. System updates the roster summaries and confirms the closure.

**Alternative / exception flows**
- A1. Session already closed: report the current state without additional writes.
- A2. Pending leave requests exist: flag as pending review instead of silently treating them as approved or discarded **[Proposed; source says pending is an exception]**.
- E1. Database transaction fails: report failure; do not claim successful closure.

**Special requirements:** **[Proposed]** Record who closed the session and when. Source v3 does not define whether automatic timed closure is required or whether a closed session may reopen.

## UC12 — Configure session geofence

- **Primary actor:** Lecturer.
- **Related requirement:** FR-10; associated with UC09.
- **Goal / trigger:** Lecturer chooses to adjust a class session's permitted geographical radius.
- **Preconditions:** Lecturer is assigned to the class; its classroom GPS anchor exists.
- **Success postconditions:** The selected session uses a valid geofence radius for subsequent location checks.

**Main success flow**
1. Lecturer opens configuration for a class session.
2. System displays room identifier, master anchor coordinates and current radius.
3. Lecturer enters a permitted radius in the source-defined range `20 m ≤ R ≤ 100 m`.
4. System validates the numeric value and that the lecturer is authorized.
5. Lecturer saves; system associates the radius with the session.
6. Subsequent UC20 evaluations compare the student distance with that session radius.

**Alternative / exception flows**
- A1. Lecturer accepts the room's default radius (the v3 data table uses 35 m).
- E1. Radius is outside 20–100 m or invalid: reject the update.
- E2. Session is in a state where changing its radius is not permitted: reject the update **[Proposed; exact locking rule must be decided]**.

**Special requirements:** Administrator maintains the master anchor through UC16; lecturer configuration should not silently rewrite master classroom coordinates **[Proposed boundary]**.

## UC13 — Monitor live attendance radar

- **Primary actor:** Lecturer.
- **Related requirement:** FR-11; Screen 6.
- **Goal / trigger:** Lecturer opens **Live Attendance Radar** for an assigned active session.
- **Preconditions:** Lecturer is authenticated and assigned; the session has roster and attendance data.
- **Success postconditions:** Lecturer sees currently recorded check-in markers and counts; no attendance is modified.

**Main success flow**
1. Lecturer chooses the active session.
2. System verifies the lecturer’s assignment.
3. System retrieves the session geofence center/radius and accepted attendance records.
4. Interface renders a geofence boundary, reported check-in positions, and Present/Late status markers.
5. System shows the counts of enrolled, Present, Late, Excused, and unrecorded/Absent students, and a recent check-in list.
6. Interface updates the view when new accepted records arrive, as specified by the v3 real-time concept.

**Alternative / exception flows**
- A1. No one has checked in: display an empty radar with the session roster count.
- A2. An update channel is unavailable: display the last known snapshot and an update/retry indicator **[Proposed]**.
- E1. Unassigned lecturer requests the session radar: deny access.

**Special requirements:** The v3 concept requires WebSocket-like updates, but **a browser check-in location is only a submitted snapshot, not proof of continued physical presence or a continuously tracked device**. Do not assert either capability without separate implementation and consent. Live Radar is an unverified implementation feature.

## UC14 — Adjust attendance record

- **Primary actor:** Lecturer.
- **Related requirement:** FR-13.
- **Goal / trigger:** Assigned lecturer selects **Adjust Attendance** for a student/session with an exceptional attendance issue.
- **Preconditions:** Lecturer is authorized for the class; the selected student/session exists.
- **Success postconditions:** An authorized correction is saved and visible in the attendance history.

**Main success flow**
1. Lecturer opens a class/session roster.
2. Lecturer selects a student and chooses **Adjust Attendance**.
3. System displays the current status, score and remarks.
4. Lecturer selects a corrected status and supplies a reason **[Proposed: reason required for auditability]**.
5. System verifies authorization and status-to-points mapping (`Present 10`, `Late 5`, `Excused 2`, `Absent 0`).
6. Lecturer confirms the correction.
7. System updates the result and the class/student summaries.

**Alternative / exception flows**
- A1. No record exists: create or finalize one for the selected student/session **[Proposed; v3 does not specify manual record creation]**.
- A2. A leave request is pending or approved: warn about possible conflict and require an explicit decision **[Proposed]**.
- E1. Lecturer is unassigned or record is invalid: deny the change.

**Special requirements:** **[Proposed]** Keep an immutable before/after correction audit trail and avoid hard-coded arbitrary user-editable points. The source authorizes manual override but leaves its approval/audit procedure unspecified.

## UC15 — Export attendance report

- **Primary actor:** Lecturer.
- **Related requirement:** FR-13; Screen 6 links to export.
- **Goal / trigger:** Lecturer selects **Export Attendance Report** for an assigned class.
- **Preconditions:** Lecturer is authenticated and authorized for the class.
- **Success postconditions:** The system returns an attendance gradebook file; no attendance data changes.

**Main success flow**
1. Lecturer selects an assigned class and opens its attendance report.
2. System verifies the assignment and retrieves class roster/session outcomes.
3. Lecturer chooses an export format: Excel (`.xlsx`) or PDF (`.pdf`) as in v3.
4. System generates a file containing relevant student identifiers, sessions, statuses, points and summaries.
5. System makes the file available to the requesting lecturer.

**Alternative / exception flows**
- A1. No sessions have been completed: show a clear empty report or a file with an explanatory header **[Proposed]**.
- E1. File generation fails: display an error without a misleading success message.
- E2. User requests a class not assigned to them: reject download.

**Special requirements:** Downloads contain educational records and must respect role-based access; v3 does not specify a fixed institutional export template. Confirm required columns and file naming **[Proposed]**.

## UC16 — Manage classroom GPS locations

- **Primary actor:** Administrator.
- **Related requirement:** FR-15; Screen 8.
- **Goal / trigger:** Administrator opens **Classroom Directory / Geofence Management**.
- **Preconditions:** Administrator has authenticated with the Admin role.
- **Success postconditions:** Classroom master data is created or updated.

**Main success flow**
1. Administrator opens the classroom management list.
2. System displays room code, building/campus, anchor latitude, longitude and default radius.
3. Administrator chooses create or edit.
4. Administrator enters or updates room details and GPS anchor coordinates.
5. System validates the supplied fields and authorized role.
6. Administrator saves; the master classroom entry is persisted.
7. System displays the updated directory.

**Alternative / exception flows**
- A1. Administrator cancels: no changes are saved.
- A2. Administrator calibrates an existing room coordinate and confirms the update.
- E1. Coordinates are invalid or room identifier is duplicated: reject the update **[Proposed validation details]**.

**Special requirements:** The source requests seven-decimal-place coordinates and a baseline radius. **[Proposed]** Preserve historical session results when classroom master coordinates later change; v3 does not define versioning/snapshot rules.

## UC17 — Manage user accounts

- **Primary actor:** Administrator.
- **Related requirement:** FR-16; Screen 8.
- **Goal / trigger:** Administrator opens **User Management**.
- **Preconditions:** Administrator is authenticated and authorized.
- **Success postconditions:** A user account/profile is created or administratively updated.

**Main success flow**
1. Administrator opens the account directory.
2. System displays accounts and their recorded roles (Student, Lecturer, Administrator).
3. Administrator selects create or update.
4. Administrator enters the user identifier, name, contact details and role as appropriate.
5. System checks required fields, uniqueness and authorization.
6. Administrator saves; the account and related student/lecturer profile are updated as appropriate.
7. System confirms success.

**Alternative / exception flows**
- A1. Administrator cancels: system leaves the account unchanged.
- A2. Administrator disables an account **[Proposed; v3 says 'manage' but does not define deactivation behavior]**.
- E1. Duplicate username/email or invalid role data: reject change.
- E2. Unauthorized user attempts administrative changes: reject request.

**Special requirements:** Never store passwords in plaintext. Source v3 calls for salted password hashing and role-based authorization. Account lifecycle (reset, deactivation, role changes, self-management) requires team confirmation.

## UC18 — Reset device binding

- **Primary actor:** Administrator.
- **Related requirement:** FR-16; linked to FR-03 and UC21; Screen 8.
- **Goal / trigger:** Administrator receives a request to reset a student's associated device identifier.
- **Preconditions:** Admin is authenticated; the student profile exists and has a device binding.
- **Success postconditions:** The old binding is cleared or reset, allowing the approved student to enroll a replacement device under the selected binding policy.

**Main success flow**
1. Administrator opens **Device Binding Management**.
2. System lists student account codes and currently registered device-binding status.
3. Administrator locates the student who requested a device replacement.
4. Administrator selects **Reset Device Binding**.
5. System displays a confirmation stating that the existing binding will be removed.
6. Administrator confirms; the system removes the binding association.
7. System shows the reset result.

**Alternative / exception flows**
- A1. No binding exists: the system reports that nothing needs resetting.
- E1. Student record not found or requester is not an administrator: deny action.
- E2. Save fails: no successful reset is claimed.

**Special requirements:** **[Proposed]** Record the reason, approver and time of a reset, and verify the student's request using an institutional process. Browser-derived fingerprints cannot reliably identify a unique physical device on every mobile browser; the technical guarantee asserted in FR-03 requires a feasibility review.

## UC19 — View security audit logs

- **Primary actor:** Administrator.
- **Related requirement:** FR-17; Screen 8.
- **Goal / trigger:** Administrator opens **Security Audit Logs**.
- **Preconditions:** Administrator is authenticated with audit-read permission.
- **Success postconditions:** Authorized security events are displayed for inspection only.

**Main success flow**
1. Administrator opens the audit interface.
2. System verifies their administrative access.
3. System retrieves recorded check-in and administration security events.
4. The interface displays event timestamp, relevant account, event type, outcome and other safe metadata.
5. Administrator inspects suspected duplicate-device, geofence-anomaly or mock-location events, where such events are actually recorded.

**Alternative / exception flows**
- A1. No matching events: display an empty state.
- A2. Administrator filters by user/event/date **[Proposed UI behaviour]**.
- E1. Unauthorized account requests logs: deny access.
- E2. Logging data is unavailable: indicate failure; do not synthesize events.

**Special requirements:** **[Proposed]** Do not expose passwords, credentials, full evidence documents or unneeded precise location data in log views. Source v3 describes intended anomaly logs, but automatic reliable detection of Fake GPS is not established.

## UC20 — Validate GPS location (`«include»` from UC04)

- **Initiating context:** UC04, started by Student; the system performs this validation internally.
- **Related requirement:** FR-05; NFR-04; source location rules.
- **Goal / trigger:** UC04 supplies browser-reported GPS latitude, longitude and accuracy for a session.
- **Preconditions:** Student is authenticated; the session exists; the browser returns a location reading.
- **Success postconditions:** A valid location comparison result (`inside geofence` / `outside geofence`) is returned to UC04; UC20 itself does not create an attendance record.

**Main success flow**
1. UC04 provides the location tuple and target attendance session.
2. System checks that coordinates and reported accuracy are present.
3. System validates the source-design requirement that reported accuracy is at most 20 metres.
4. System retrieves the session's classroom anchor and configured radius.
5. System calculates the Haversine great-circle distance using Earth radius 6,371,000 metres, as specified by the source.
6. System compares the measured distance `d` with the session's allowed geofence radius `R`.
7. If `d ≤ R`, UC20 returns a valid in-range decision to UC04.

**Alternative / exception flows**
- A1. `d > R`: return out-of-range plus distance feedback; UC04 rejects check-in.
- E1. Browser location permission is denied or no fix is available: UC04 displays an acquisition error.
- E2. Latitude, longitude, accuracy or room center is invalid/missing: do not accept check-in **[Proposed strict input handling]**.

**Special requirements:** Source v3 does not define GPS sample freshness or tamper-proof acquisition. Browser geolocation readings and Haversine comparisons alone do **not** prove a student is inside a particular room or establish resistance to all location spoofing. The exact edge thresholds and overlapping timestamps require team testing.

## UC21 — Verify device binding (`«include»` from UC04)

- **Initiating context:** UC04, started by Student; the system performs this check internally.
- **Related requirement:** FR-03; NFR-10; UC18.
- **Goal / trigger:** UC04 submits a check-in that requires verification of the claimed device identifier.
- **Preconditions:** Student is authenticated; device binding is enabled for the project; the account and binding record are available.
- **Success postconditions:** UC04 receives an allow/reject decision; UC21 itself does not write an attendance result.

**Main success flow**
1. UC04 identifies the authenticated student account and the browser-provided device identifier/signature.
2. System retrieves the student's existing device binding, where applicable.
3. System checks whether the supplied identifier matches the expected student binding.
4. System checks whether the identifier is already associated with another account for the relevant active session, as envisaged by FR-03.
5. If the checks pass, UC21 allows UC04 to proceed to the other check-in validations.

**Alternative / exception flows**
- A1. No existing binding is present: follow first-device-enrollment handling **[Proposed; requires an approved binding policy]**.
- A2. Supplied identifier is already associated with another student: reject the attempted check-in, display the conflict, and provide an administrator-contact path.
- E1. Identifier cannot be obtained or is inconsistent: block or escalate under the selected policy **[Proposed; exact fallback not defined by v3]**.

**Special requirements:** The v3 SRS assumes reliable physical-device identity from a browser hardware fingerprint. That guarantee is **not supported by standard mobile-web access alone** and needs explicit technical and privacy review. This specification reflects the intended rule, **not a validated anti-proxy mechanism**. Device reset is handled in UC18.

---

## Traceability and unresolved requirements

| UC | Source use-case name | Primary actor | v3 FR link | Source completeness |
|---|---|---|---|---|
| UC02 | Sign out | System User | FR-01 (related, no explicit sign-out FR) | Supplement proposed |
| UC03 | View today's schedule | Student | FR-04 | Supplement proposed |
| UC06 | View attendance summary | Student | FR-07 | Supplement proposed |
| UC07 | View detailed course attendance | Student | FR-08 | Supplement proposed |
| UC08 | View assigned classes | Lecturer | FR-10 | Supplement proposed |
| UC09 | Open attendance session | Lecturer | FR-09 | Supplement proposed |
| UC11 | Close attendance session | Lecturer | FR-09 | Supplement proposed |
| UC12 | Configure session geofence | Lecturer | FR-10 | Supplement proposed |
| UC13 | Monitor live attendance radar | Lecturer | FR-11 | Supplement proposed |
| UC14 | Adjust attendance record | Lecturer | FR-13 | Supplement proposed |
| UC15 | Export attendance report | Lecturer | FR-13 | Supplement proposed |
| UC16 | Manage classroom GPS locations | Administrator | FR-15 | Supplement proposed |
| UC17 | Manage user accounts | Administrator | FR-16 | Supplement proposed |
| UC18 | Reset device binding | Administrator | FR-16, FR-03 | Supplement proposed |
| UC19 | View security audit logs | Administrator | FR-17 | Supplement proposed |
| UC20 | Validate GPS location | Included from UC04 | FR-05 | Supplement proposed |
| UC21 | Verify device binding | Included from UC04 | FR-03 | Supplement proposed |

**Still present and unchanged in the original Word:** UC01 (Sign in), UC04 (GPS Check-in), UC05 (Submit Leave Request), UC10 (Review Leave Requests). These four have detailed specifications in v3. Together with this supplement, all UC01–UC21 now have a **written draft specification**, pending team validation.

### Decisions Group 01 must confirm before treating this as the approved baseline

1. **Timing boundary:** The source labels *Present* through `T_start + 15 minutes` and *Late* from `T_start + 16 minutes`, leaving the seconds between +15:00 and +16:00 unspecified. Define continuous boundaries precisely. Also confirm behavior when the scheduled class ends before +45 minutes.
2. **Session lifecycle:** Decide who can activate early, whether sessions close automatically at their deadlines, whether they can be reopened, and how to finalize attendance while leave requests are pending.
3. **Aggregation:** Define the numerator/denominator for course and semester attendance percentages, whether Excused counts as attendance, and how to treat ongoing/pending sessions.
4. **GPS policy:** Define location acquisition age/freshness, reliability acceptance in real classrooms, and cases where accuracy exceeds 20 metres. The geofence does not establish exact room-level presence.
5. **Device binding and Fake GPS:** Decide whether device fingerprinting is feasible and proportionate. Do not promise hardware uniqueness or foolproof Fake GPS detection in an ordinary browser.
6. **Correction and audit:** Approve who can manually correct a result, required justifications, conflict handling with approved leave requests and the audit retention policy.
7. **Reporting and user administration:** Confirm exports, account lifecycle and whether the lecturer/admin interfaces shown in v3 cover all intended actions.
8. **Implementation evidence:** A specification is not proof that Live Radar, WebSockets, anti-spoofing, 5,000–10,000 concurrency, or 99.8% availability was implemented or tested.

**Change rule:** Keep `RequirementAndDesignDocument_v3.docx` and the previously committed 25 files unchanged. Submit this one additional Markdown file as a **separate documentation commit**; after Group 01 approves outstanding decisions, update the formal document in a later, explicitly reviewed revision.
