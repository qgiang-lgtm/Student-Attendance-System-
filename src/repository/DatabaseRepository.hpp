#pragma once
#include <vector>
#include <unordered_map>
#include <mutex>
#include <memory>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include "../models/Models.hpp"
#include "../services/GeoService.hpp"
#include "../services/AttendanceGradingService.hpp"
#include "../services/AntiFraudService.hpp"

namespace attendance {

class DatabaseRepository {
private:
    std::mutex mtx;

    // In-Memory Relational Stores
    std::unordered_map<int, User> users;
    std::unordered_map<int, Student> students; // key: user_id
    std::unordered_map<int, Lecturer> lecturers; // key: user_id
    std::unordered_map<int, Admin> admins; // key: user_id
    std::unordered_map<int, Classroom> classrooms;
    std::unordered_map<int, CourseClass> course_classes;
    std::unordered_map<int, AttendanceSession> sessions;
    std::unordered_map<int, AttendanceRecord> records;
    std::unordered_map<int, LeaveRequest> leave_requests;
    std::vector<SecurityAuditLog> audit_logs;
    std::vector<DeviceResetRequest> device_reset_requests;

    int nextUserId{1};
    int nextClassroomId{1};
    int nextCourseId{1};
    int nextSessionId{1};
    int nextRecordId{1};
    int nextLeaveId{101};
    int nextAuditId{1};
    int nextDeviceResetId{301};

    // Directory for persistent file storage (created next to the executable).
    std::string dataDir{"data"};

    // ----- File persistence helpers (simple pipe-delimited format) -----
    void ensureDataDir() {
        std::error_code ec;
        std::filesystem::create_directories(dataDir, ec);
    }

    std::vector<std::string> splitPipe(const std::string& line) {
        std::vector<std::string> out;
        std::string cur;
        for (char c : line) {
            if (c == '|') { out.push_back(cur); cur.clear(); }
            else cur += c;
        }
        out.push_back(cur);
        return out;
    }

    void saveUsersToFile() {
        ensureDataDir();
        std::ofstream f(dataDir + "/users.txt");
        if (!f) return;
        for (const auto& [id, u] : users) {
            f << u.id << "|" << u.username << "|" << u.password_hash << "|"
              << u.full_name << "|" << u.email << "|"
              << roleToString(u.role) << "|" << u.created_at << "|"
              << u.phone << "|" << u.date_of_birth << "\n";
        }
    }

    void loadUsersFromFile() {
        std::ifstream f(dataDir + "/users.txt");
        if (!f) return;
        std::string line;
        while (std::getline(f, line)) {
            if (line.empty()) continue;
            auto p = splitPipe(line);
            if (p.size() < 7) continue;
            User u;
            u.id = std::stoi(p[0]);
            u.username = p[1];
            u.password_hash = p[2];
            u.full_name = p[3];
            u.email = p[4];
            u.role = stringToRole(p[5]);
            u.created_at = p[6];
            // Backward compatible: older users.txt had 7 columns only.
            if (p.size() >= 9) {
                u.phone = p[7];
                u.date_of_birth = p[8];
            }
            users[u.id] = u;
            if (u.id >= nextUserId) nextUserId = u.id + 1;
        }
    }

    void saveStudentsToFile() {
        ensureDataDir();
        std::ofstream f(dataDir + "/students.txt");
        if (!f) return;
        for (const auto& [uid, s] : students) {
            f << s.user_id << "|" << s.student_code << "|"
              << s.current_device_id << "|" << (s.is_device_locked ? "1" : "0") << "\n";
        }
    }

    void loadStudentsFromFile() {
        std::ifstream f(dataDir + "/students.txt");
        if (!f) return;
        std::string line;
        while (std::getline(f, line)) {
            if (line.empty()) continue;
            auto p = splitPipe(line);
            if (p.size() < 4) continue;
            Student s;
            s.user_id = std::stoi(p[0]);
            s.student_code = p[1];
            s.current_device_id = p[2];
            s.is_device_locked = (p[3] == "1");
            students[s.user_id] = s;
        }
    }

    void saveAuditLogsToFile() {
        ensureDataDir();
        std::ofstream f(dataDir + "/audit.txt");
        if (!f) return;
        for (const auto& log : audit_logs) {
            f << log.id << "|" << log.timestamp << "|" << log.student_code << "|"
              << log.device_id << "|" << log.event_type << "|"
              << log.details << "|" << log.severity << "\n";
        }
    }

    void loadAuditLogsFromFile() {
        std::ifstream f(dataDir + "/audit.txt");
        if (!f) return;
        std::string line;
        while (std::getline(f, line)) {
            if (line.empty()) continue;
            auto p = splitPipe(line);
            if (p.size() < 7) continue;
            SecurityAuditLog log;
            log.id = std::stoi(p[0]);
            log.timestamp = p[1];
            log.student_code = p[2];
            log.device_id = p[3];
            log.event_type = p[4];
            log.details = p[5];
            log.severity = p[6];
            audit_logs.push_back(log);
            if (log.id >= nextAuditId) nextAuditId = log.id + 1;
        }
    }

    void saveDeviceResetRequestsToFile() {
        ensureDataDir();
        std::ofstream f(dataDir + "/device_reset_requests.txt");
        if (!f) return;
        for (const auto& r : device_reset_requests) {
            f << r.id << "|" << r.user_id << "|" << r.student_code << "|" << r.student_name << "|"
              << r.reason << "|" << r.explanation << "|" << r.file_name << "|" << r.file_size << "|"
              << r.status << "|" << r.created_at << "|" << r.reviewer_note << "\n";
        }
    }

    void loadDeviceResetRequestsFromFile() {
        std::ifstream f(dataDir + "/device_reset_requests.txt");
        if (!f) return;
        std::string line;
        while (std::getline(f, line)) {
            if (line.empty()) continue;
            auto p = splitPipe(line);
            if (p.size() < 11) continue;
            DeviceResetRequest r;
            r.id = std::stoi(p[0]);
            r.user_id = std::stoi(p[1]);
            r.student_code = p[2];
            r.student_name = p[3];
            r.reason = p[4];
            r.explanation = p[5];
            r.file_name = p[6];
            r.file_size = std::stoll(p[7]);
            r.status = p[8];
            r.created_at = p[9];
            r.reviewer_note = p[10];
            device_reset_requests.push_back(r);
            if (r.id >= nextDeviceResetId) nextDeviceResetId = r.id + 1;
        }
    }

    // Persist all mutable state after any change.
    void persistAll() {
        saveUsersToFile();
        saveStudentsToFile();
        saveAuditLogsToFile();
        saveDeviceResetRequestsToFile();
    }

    // Caller must already hold mtx (used inside processCheckIn to avoid self-deadlock).
    bool isDeviceBoundToAnotherStudentLocked(const std::string& deviceId, const std::string& studentCode) const {
        for (const auto& [uid, s] : students) {
            if (s.student_code == studentCode) continue; // same student is fine
            if (!s.current_device_id.empty() && s.current_device_id == deviceId) {
                return true;
            }
        }
        return false;
    }

public:
    DatabaseRepository() {
        seedInitialData();
        // Override with persisted data if files exist (survives container restart).
        loadUsersFromFile();
        loadStudentsFromFile();
        loadAuditLogsFromFile();
        loadDeviceResetRequestsFromFile();
    }

    void seedInitialData() {
        std::lock_guard<std::mutex> lock(mtx);

        // 1. Seed Classrooms
        Classroom b204{nextClassroomId++, "B204", "Bldg B (2nd floor)", 10.823098, 106.629663, 35.0};
        Classroom a301{nextClassroomId++, "A301", "Bldg A", 10.824150, 106.630120, 40.0};
        classrooms[b204.id] = b204;
        classrooms[a301.id] = a301;

        // 2. Seed Users & Lecturer
        User uLec{nextUserId++, "GV101", "password123", "Dr. Alexander Vance", "vance@university.edu.vn", UserRole::LECTURER, "2026-09-01 08:00:00"};
        users[uLec.id] = uLec;
        lecturers[uLec.id] = Lecturer{uLec.id, "GV101", "Software Engineering Dept"};

        // 3. Seed Course Classes
        CourseClass cs301{nextCourseId++, "CS301-G01", "[CS301] Software Engineering", uLec.id, "Fall 2026", "2026-2027"};
        CourseClass cs302{nextCourseId++, "CS302-G01", "[CS302] Computer Networks", uLec.id, "Fall 2026", "2026-2027"};
        course_classes[cs301.id] = cs301;
        course_classes[cs302.id] = cs302;

        // 4. Seed Students
        User uStu1{nextUserId++, "B22DCCN001", "password123", "Alexander Tran", "tran.b22@university.edu.vn", UserRole::STUDENT, "2026-09-01 08:00:00"};
        users[uStu1.id] = uStu1;
        students[uStu1.id] = Student{uStu1.id, "B22DCCN001", "DEV-IP15-9921", true};

        User uStu2{nextUserId++, "B22DCCN045", "password123", "Maria Nguyen", "nguyen.b22@university.edu.vn", UserRole::STUDENT, "2026-09-01 08:00:00"};
        users[uStu2.id] = uStu2;
        students[uStu2.id] = Student{uStu2.id, "B22DCCN045", "DEV-SS24-1102", true};

        User uStu3{nextUserId++, "B22DCCN089", "password123", "David Pham", "pham.b22@university.edu.vn", UserRole::STUDENT, "2026-09-01 08:00:00"};
        users[uStu3.id] = uStu3;
        students[uStu3.id] = Student{uStu3.id, "B22DCCN089", "", false};

        User uStu4{nextUserId++, "B22DCCN120", "password123", "David Lee", "lee.b22@university.edu.vn", UserRole::STUDENT, "2026-09-01 08:00:00"};
        users[uStu4.id] = uStu4;
        students[uStu4.id] = Student{uStu4.id, "B22DCCN120", "DEV-PIX8-5521", true};

        // 5. Seed Admin
        User uAdm{nextUserId++, "ADM001", "admin123", "System Administrator", "admin@university.edu.vn", UserRole::ADMIN, "2026-09-01 08:00:00"};
        users[uAdm.id] = uAdm;
        admins[uAdm.id] = Admin{uAdm.id, "ADM001"};

        // 6. Seed Active Attendance Session (Room B204, CS301, 07:00-09:20)
        AttendanceSession sess1{
            nextSessionId++,
            cs301.id,
            b204.id,
            "2026-11-01",
            "07:00",
            "09:20",
            "06:45",
            "07:15",
            "07:45",
            35.0,
            "OPEN"
        };
        sessions[sess1.id] = sess1;

        // 7. NO pre-seeded attendance records.
        // The session roster starts empty on purpose: a student only appears on the
        // lecturer radar after they actually check in through the app. Records are
        // created on demand by processCheckIn().

        // 8. Seed Leave Request matching Wireframe 7 (Ticket #108)
        LeaveRequest lr1{
            108,
            sess1.id,
            uStu4.id,
            "B22DCCN120",
            "David Lee",
            "Medical Illness / Hospitalization",
            "Acute dengue fever; admitted to University Medical Center.",
            "hospital_admission_slip.jpg",
            "2026-11-01 06:30:00",
            uLec.id,
            LeaveStatus::PENDING,
            "",
            ""
        };
        leave_requests[lr1.id] = lr1;
        nextLeaveId = 109; // keep new submissions newer than the seeded ticket #108

        // 9. Seed Security Audit Logs matching Wireframe 8
        audit_logs.push_back(SecurityAuditLog{
            nextAuditId++,
            "2026-11-01 06:55:10",
            "B22DCCN088",
            "DEV-XIAOMI-101",
            "MOCK_LOCATION",
            "Flagged for Mock Location injection provider. Check-in aborted.",
            "CRITICAL"
        });
        audit_logs.push_back(SecurityAuditLog{
            nextAuditId++,
            "2026-11-01 07:01:25",
            "B22DCCN099",
            "DEV-IP15-9921",
            "PROXY_ATTEMPT",
            "Device DEV-IP15-9921 attempted 2nd check-in for B22DCCN099 while registered to Alexander Tran. Blocked.",
            "CRITICAL"
        });
    }

    // --- User & Auth Operations ---
    User* findUserByUsername(const std::string& username) {
        std::lock_guard<std::mutex> lock(mtx);
        for (auto& pair : users) {
            if (pair.second.username == username) {
                return &pair.second;
            }
        }
        return nullptr;
    }

    User* findUserById(int userId) {
        std::lock_guard<std::mutex> lock(mtx);
        auto it = users.find(userId);
        if (it != users.end()) return &it->second;
        return nullptr;
    }

    // Admin creates a new account (student / lecturer / admin). Returns new user id or -1 on failure.
    int createUser(const std::string& username, const std::string& password,
                   const std::string& fullName, const std::string& email,
                   UserRole role, const std::string& code,
                   const std::string& phone = "", const std::string& dateOfBirth = "") {
        std::lock_guard<std::mutex> lock(mtx);

        if (username.empty() || password.empty()) return -1;

        // Duplicate check
        for (const auto& [id, u] : users) {
            if (u.username == username) return -1;
        }

        User u;
        u.id = nextUserId++;
        u.username = username;
        u.password_hash = password;
        u.full_name = fullName;
        u.email = email;
        u.role = role;
        u.created_at = "2026-11-01 08:00:00";
        u.phone = phone;
        u.date_of_birth = dateOfBirth;
        users[u.id] = u;

        if (role == UserRole::STUDENT) {
            // MSSV chinh la ten dang nhap (username) - 1 tai khoan = 1 MSSV.
            students[u.id] = Student{u.id, username, "", false};
        } else if (role == UserRole::LECTURER) {
            lecturers[u.id] = Lecturer{u.id, code, "Department"};
        } else if (role == UserRole::ADMIN) {
            admins[u.id] = Admin{u.id, code};
        }

        audit_logs.push_back(SecurityAuditLog{
            nextAuditId++,
            "2026-11-01 08:00:00",
            username,
            "N/A",
            "ACCOUNT_CREATED",
            "Admin created a new " + roleToString(role) + " account: " + username,
            "INFO"
        });

        persistAll();
        return u.id;
    }

    // Admin deletes an account by username. Returns true when removed.
    bool deleteUser(const std::string& username) {
        std::lock_guard<std::mutex> lock(mtx);

        auto it = users.begin();
        for (; it != users.end(); ++it) {
            if (it->second.username == username) break;
        }
        if (it == users.end()) return false;

        int userId = it->first;
        UserRole role = it->second.role;
        users.erase(it);

        students.erase(userId);
        lecturers.erase(userId);
        admins.erase(userId);

        audit_logs.push_back(SecurityAuditLog{
            nextAuditId++,
            "2026-11-01 08:00:00",
            username,
            "N/A",
            "ACCOUNT_DELETED",
            "Admin deleted " + roleToString(role) + " account: " + username,
            "WARNING"
        });

        persistAll();
        return true;
    }

    // Snapshot of every account, ordered by id (used by the admin dashboard table).
    std::vector<User> getAllUsers() {
        std::lock_guard<std::mutex> lock(mtx);
        std::vector<User> result;
        result.reserve(users.size());
        for (const auto& pair : users) result.push_back(pair.second);
        std::sort(result.begin(), result.end(),
                  [](const User& a, const User& b) { return a.id < b.id; });
        return result;
    }

    Student* findStudentByCode(const std::string& code) {
        std::lock_guard<std::mutex> lock(mtx);
        for (auto& pair : students) {
            if (pair.second.student_code == code) {
                return &pair.second;
            }
        }
        return nullptr;
    }

    Student* findStudentByUserId(int userId) {
        std::lock_guard<std::mutex> lock(mtx);
        auto it = students.find(userId);
        if (it != students.end()) return &it->second;
        return nullptr;
    }

    std::vector<Student> getAllStudents() {
        std::lock_guard<std::mutex> lock(mtx);
        std::vector<Student> result;
        for (const auto& pair : students) result.push_back(pair.second);
        return result;
    }

    bool resetDeviceBinding(const std::string& studentCode) {
        std::lock_guard<std::mutex> lock(mtx);
        for (auto& pair : students) {
            if (pair.second.student_code == studentCode) {
                pair.second.current_device_id = "";
                pair.second.is_device_locked = false;

                audit_logs.push_back(SecurityAuditLog{
                    nextAuditId++,
                    "2026-11-01 08:00:00",
                    studentCode,
                    "N/A",
                    "DEVICE_RESET",
                    "Admin reset hardware binding lock for student " + studentCode,
                    "INFO"
                });
                persistAll();
                return true;
            }
        }
        return false;
    }

    // Returns true if deviceId is already bound to a DIFFERENT student (any session).
    bool isDeviceBoundToAnotherStudent(const std::string& deviceId, const std::string& studentCode) {
        std::lock_guard<std::mutex> lock(mtx);
        return isDeviceBoundToAnotherStudentLocked(deviceId, studentCode);
    }

    // --- Classroom Operations ---
    Classroom* findClassroomById(int id) {
        std::lock_guard<std::mutex> lock(mtx);
        auto it = classrooms.find(id);
        if (it != classrooms.end()) return &it->second;
        return nullptr;
    }

    std::vector<Classroom> getAllClassrooms() {
        std::lock_guard<std::mutex> lock(mtx);
        std::vector<Classroom> list;
        for (const auto& pair : classrooms) list.push_back(pair.second);
        return list;
    }

    bool updateClassroomAnchor(int id, double lat, double lon, double radius) {
        std::lock_guard<std::mutex> lock(mtx);
        auto it = classrooms.find(id);
        if (it != classrooms.end()) {
            it->second.latitude = lat;
            it->second.longitude = lon;
            it->second.default_radius = radius;
            return true;
        }
        return false;
    }

    // --- Session Operations ---
    AttendanceSession* findSessionById(int id) {
        std::lock_guard<std::mutex> lock(mtx);
        auto it = sessions.find(id);
        if (it != sessions.end()) return &it->second;
        return nullptr;
    }

    AttendanceSession* getActiveSession() {
        std::lock_guard<std::mutex> lock(mtx);
        for (auto& pair : sessions) {
            if (pair.second.status == "OPEN") {
                return &pair.second;
            }
        }
        return nullptr;
    }

    bool updateSessionRadius(int sessionId, double newRadius) {
        std::lock_guard<std::mutex> lock(mtx);
        auto it = sessions.find(sessionId);
        if (it != sessions.end()) {
            it->second.allowed_radius = newRadius;
            return true;
        }
        return false;
    }

    // --- Check-in zone (geofence circle) operations ---
    struct GeofenceSnapshot {
        bool found{false};
        int session_id{0};
        std::string room_code;
        std::string building;
        double latitude{0.0};
        double longitude{0.0};
        double radius{0.0};
        std::string session_date;
        std::string start_time;
        std::string status;
    };

    GeofenceSnapshot getGeofence(int sessionId) {
        std::lock_guard<std::mutex> lock(mtx);
        GeofenceSnapshot snap;
        auto it = sessions.find(sessionId);
        if (it == sessions.end()) return snap;
        auto roomIt = classrooms.find(it->second.classroom_id);
        if (roomIt == classrooms.end()) return snap;

        snap.found = true;
        snap.session_id = it->second.id;
        snap.room_code = roomIt->second.room_code;
        snap.building = roomIt->second.building;
        snap.latitude = roomIt->second.latitude;
        snap.longitude = roomIt->second.longitude;
        snap.radius = it->second.allowed_radius;
        snap.session_date = it->second.session_date;
        snap.start_time = it->second.start_time;
        snap.status = it->second.status;
        return snap;
    }

    // Moves the circle: new centre (lat/lng) + new radius for the active session.
    // Caller must already have verified the LECTURER/ADMIN role (ApiRouter does it).
    bool updateSessionGeofence(int sessionId, double lat, double lng, double radius) {
        std::lock_guard<std::mutex> lock(mtx);
        auto it = sessions.find(sessionId);
        if (it == sessions.end()) return false;
        auto roomIt = classrooms.find(it->second.classroom_id);
        if (roomIt == classrooms.end()) return false;

        roomIt->second.latitude = lat;
        roomIt->second.longitude = lng;
        it->second.allowed_radius = radius;
        return true;
    }

    void logGeofenceUpdate(const std::string& actor, double lat, double lng, double radius) {
        std::lock_guard<std::mutex> lock(mtx);
        std::ostringstream ss;
        ss << std::fixed << std::setprecision(6)
           << "Check-in zone changed by " << actor
           << " to centre (" << lat << ", " << lng << ") R=" << std::setprecision(1) << radius << "m.";
        audit_logs.push_back(SecurityAuditLog{
            nextAuditId++,
            "2026-11-01 06:50:00",
            actor,
            "LECTURER_PORTAL",
            "RADIUS_CHANGE",
            ss.str(),
            "INFO"
        });
    }

    bool closeSession(int sessionId) {
        std::lock_guard<std::mutex> lock(mtx);
        auto it = sessions.find(sessionId);
        if (it != sessions.end()) {
            it->second.status = "CLOSED";

            // FR-09: Mark non-attending students as Absent (Unexcused, 0/10)
            for (const auto& sPair : students) {
                bool found = false;
                for (const auto& rPair : records) {
                    if (rPair.second.session_id == sessionId && rPair.second.student_id == sPair.second.user_id) {
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    AttendanceRecord absentRec{
                        nextRecordId++,
                        sessionId,
                        sPair.second.user_id,
                        sPair.second.student_code,
                        users[sPair.second.user_id].full_name,
                        "09:20:00",
                        0.0, 0.0, 999.0, "N/A",
                        AttendanceStatus::ABSENT,
                        0,
                        "Unexcused absence (Auto-locked upon session close)",
                        false
                    };
                    records[absentRec.id] = absentRec;
                }
            }
            return true;
        }
        return false;
    }

    // --- Attendance Check-in Operations ---
    struct CheckInInput {
        int session_id;
        std::string student_code;
        std::string checkin_time;
        double client_lat;
        double client_long;
        double accuracy;
        std::string device_id;
        bool mock_flag{false};
    };

    struct CheckInResponse {
        bool success{false};
        AttendanceStatus status{AttendanceStatus::ABSENT};
        int points{0};
        double distance_meters{0.0};
        std::string remarks;
        std::string error_message;
        bool proxy_attempt{false};  // Device already bound to a different student
        std::string device_id;
    };

    CheckInResponse processCheckIn(const CheckInInput& input) {
        std::lock_guard<std::mutex> lock(mtx);
        CheckInResponse resp;

        auto sessIt = sessions.find(input.session_id);
        if (sessIt == sessions.end() || sessIt->second.status != "OPEN") {
            resp.error_message = "Session is closed or not found.";
            return resp;
        }
        AttendanceSession& sess = sessIt->second;

        auto roomIt = classrooms.find(sess.classroom_id);
        if (roomIt == classrooms.end()) {
            resp.error_message = "Classroom anchor missing.";
            return resp;
        }
        Classroom& room = roomIt->second;

        // Find Student
        Student* stuPtr = nullptr;
        User* userPtr = nullptr;
        for (auto& pair : students) {
            if (pair.second.student_code == input.student_code) {
                stuPtr = &pair.second;
                userPtr = &users[pair.first];
                break;
            }
        }
        if (!stuPtr || !userPtr) {
            resp.error_message = "Student record not found for code: " + input.student_code;
            return resp;
        }

        // 1. Anti-Fraud: Accuracy Validation (NFR-09)
        auto accCheck = AntiFraudService::validateAccuracy(input.accuracy);
        if (!accCheck.is_valid) {
            resp.error_message = accCheck.error_message;
            return resp;
        }

        // 2. Anti-Fraud: Mock Location Detection (NFR-09)
        auto mockCheck = AntiFraudService::checkMockLocation(input.mock_flag, false);
        if (!mockCheck.is_valid) {
            audit_logs.push_back(SecurityAuditLog{
                nextAuditId++,
                sess.session_date + " " + input.checkin_time,
                input.student_code,
                input.device_id,
                "MOCK_LOCATION",
                "Mock location detected during GPS check-in attempt.",
                "CRITICAL"
            });
            resp.error_message = mockCheck.error_message;
            return resp;
        }

        // 3. Anti-Fraud: Device Binding & Anti-Proxy Handset Check (FR-03 & UC21)
        // NEW: 1 thiết bị chỉ được gán với 1 tài khoản trên toàn hệ thống.
        // Nếu thiết bị đã được sinh viên khác sử dụng → chặn là điểm danh hộ.
        if (!input.device_id.empty() && this->isDeviceBoundToAnotherStudentLocked(input.device_id, input.student_code)) {
            audit_logs.push_back(SecurityAuditLog{
                nextAuditId++,
                sess.session_date + " " + input.checkin_time,
                input.student_code,
                input.device_id,
                "PROXY_ATTEMPT",
                "Device " + input.device_id + " is already bound to another student. Proxy check-in attempt blocked.",
                "CRITICAL"
            });
            resp.proxy_attempt = true;
            resp.device_id = input.device_id;
            resp.error_message = "Cố tình điểm danh hộ: Thiết bị " + input.device_id +
                                 " đã được gán với tài khoản sinh viên khác.";
            persistAll();
            return resp;
        }

        std::unordered_map<std::string, std::string> sessionDeviceMap;
        for (const auto& rPair : records) {
            if (rPair.second.session_id == input.session_id && !rPair.second.device_id.empty()) {
                sessionDeviceMap[rPair.second.device_id] = rPair.second.student_code;
            }
        }

        auto deviceCheck = AntiFraudService::verifyDeviceBinding(
            stuPtr->user_id,
            stuPtr->student_code,
            input.device_id,
            stuPtr->current_device_id,
            stuPtr->is_device_locked,
            sessionDeviceMap
        );

        if (!deviceCheck.is_valid) {
            // Two distinct cases can land here:
            //  - the device already checked in for another student this session
            //  - the student's account is locked to a different device
            // Both are proxy / device-misuse attempts and must show a warning.
            auto devIt = sessionDeviceMap.find(input.device_id);
            const bool usedByOtherStudentInSession =
                (devIt != sessionDeviceMap.end() && devIt->second != input.student_code);
            const bool boundToDifferentAccount =
                (!stuPtr->current_device_id.empty() && stuPtr->current_device_id != input.device_id);

            std::string detail = deviceCheck.error_message;
            if (usedByOtherStudentInSession || boundToDifferentAccount) {
                resp.proxy_attempt = true;
                resp.device_id = input.device_id;
                detail = "Cố tình điểm danh hộ: Thiết bị " + input.device_id +
                         " đã được dùng bởi tài khoản sinh viên khác trong buổi điểm danh này.";
            }

            audit_logs.push_back(SecurityAuditLog{
                nextAuditId++,
                sess.session_date + " " + input.checkin_time,
                input.student_code,
                input.device_id,
                "PROXY_ATTEMPT",
                detail,
                "CRITICAL"
            });
            persistAll();
            resp.error_message = detail;
            return resp;
        }

        // If first time binding, lock device
        if (stuPtr->current_device_id.empty()) {
            stuPtr->current_device_id = input.device_id;
            stuPtr->is_device_locked = true;
        }
        persistAll();

        // 4. Calculate Haversine spherical distance d (NFR-04)
        double dist = GeoService::calculateDistance(input.client_lat, input.client_long, room.latitude, room.longitude);
        resp.distance_meters = dist;

        // 5. Apply Business Rules Grading (Section II.2)
        auto grade = AttendanceGradingService::evaluateCheckIn(input.checkin_time, sess.start_time, dist, sess.allowed_radius);
        resp.success = grade.is_success;
        resp.status = grade.status;
        resp.points = grade.points;
        resp.remarks = grade.remarks;

        // 6. Record or update AttendanceRecord
        int existingRecId = 0;
        for (const auto& rPair : records) {
            if (rPair.second.session_id == input.session_id && rPair.second.student_id == stuPtr->user_id) {
                existingRecId = rPair.first;
                break;
            }
        }

        if (existingRecId > 0) {
            records[existingRecId].checkin_time = input.checkin_time;
            records[existingRecId].client_lat = input.client_lat;
            records[existingRecId].client_long = input.client_long;
            records[existingRecId].distance_meters = dist;
            records[existingRecId].device_id = input.device_id;
            records[existingRecId].status = grade.status;
            records[existingRecId].points = grade.points;
            records[existingRecId].remarks = grade.remarks;
        } else {
            AttendanceRecord newRec{
                nextRecordId++,
                input.session_id,
                stuPtr->user_id,
                stuPtr->student_code,
                userPtr->full_name,
                input.checkin_time,
                input.client_lat,
                input.client_long,
                dist,
                input.device_id,
                grade.status,
                grade.points,
                grade.remarks,
                false
            };
            records[newRec.id] = newRec;
        }

        persistAll();
        return resp;
    }

    // --- Leave Requests Operations ---
    int submitLeaveRequest(int sessionId, const std::string& studentCode, const std::string& category, const std::string& explanation, const std::string& fileName, long long fileSize = 0) {
        std::lock_guard<std::mutex> lock(mtx);
        Student* stu = nullptr;
        User* user = nullptr;
        for (auto& pair : students) {
            if (pair.second.student_code == studentCode) {
                stu = &pair.second;
                user = &users[pair.first];
                break;
            }
        }
        if (!stu || !user) return -1;

        LeaveRequest req{
            nextLeaveId++,
            sessionId,
            stu->user_id,
            stu->student_code,
            user->full_name,
            category,
            explanation,
            fileName,
            "2026-11-01 07:10:00",
            0,
            LeaveStatus::PENDING,
            "",
            "",
            fileSize
        };
        leave_requests[req.id] = req;
        return req.id;
    }

    bool adjudicateLeaveRequest(int requestId, bool isApproved, const std::string& feedbackNotes, int reviewerId) {
        std::lock_guard<std::mutex> lock(mtx);
        auto it = leave_requests.find(requestId);
        if (it == leave_requests.end()) return false;

        it->second.review_status = isApproved ? LeaveStatus::APPROVED : LeaveStatus::REJECTED;
        it->second.review_notes = feedbackNotes;
        it->second.reviewer_id = reviewerId;
        it->second.reviewed_at = "2026-11-01 07:30:00";

        // Update corresponding AttendanceRecord
        auto grade = AttendanceGradingService::evaluateLeaveAdjudication(isApproved, it->second.reason_category);

        bool updated = false;
        for (auto& rPair : records) {
            if (rPair.second.session_id == it->second.session_id && rPair.second.student_id == it->second.student_id) {
                rPair.second.status = grade.status;
                rPair.second.points = grade.points;
                rPair.second.remarks = grade.remarks;
                updated = true;
                break;
            }
        }

        if (!updated) {
            AttendanceRecord rec{
                nextRecordId++,
                it->second.session_id,
                it->second.student_id,
                it->second.student_code,
                it->second.student_name,
                "07:30:00",
                0.0, 0.0, 0.0, "EXCUSE_PORTAL",
                grade.status,
                grade.points,
                grade.remarks,
                false
            };
            records[rec.id] = rec;
        }

        return true;
    }

    std::vector<LeaveRequest> getPendingLeaveRequests() {
        std::lock_guard<std::mutex> lock(mtx);
        std::vector<LeaveRequest> list;
        for (const auto& pair : leave_requests) {
            if (pair.second.review_status == LeaveStatus::PENDING) {
                list.push_back(pair.second);
            }
        }
        return list;
    }

    // --- Record Adjustments & Querying ---
    bool manualOverrideRecord(int recordId, AttendanceStatus newStatus, int newPoints, const std::string& reason) {
        std::lock_guard<std::mutex> lock(mtx);
        auto it = records.find(recordId);
        if (it != records.end()) {
            it->second.status = newStatus;
            it->second.points = newPoints;
            it->second.remarks = "Manual override by Lecturer: " + reason;
            it->second.is_manual_override = true;
            return true;
        }
        return false;
    }

    std::vector<AttendanceRecord> getSessionRecords(int sessionId) {
        std::lock_guard<std::mutex> lock(mtx);
        std::vector<AttendanceRecord> list;
        for (const auto& pair : records) {
            if (pair.second.session_id == sessionId) {
                list.push_back(pair.second);
            }
        }
        return list;
    }

    std::vector<AttendanceRecord> getStudentHistory(const std::string& studentCode) {
        std::lock_guard<std::mutex> lock(mtx);
        std::vector<AttendanceRecord> list;
        for (const auto& pair : records) {
            if (pair.second.student_code == studentCode) {
                list.push_back(pair.second);
            }
        }
        return list;
    }

    std::vector<SecurityAuditLog> getAuditLogs() {
        std::lock_guard<std::mutex> lock(mtx);
        return audit_logs;
    }

    // ---- Device bindings & device-reset requests (admin / student) ----
    struct DeviceBindingInfo {
        int user_id{0};
        std::string student_code;
        std::string full_name;
        std::string device_id;
        bool locked{false};
    };

    std::vector<DeviceBindingInfo> getDeviceBindings() {
        std::lock_guard<std::mutex> lock(mtx);
        std::vector<DeviceBindingInfo> list;
        for (const auto& pair : students) {
            DeviceBindingInfo info;
            info.user_id = pair.second.user_id;
            info.student_code = pair.second.student_code;
            info.device_id = pair.second.current_device_id;
            info.locked = pair.second.is_device_locked;
            auto uIt = users.find(pair.second.user_id);
            if (uIt != users.end()) info.full_name = uIt->second.full_name;
            list.push_back(info);
        }
        std::sort(list.begin(), list.end(),
                  [](const DeviceBindingInfo& a, const DeviceBindingInfo& b) {
                      return a.student_code < b.student_code;
                  });
        return list;
    }

    int submitDeviceResetRequest(const std::string& studentCode, const std::string& reason,
                                 const std::string& explanation, const std::string& fileName,
                                 long long fileSize) {
        std::lock_guard<std::mutex> lock(mtx);
        Student* stu = nullptr;
        User* user = nullptr;
        for (auto& pair : students) {
            if (pair.second.student_code == studentCode) {
                stu = &pair.second;
                auto uIt = users.find(pair.first);
                if (uIt != users.end()) user = &uIt->second;
                break;
            }
        }
        if (!stu || !user) return -1;

        DeviceResetRequest req;
        req.id = nextDeviceResetId++;
        req.user_id = stu->user_id;
        req.student_code = studentCode;
        req.student_name = user->full_name;
        req.reason = reason;
        req.explanation = explanation;
        req.file_name = fileName;
        req.file_size = fileSize;
        req.status = "PENDING";
        req.created_at = "2026-11-01 07:10:00";
        device_reset_requests.push_back(req);
        persistAll();
        return req.id;
    }

    std::vector<DeviceResetRequest> getPendingDeviceResetRequests() {
        std::lock_guard<std::mutex> lock(mtx);
        std::vector<DeviceResetRequest> list;
        for (const auto& r : device_reset_requests) {
            if (r.status == "PENDING") list.push_back(r);
        }
        std::sort(list.begin(), list.end(),
                  [](const DeviceResetRequest& a, const DeviceResetRequest& b) { return a.id > b.id; });
        return list;
    }

    std::vector<DeviceResetRequest> getDeviceResetRequestsForStudent(const std::string& studentCode) {
        std::lock_guard<std::mutex> lock(mtx);
        std::vector<DeviceResetRequest> list;
        for (const auto& r : device_reset_requests) {
            if (r.student_code == studentCode) list.push_back(r);
        }
        std::sort(list.begin(), list.end(),
                  [](const DeviceResetRequest& a, const DeviceResetRequest& b) { return a.id > b.id; });
        return list;
    }

    bool reviewDeviceResetRequest(int requestId, bool approve, const std::string& reviewerNote) {
        std::lock_guard<std::mutex> lock(mtx);
        DeviceResetRequest* found = nullptr;
        for (auto& r : device_reset_requests) {
            if (r.id == requestId) { found = &r; break; }
        }
        if (!found || found->status != "PENDING") return false;

        found->status = approve ? "APPROVED" : "REJECTED";
        found->reviewer_note = reviewerNote;

        if (approve) {
            for (auto& pair : students) {
                if (pair.second.student_code == found->student_code) {
                    pair.second.current_device_id = "";
                    pair.second.is_device_locked = false;
                    break;
                }
            }
        }

        audit_logs.push_back(SecurityAuditLog{
            nextAuditId++,
            "2026-11-01 08:00:00",
            found->student_code,
            approve ? "" : "N/A",
            approve ? "DEVICE_RESET" : "DEVICE_RESET_REJECTED",
            std::string("Device reset request #") + std::to_string(requestId) +
                (approve ? " approved by admin. Device lock cleared." : " rejected by admin."),
            approve ? "INFO" : "WARNING"
        });
        persistAll();
        return true;
    }
};

} // namespace attendance
