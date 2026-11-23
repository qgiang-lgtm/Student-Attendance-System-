#pragma once
#include <string>
#include <memory>
#include "../models/Models.hpp"

namespace attendance {

struct AuthResult {
    bool success{false};
    std::string token;
    UserRole effective_role{UserRole::STUDENT};
    std::string user_code;
    std::string full_name;
    std::string redirect_url;
    std::string error_message;
};

class AuthService {
public:
    /**
     * FR-01 & FR-02: Authenticates credential and smart routes Admin regardless of tab chosen
     */
    static AuthResult authenticate(const User& user, const std::string& inputPassword, const std::string& selectedTab) {
        AuthResult res;

        // In production this verifies bcrypt/Argon2; for our simulation we verify hash/match
        if (user.password_hash != inputPassword && user.password_hash != "hashed_" + inputPassword) {
            res.success = false;
            res.error_message = "Invalid identification code or password.";
            return res;
        }

        res.success = true;
        res.full_name = user.full_name;
        res.effective_role = user.role;

        // Generate session token (e.g. "AUTH_TK_<role>_<id>_<timestamp>")
        res.token = "TOKEN_" + roleToString(user.role) + "_" + std::to_string(user.id);

        // FR-02: Universal Admin Auto-Routing:
        // When role is 'ADMIN', bypass selected tab and redirect to Admin Dashboard
        if (user.role == UserRole::ADMIN) {
            res.redirect_url = "/admin-dashboard";
            return res;
        }

        // Student Tab
        if (user.role == UserRole::STUDENT) {
            if (selectedTab == "Lecturer") {
                // Warning or cross-tab auto-dispatch
                res.redirect_url = "/student-dashboard";
            } else {
                res.redirect_url = "/student-dashboard";
            }
            return res;
        }

        // Lecturer Tab
        if (user.role == UserRole::LECTURER) {
            res.redirect_url = "/lecturer-dashboard";
            return res;
        }

        return res;
    }
};

} // namespace attendance
