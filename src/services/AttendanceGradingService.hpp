#pragma once
#include <string>
#include <sstream>
#include <iomanip>
#include "../models/Models.hpp"

namespace attendance {

struct GradingResult {
    AttendanceStatus status{AttendanceStatus::ABSENT};
    int points{0};
    std::string remarks;
    bool is_success{false};
};

class AttendanceGradingService {
public:
    static int timeToMinutes(const std::string& timeStr) {
        // Expected format "HH:MM" or "HH:MM:SS"
        int h = 0, m = 0;
        char colon = ':';
        std::istringstream ss(timeStr);
        if (ss >> h >> colon >> m) {
            return h * 60 + m;
        }
        return 0;
    }

    static std::string minutesToTime(int minutes) {
        int h = (minutes / 60) % 24;
        int m = minutes % 60;
        std::ostringstream ss;
        ss << std::setfill('0') << std::setw(2) << h << ":"
           << std::setfill('0') << std::setw(2) << m;
        return ss.str();
    }

    /**
     * Evaluates attendance status & points based on Section II.2 Core Business Rules
     */
    static GradingResult evaluateCheckIn(const std::string& checkinTime, 
                                        const std::string& startTime,
                                        double distanceMeters,
                                        double allowedRadiusMeters) {
        GradingResult result;
        int tCheckin = timeToMinutes(checkinTime);
        int tStart = timeToMinutes(startTime);

        int tEarlyStart = tStart - 15;
        int tOnTimeEnd  = tStart + 15;
        int tLateEnd    = tStart + 45;

        // Condition 1: Outside Geofence
        if (distanceMeters > allowedRadiusMeters) {
            result.status = AttendanceStatus::REJECTED;
            result.points = 0;
            std::ostringstream ss;
            ss << "Error: Outside classroom range (" << std::fixed << std::setprecision(1) << distanceMeters 
               << "m > " << allowedRadiusMeters << "m)";
            result.remarks = ss.str();
            result.is_success = false;
            return result;
        }

        // Condition 2: Too early - the window opens 15 minutes before class
        if (tCheckin < tEarlyStart) {
            result.status = AttendanceStatus::REJECTED;
            result.points = 0;
            result.remarks = "Rejected: check-in window opens at " + minutesToTime(tEarlyStart) +
                             " (15 minutes before class starts at " + minutesToTime(tStart) + ")";
            result.is_success = false;
            return result;
        }

        // Inside geofence:
        // Window 1: [T_start - 15, T_start + 15]
        if (tCheckin >= tEarlyStart && tCheckin <= tOnTimeEnd) {
            result.status = AttendanceStatus::PRESENT;
            result.points = 10;
            result.remarks = "Self-recorded via GPS (On-time)";
            result.is_success = true;
            return result;
        }

        // Window 2: [T_start + 16, T_start + 45]
        if (tCheckin > tOnTimeEnd && tCheckin <= tLateEnd) {
            result.status = AttendanceStatus::LATE;
            result.points = 5;
            int lateMins = tCheckin - tStart;
            result.remarks = "Self-recorded via GPS (Late: " + std::to_string(lateMins) + " mins)";
            result.is_success = true;
            return result;
        }

        // Window 3: After minute 45
        result.status = AttendanceStatus::ABSENT;
        result.points = 0;
        result.remarks = "Unexcused absence (Auto-locked: check-in window expired)";
        result.is_success = false;
        return result;
    }

    /**
     * Evaluates Leave Request adjudication points (2 pts for approved, 0 pts for rejected)
     */
    static GradingResult evaluateLeaveAdjudication(bool isApproved, const std::string& reasonCategory) {
        GradingResult result;
        if (isApproved) {
            result.status = AttendanceStatus::EXCUSED;
            result.points = 2;
            result.remarks = "Excused absence (Approved: " + reasonCategory + ")";
            result.is_success = true;
        } else {
            result.status = AttendanceStatus::ABSENT;
            result.points = 0;
            result.remarks = "Rejected absence (Declined: " + reasonCategory + ")";
            result.is_success = false;
        }
        return result;
    }
};

} // namespace attendance
