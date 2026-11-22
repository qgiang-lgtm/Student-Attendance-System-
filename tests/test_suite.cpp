#include <iostream>
#include <cassert>
#include <cmath>
#include <string>
#include <memory>
#include "../src/models/Models.hpp"
#include "../src/services/GeoService.hpp"
#include "../src/services/AttendanceGradingService.hpp"
#include "../src/services/AntiFraudService.hpp"
#include "../src/services/AuthService.hpp"
#include "../src/repository/DatabaseRepository.hpp"
#include "../src/server/ApiRouter.hpp"

using namespace attendance;

int testsTotal = 0;
int testsPassed = 0;

#define TEST_ASSERT(cond, msg) \
    do { \
        testsTotal++; \
        if (cond) { \
            testsPassed++; \
            std::cout << "  [PASS] " << msg << std::endl; \
        } else { \
            std::cerr << "  [FAIL] " << msg << " (" << #cond << ")" << std::endl; \
        } \
    } while(0)

void testHaversineDistance() {
    std::cout << "\n>>> [SUITE 1] Haversine Spherical Distance Calculation (NFR-04)" << std::endl;
    // B204 classroom anchor: 10.823098, 106.629663
    double anchorLat = 10.823098;
    double anchorLon = 106.629663;

    // Test zero distance
    double d0 = GeoService::calculateDistance(anchorLat, anchorLon, anchorLat, anchorLon);
    TEST_ASSERT(std::abs(d0) < 0.001, "Distance to self is 0 meters");

    // Test nearby point inside room (~7.0m)
    double clientLat = 10.823150;
    double clientLon = 106.629700;
    double d1 = GeoService::calculateDistance(clientLat, clientLon, anchorLat, anchorLon);
    TEST_ASSERT(d1 > 5.0 && d1 < 15.0, "Nearby coordinate distance calculated accurately (~7m - 12m)");

    // Test far point (~250m away)
    double farLat = 10.825000;
    double farLon = 106.631000;
    double dFar = GeoService::calculateDistance(farLat, farLon, anchorLat, anchorLon);
    TEST_ASSERT(dFar > 200.0, "Far coordinate distance exceeds 200 meters");
}

void testGeofenceEvaluation() {
    std::cout << "\n>>> [SUITE 2] Geofence Boundary Evaluation (NFR-04, FR-05)" << std::endl;
    double anchorLat = 10.823098;
    double anchorLon = 106.629663;
    double radius = 35.0;

    // Coordinate at ~7m is inside 35m radius
    bool inside = GeoService::isWithinGeofence(10.823150, 106.629700, anchorLat, anchorLon, radius);
    TEST_ASSERT(inside == true, "Coordinate inside 35m radius is detected as INSIDE");

    // Coordinate at 250m is outside 35m radius
    bool outside = GeoService::isWithinGeofence(10.825000, 106.631000, anchorLat, anchorLon, radius);
    TEST_ASSERT(outside == false, "Coordinate outside 35m radius is detected as OUTSIDE");
}

void testAttendanceGradingBusinessRules() {
    std::cout << "\n>>> [SUITE 3] Attendance Business Rules Grading Engine (Section II.2)" << std::endl;
    std::string startTime = "07:00";
    double radius = 35.0;

    // 1. On-time early window: 06:45 (T_start - 15) -> 10/10 pts
    auto rEarly = AttendanceGradingService::evaluateCheckIn("06:45", startTime, 12.0, radius);
    TEST_ASSERT(rEarly.status == AttendanceStatus::PRESENT && rEarly.points == 10, "Early check-in at 06:45 is Present (10/10)");

    // 2. On-time standard: 06:58 -> 10/10 pts
    auto rOnTime = AttendanceGradingService::evaluateCheckIn("06:58", startTime, 10.0, radius);
    TEST_ASSERT(rOnTime.status == AttendanceStatus::PRESENT && rOnTime.points == 10, "On-time check-in at 06:58 is Present (10/10)");

    // 3. On-time boundary: 07:15 (T_start + 15) -> 10/10 pts
    auto rOnTimeBoundary = AttendanceGradingService::evaluateCheckIn("07:15", startTime, 14.0, radius);
    TEST_ASSERT(rOnTimeBoundary.status == AttendanceStatus::PRESENT && rOnTimeBoundary.points == 10, "Boundary check-in at 07:15 is Present (10/10)");

    // 4. Late window: 07:18 (T_start + 18) -> 5/10 pts
    auto rLate = AttendanceGradingService::evaluateCheckIn("07:18", startTime, 8.0, radius);
    TEST_ASSERT(rLate.status == AttendanceStatus::LATE && rLate.points == 5, "Late check-in at 07:18 is Late (5/10)");

    // 5. Late window boundary: 07:45 (T_start + 45) -> 5/10 pts
    auto rLateBoundary = AttendanceGradingService::evaluateCheckIn("07:45", startTime, 15.0, radius);
    TEST_ASSERT(rLateBoundary.status == AttendanceStatus::LATE && rLateBoundary.points == 5, "Boundary check-in at 07:45 is Late (5/10)");

    // 6. After late cutoff: 07:46 -> Absent 0/10 pts
    auto rAbsent = AttendanceGradingService::evaluateCheckIn("07:46", startTime, 10.0, radius);
    TEST_ASSERT(rAbsent.status == AttendanceStatus::ABSENT && rAbsent.points == 0, "Cutoff expired at 07:46 is Absent (0/10)");

    // 7. Outside Geofence: d = 60m > 35m -> Rejected 0/10 pts
    auto rOut = AttendanceGradingService::evaluateCheckIn("06:55", startTime, 60.0, radius);
    TEST_ASSERT(rOut.status == AttendanceStatus::REJECTED && rOut.points == 0, "Check-in outside perimeter is Rejected (0/10)");

    // 8. Leave request approved -> Excused 2/10 pts
    auto rLeaveApprove = AttendanceGradingService::evaluateLeaveAdjudication(true, "Medical Illness");
    TEST_ASSERT(rLeaveApprove.status == AttendanceStatus::EXCUSED && rLeaveApprove.points == 2, "Approved leave awards Excused (2/10)");

    // 9. Leave request rejected -> Unexcused 0/10 pts
    auto rLeaveReject = AttendanceGradingService::evaluateLeaveAdjudication(false, "Unverified");
    TEST_ASSERT(rLeaveReject.status == AttendanceStatus::ABSENT && rLeaveReject.points == 0, "Rejected leave awards Unexcused (0/10)");
}

void testAntiFraudAndSecurity() {
    std::cout << "\n>>> [SUITE 4] Anti-Fraud & Anti-Proxy Device Binding (NFR-09, NFR-10, FR-03)" << std::endl;

    // 1. Accuracy Check (Threshold <= 20m)
    auto accOk = AntiFraudService::validateAccuracy(15.0);
    TEST_ASSERT(accOk.is_valid == true, "GPS accuracy of 15m is ACCEPTED (<= 20m)");

    auto accBad = AntiFraudService::validateAccuracy(35.0);
    TEST_ASSERT(accBad.is_valid == false, "GPS accuracy of 35m is REJECTED (> 20m)");

    // 2. Mock GPS Detection
    auto mockBad = AntiFraudService::checkMockLocation(true, false);
    TEST_ASSERT(mockBad.is_valid == false, "Mock Location flag detected and BLOCKED");

    // 3. Anti-Proxy Handset Binding: One device used for multiple students in same session
    std::unordered_map<std::string, std::string> sessionDevices;
    sessionDevices["DEV-IP15-9921"] = "B22DCCN001"; // Alexander Tran already checked in

    // Another student tries to check in with the same handset
    auto proxyCheck = AntiFraudService::verifyDeviceBinding(
        2, "B22DCCN045", "DEV-IP15-9921", "DEV-SS24-1102", true, sessionDevices
    );
    TEST_ASSERT(proxyCheck.is_valid == false, "Proxy check-in with same handset for different student is BLOCKED");
}

void testUniversalAuthAndRoleRouting() {
    std::cout << "\n>>> [SUITE 5] Universal Authentication & Admin Auto-Routing (FR-01, FR-02)" << std::endl;

    User adminUser{99, "ADM001", "admin123", "System Administrator", "admin@uni.edu.vn", UserRole::ADMIN, ""};
    User studentUser{1, "B22DCCN001", "password123", "Alexander Tran", "tran@uni.edu.vn", UserRole::STUDENT, ""};
    User lecturerUser{2, "GV101", "password123", "Dr. Alexander Vance", "vance@uni.edu.vn", UserRole::LECTURER, ""};

    // Admin logs in through "Student" tab -> Auto-routes to Admin Portal
    auto resAdminFromStudent = AuthService::authenticate(adminUser, "admin123", "Student");
    TEST_ASSERT(resAdminFromStudent.success == true && resAdminFromStudent.redirect_url == "/admin-dashboard", 
                "Admin logging in through Student tab auto-routed to Admin Dashboard");

    // Admin logs in through "Lecturer" tab -> Auto-routes to Admin Portal
    auto resAdminFromLecturer = AuthService::authenticate(adminUser, "admin123", "Lecturer");
    TEST_ASSERT(resAdminFromLecturer.success == true && resAdminFromLecturer.redirect_url == "/admin-dashboard", 
                "Admin logging in through Lecturer tab auto-routed to Admin Dashboard");

    // Student logs in -> Routes to Student Dashboard
    auto resStudent = AuthService::authenticate(studentUser, "password123", "Student");
    TEST_ASSERT(resStudent.success == true && resStudent.redirect_url == "/student-dashboard", 
                "Student logging in routed to Student Dashboard");

    // Lecturer logs in -> Routes to Lecturer Dashboard
    auto resLecturer = AuthService::authenticate(lecturerUser, "password123", "Lecturer");
    TEST_ASSERT(resLecturer.success == true && resLecturer.redirect_url == "/lecturer-dashboard", 
                "Lecturer logging in routed to Lecturer Dashboard");
}

void testDatabaseRepositoryIntegration() {
    std::cout << "\n>>> [SUITE 6] Repository & Use Case Integration (UC01 - UC21)" << std::endl;

    auto repo = std::make_shared<DatabaseRepository>();

    // 1. Process valid student check-in
    DatabaseRepository::CheckInInput in1;
    in1.session_id = 1;
    in1.student_code = "B22DCCN001";
    in1.checkin_time = "06:58:12";
    in1.client_lat = 10.823150;
    in1.client_long = 106.629700;
    in1.accuracy = 4.0;
    in1.device_id = "DEV-IP15-9921";
    in1.mock_flag = false;

    auto out1 = repo->processCheckIn(in1);
    TEST_ASSERT(out1.success == true && out1.points == 10, "Student B22DCCN001 check-in processed: Present 10/10");

    // 2. Submit Leave Request
    int reqId = repo->submitLeaveRequest(1, "B22DCCN120", "Medical Illness", "Acute fever", "slip.jpg");
    TEST_ASSERT(reqId > 0, "Leave request submitted with PENDING status");

    // 3. Adjudicate Leave Request (Lecturer approves -> 2 pts)
    bool adjOk = repo->adjudicateLeaveRequest(reqId, true, "Approved by Dr. Vance", 2);
    TEST_ASSERT(adjOk == true, "Lecturer adjudicated leave request: Excused 2/10 pts awarded");

    // 4. Admin Reset Device Lock
    bool rstOk = repo->resetDeviceBinding("B22DCCN001");
    TEST_ASSERT(rstOk == true, "Admin successfully reset hardware device binding lock");
}

void testGeofenceZoneControl() {
    std::cout << "\n>>> [SUITE 7] Check-in Zone (Geofence) Control (UC22 - UC24)" << std::endl;

    auto repo = std::make_shared<DatabaseRepository>();

    // 1. Lecturer picks a new circle (centre + radius) for the active session
    bool moved = repo->updateSessionGeofence(1, 10.824000, 106.630500, 60.0);
    auto zone = repo->getGeofence(1);
    TEST_ASSERT(moved && zone.found &&
                std::abs(zone.latitude - 10.824000) < 1e-9 &&
                std::abs(zone.longitude - 106.630500) < 1e-9 &&
                std::abs(zone.radius - 60.0) < 1e-9,
                "Lecturer moves the check-in circle: new centre + radius are persisted");

    // 2. A check-in outside the selected circle is rejected
    DatabaseRepository::CheckInInput out;
    out.session_id = 1;
    out.student_code = "B22DCCN001";
    out.checkin_time = "06:58:00";
    out.client_lat = 10.823150;   // old classroom spot, now outside the 60 m circle
    out.client_long = 106.629700;
    out.accuracy = 4.0;
    out.device_id = "DEV-IP15-9921";
    auto rOut = repo->processCheckIn(out);
    TEST_ASSERT(!rOut.success && rOut.status == AttendanceStatus::REJECTED && rOut.points == 0,
                "Check-in outside the selected circle is REJECTED (0/10)");

    // 3. A check-in inside the selected circle is accepted
    DatabaseRepository::CheckInInput in;
    in.session_id = 1;
    in.student_code = "B22DCCN045";
    in.checkin_time = "06:58:00";
    in.client_lat = 10.824010;
    in.client_long = 106.630510;
    in.accuracy = 4.0;
    in.device_id = "DEV-SS24-1102";
    auto rIn = repo->processCheckIn(in);
    TEST_ASSERT(rIn.success && rIn.status == AttendanceStatus::PRESENT && rIn.points == 10,
                "Check-in inside the selected circle is PRESENT (10/10)");

    // 4. RBAC: a student may not change the zone (HTTP 403)
    ApiRouter router(repo);
    auto resStudent = router.handleRequest(
        "POST", "/api/lecturer/geofence",
        R"({"role":"STUDENT","lat":10.824,"long":106.6305,"radius":50})");
    TEST_ASSERT(resStudent.status_code == 403,
                "Student is forbidden from changing the check-in zone (HTTP 403)");

    // 5. RBAC: a lecturer may change the zone (HTTP 200)
    auto resLecturer = router.handleRequest(
        "POST", "/api/lecturer/geofence",
        R"({"role":"LECTURER","lat":10.824,"long":106.6305,"radius":50})");
    TEST_ASSERT(resLecturer.status_code == 200,
                "Lecturer can change the check-in zone (HTTP 200)");
}

int main() {
    std::cout << "===========================================================" << std::endl;
    std::cout << "  GPS ATTENDANCE SYSTEM - AUTOMATED UNIT TEST SUITE (C++)   " << std::endl;
    std::cout << "===========================================================" << std::endl;

    testHaversineDistance();
    testGeofenceEvaluation();
    testAttendanceGradingBusinessRules();
    testAntiFraudAndSecurity();
    testUniversalAuthAndRoleRouting();
    testDatabaseRepositoryIntegration();
    testGeofenceZoneControl();

    std::cout << "\n===========================================================" << std::endl;
    std::cout << " TEST RESULTS SUMMARY: " << testsPassed << " / " << testsTotal << " PASSED (" 
              << (testsPassed * 100 / testsTotal) << "%)" << std::endl;
    std::cout << "===========================================================" << std::endl;

    return (testsPassed == testsTotal) ? 0 : 1;
}
