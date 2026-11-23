#pragma once
#include <string>
#include <vector>
#include <chrono>

namespace attendance {

enum class UserRole {
    STUDENT,
    LECTURER,
    ADMIN
};

inline std::string roleToString(UserRole role) {
    switch (role) {
        case UserRole::STUDENT: return "STUDENT";
        case UserRole::LECTURER: return "LECTURER";
        case UserRole::ADMIN: return "ADMIN";
    }
    return "UNKNOWN";
}

inline UserRole stringToRole(const std::string& str) {
    if (str == "STUDENT") return UserRole::STUDENT;
    if (str == "LECTURER") return UserRole::LECTURER;
    if (str == "ADMIN") return UserRole::ADMIN;
    return UserRole::STUDENT;
}

enum class AttendanceStatus {
    PRESENT,
    LATE,
    ABSENT,
    EXCUSED,
    REJECTED
};

inline std::string statusToString(AttendanceStatus status) {
    switch (status) {
        case AttendanceStatus::PRESENT: return "PRESENT";
        case AttendanceStatus::LATE: return "LATE";
        case AttendanceStatus::ABSENT: return "ABSENT";
        case AttendanceStatus::EXCUSED: return "EXCUSED";
        case AttendanceStatus::REJECTED: return "REJECTED";
    }
    return "UNKNOWN";
}

enum class LeaveStatus {
    PENDING,
    APPROVED,
    REJECTED
};

inline std::string leaveStatusToString(LeaveStatus status) {
    switch (status) {
        case LeaveStatus::PENDING: return "PENDING";
        case LeaveStatus::APPROVED: return "APPROVED";
        case LeaveStatus::REJECTED: return "REJECTED";
    }
    return "PENDING";
}

// 1. User base model
struct User {
    int id{0};
    std::string username;
    std::string password_hash;
    std::string full_name;
    std::string email;
    UserRole role{UserRole::STUDENT};
    std::string created_at;
    std::string phone;         // So dien thoai lien he
    std::string date_of_birth; // dd/mm/yyyy
};

// 2. Student model (extends User)
struct Student {
    int user_id{0};
    std::string student_code; // MSSV, e.g. "B22DCCN001"
    std::string current_device_id;
    bool is_device_locked{false};
};

// 3. Lecturer model (extends User)
struct Lecturer {
    int user_id{0};
    std::string lecturer_code; // MGV, e.g. "GV101"
    std::string department;
};

// 4. Admin model (extends User)
struct Admin {
    int user_id{0};
    std::string admin_code; // e.g. "ADM001"
};

// 5. Classroom model with GPS Anchor
struct Classroom {
    int id{0};
    std::string room_code; // e.g. "B204"
    std::string building;  // e.g. "Bldg B"
    double latitude{0.0};  // e.g. 10.823098
    double longitude{0.0}; // e.g. 106.629663
    double default_radius{35.0}; // in meters
};

// 6. CourseClass model
struct CourseClass {
    int id{0};
    std::string class_code; // e.g. "CS301-G01"
    std::string course_name; // e.g. "[CS301] Software Engineering"
    int lecturer_id{0};
    std::string semester; // e.g. "Fall 2026"
    std::string academic_year; // e.g. "2026-2027"
};

// 7. AttendanceSession model
struct AttendanceSession {
    int id{0};
    int course_class_id{0};
    int classroom_id{0};
    std::string session_date; // "2026-11-01"
    std::string start_time;   // "07:00"
    std::string end_time;     // "09:20"
    std::string early_checkin_start; // "06:45" (T_start - 15)
    std::string late_threshold_time; // "07:15" (T_start + 15)
    std::string absent_threshold_time; // "07:45" (T_start + 45)
    double allowed_radius{35.0};
    std::string status{"OPEN"}; // "OPEN", "CLOSED", "SCHEDULED"
};

// 8. AttendanceRecord model
struct AttendanceRecord {
    int id{0};
    int session_id{0};
    int student_id{0};
    std::string student_code;
    std::string student_name;
    std::string checkin_time;
    double client_lat{0.0};
    double client_long{0.0};
    double distance_meters{0.0};
    std::string device_id;
    AttendanceStatus status{AttendanceStatus::ABSENT};
    int points{0}; // 10, 5, 2, 0
    std::string remarks;
    bool is_manual_override{false};
};

// 9. LeaveRequest model
struct LeaveRequest {
    int id{0};
    int session_id{0};
    int student_id{0};
    std::string student_code;
    std::string student_name;
    std::string reason_category; // "Medical Illness", "Family Emergency", etc.
    std::string reason_detail;
    std::string evidence_file_url;
    std::string submitted_at;
    int reviewer_id{0};
    LeaveStatus review_status{LeaveStatus::PENDING};
    std::string review_notes;
    std::string reviewed_at;
    long long evidence_file_size{0}; // size of the uploaded proof file in bytes (0 = unknown)
};

// 10. SecurityAuditLog model
struct SecurityAuditLog {
    int id{0};
    std::string timestamp;
    std::string student_code;
    std::string device_id;
    std::string event_type; // "MOCK_LOCATION", "PROXY_ATTEMPT", "DEVICE_RESET", "RADIUS_CHANGE"
    std::string details;
    std::string severity; // "CRITICAL", "WARNING", "INFO"
};

// 11. DeviceResetRequest: a student asks the admin to unbind a lost/old device.
//     Like a leave request, it must carry an image proof (JPG/PNG).
struct DeviceResetRequest {
    int id{0};
    int user_id{0};
    std::string student_code;
    std::string student_name;
    std::string reason;
    std::string explanation;
    std::string file_name;
    long long file_size{0};
    std::string status{"PENDING"};   // PENDING / APPROVED / REJECTED
    std::string created_at;
    std::string reviewer_note;
};

} // namespace attendance
