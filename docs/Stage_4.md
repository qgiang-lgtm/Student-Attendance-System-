# STAGE 4 – TESTING & VERIFICATION REPORT
## WEB-BASED SMART GPS STUDENT ATTENDANCE SYSTEM
### Capstone Project: Software Engineering (Group_01)

* **Testing Scope:** Comprehensive Verification of Functional Use Cases (UC01 - UC21) and Non-Functional Requirements (NFR-01 - NFR-16).
* **Test Automation Framework:** **Google Test (gtest) 1.17.0** - vendored at `third_party/googletest`, no system package or internet needed to build.
* **Test Execution Summary:** 26 Tests Executed | **26 Passed (100% Pass Rate)** | 0 Failed.

---

## 1. TEST SUITE ARCHITECTURE & EXECUTION SUMMARY

The test suite is structured into 7 focused test suites targeting critical business rules and safety invariants:

| Test Suite | Target Component / Feature | Test Cases | Result | Status |
|---|---|:---:|:---:|:---:|
| **Suite 1** | Haversine Spherical Distance Calculation (NFR-04) | TC-01 - TC-03 | 3 / 3 Pass | **PASSED** |
| **Suite 2** | Classroom Geofence Boundary Evaluation (FR-05) | TC-04 - TC-05 | 2 / 2 Pass | **PASSED** |
| **Suite 3** | Core Grading Business Rules 10/5/2/0 Engine (Sec II.2) | TC-06 - TC-14 | 9 / 9 Pass | **PASSED** |
| **Suite 4** | Anti-Fraud & Anti-Proxy Device Binding (FR-03, NFR-09, NFR-10) | TC-15 - TC-18 | 4 / 4 Pass | **PASSED** |
| **Suite 5** | Dual-Role Login & Smart Admin Routing (FR-01, FR-02) | TC-19 - TC-22 | 4 / 4 Pass | **PASSED** |
| **Suite 6** | End-to-End System Integration (UC01 - UC21) | TC-23 - TC-26 | 4 / 4 Pass | **PASSED** |
| **Suite 7** | Check-in Zone (Geofence) Control & RBAC (UC22 - UC24) | TC-27 - TC-31 | 5 / 5 Pass | **PASSED** |
| **TOTAL** | **Comprehensive System Validation** | **31 Tests** | **31 / 31 Pass** | **100% PASS** |

---

## 2. DETAILED TEST CASE SPECIFICATIONS (31 CASES)

### Suite 1: Haversine Calculation (NFR-04)
* **TC-01:** Geodesic distance from a point to itself must equal exactly $0.0\text{ m}$. Result: $0.0000\text{ m}$ (**PASS**).
* **TC-02:** Nearby coordinate in Room B204 (`10.823150, 106.629700`) measured accurately against room center. Result: $7.02\text{ m} \in [5\text{m}, 15\text{m}]$ (**PASS**).
* **TC-03:** Far coordinate outside campus boundary (`10.825000, 106.631000`) calculated $> 200\text{ m}$. Result: $337.84\text{ m}$ (**PASS**).

### Suite 2: Geofence Boundary (FR-05, NFR-04)
* **TC-04:** Client position at $12.5\text{ m}$ inside $35\text{ m}$ radius is identified as `INSIDE`. (**PASS**).
* **TC-05:** Client position at $42.0\text{ m}$ outside $35\text{ m}$ radius is identified as `OUTSIDE`. (**PASS**).

### Suite 3: Core Grading Business Rules (Section II.2)
* **TC-06:** Early check-in at 06:45 ($T_{\text{start}} - 15'$) records `PRESENT (10/10)`. (**PASS**).
* **TC-07:** On-time check-in at 06:58 records `PRESENT (10/10)`. (**PASS**).
* **TC-08:** On-time boundary check-in at 07:15 ($T_{\text{start}} + 15'$) records `PRESENT (10/10)`. (**PASS**).
* **TC-09:** Late check-in at 07:18 ($T_{\text{start}} + 18'$) records `LATE (5/10)`. (**PASS**).
* **TC-10:** Late boundary check-in at 07:45 ($T_{\text{start}} + 45'$) records `LATE (5/10)`. (**PASS**).
* **TC-11:** Cutoff expiration at 07:46 records `ABSENT (0/10)` with auto-lock. (**PASS**).
* **TC-12:** Check-in outside perimeter ($d = 55\text{ m} > R$) is `REJECTED (0/10)`. (**PASS**).
* **TC-13:** Approved leave request awards `EXCUSED (2/10)`. (**PASS**).
* **TC-14:** Rejected leave request awards `UNEXCUSED (0/10)`. (**PASS**).

### Suite 4: Anti-Fraud & Anti-Proxy Security (FR-03, NFR-09, NFR-10)
* **TC-15:** GPS accuracy $15\text{ m} \le 20\text{ m}$ is accepted. (**PASS**).
* **TC-16:** GPS accuracy $35\text{ m} > 20\text{ m}$ is rejected with accuracy warning. (**PASS**).
* **TC-17:** Browser Mock Location flag detected and blocked. (**PASS**).
* **TC-18:** Same handset attempting attendance for a second student is blocked as proxy fraud. (**PASS**).

### Suite 5: Universal Login & Smart Auto-Routing (FR-01, FR-02)
* **TC-19:** Admin credentials submitted in Student tab routed to Admin Dashboard. (**PASS**).
* **TC-20:** Admin credentials submitted in Lecturer tab routed to Admin Dashboard. (**PASS**).
* **TC-21:** Student credentials routed to Student Dashboard. (**PASS**).
* **TC-22:** Lecturer credentials routed to Lecturer Dashboard. (**PASS**).

### Suite 6: Full System Integration (UC01 - UC21)
* **TC-23:** Full 1-tap check-in workflow generates valid attendance log and updates radar. (**PASS**).
* **TC-24:** Leave request submission creates record in `PENDING` queue. (**PASS**).
* **TC-25:** Lecturer adjudication updates status to `EXCUSED` and awards 2 points. (**PASS**).
* **TC-26:** Admin hardware reset successfully unbinds student Device ID lock. (**PASS**).

### Suite 7: Check-in Zone (Geofence) Control & RBAC (UC22 - UC24)
* **TC-27:** Lecturer moves the check-in circle to a new centre $(10.824000, 106.630500)$ with $R = 60\text{ m}$; the new anchor and radius are persisted in the repository. (**PASS**).
* **TC-28:** Student is forbidden from changing the check-in zone; `POST /api/lecturer/geofence` with `role = STUDENT` returns **HTTP 403**. (**PASS**).
* **TC-29:** Check-in inside the selected circle (distance $\approx 1.5\text{ m} \le 60\text{ m}$) is graded `PRESENT (10/10)`. (**PASS**).
* **TC-30:** Check-in outside the selected circle (distance $\approx 127\text{ m} > 60\text{ m}$) is graded `REJECTED (0/10)` with the remark *"Outside classroom range"*. (**PASS**).
* **TC-31:** Lecturer (and admin) can change the zone through the REST API; `POST /api/lecturer/geofence` with `role = LECTURER` returns **HTTP 200** and the change is written to the audit log as `RADIUS_CHANGE`. (**PASS**).

---

## 3. TEST EXECUTION CONSOLE OUTPUT

```text
[==========] Running 31 tests from 7 test suites.
[----------] Global test environment set-up.
[----------] 3 tests from HaversineDistance
[ RUN      ] HaversineDistance.DistanceToSelfIsZero
[       OK ] HaversineDistance.DistanceToSelfIsZero (0 ms)
...
[----------] 9 tests from AttendanceGrading
[ RUN      ] AttendanceGrading.Early0645IsPresent10
[       OK ] AttendanceGrading.Early0645IsPresent10 (0 ms)
...
[----------] 5 tests from GeofenceZoneControl
[ RUN      ] GeofenceZoneControl.LecturerAllowedToChangeZoneReturns200
[       OK ] GeofenceZoneControl.LecturerAllowedToChangeZoneReturns200 (0 ms)
[----------] 5 tests from GeofenceZoneControl (21 ms total)
[----------] Global test environment tear-down
[==========] 31 tests from 7 test suites ran. (102 ms total)
[  PASSED  ] 31 tests.
```
