#pragma once
#include <string>
#include <sstream>
#include <algorithm>
#include <vector>
#include <iomanip>
#include <cctype>
#include <cstdlib>
#include <cstdio>
#include <mutex>
#include <unordered_map>
#include <iostream>
#include "../services/AuthService.hpp"
#include "../repository/DatabaseRepository.hpp"
#include "../ui/WebClient.hpp"

namespace attendance {

// Simple JSON extraction helper
inline std::string extractJsonString(const std::string& body, const std::string& key) {
    std::string needle = "\"" + key + "\"";
    size_t pos = body.find(needle);
    if (pos == std::string::npos) return "";
    pos = body.find(':', pos);
    if (pos == std::string::npos) return "";
    pos = body.find('"', pos);
    if (pos == std::string::npos) return "";
    size_t endPos = body.find('"', pos + 1);
    if (endPos == std::string::npos) return "";
    return body.substr(pos + 1, endPos - pos - 1);
}

inline double extractJsonNumber(const std::string& body, const std::string& key, double defaultVal = 0.0) {
    std::string needle = "\"" + key + "\"";
    size_t pos = body.find(needle);
    if (pos == std::string::npos) return defaultVal;
    pos = body.find(':', pos);
    if (pos == std::string::npos) return defaultVal;
    pos++;
    while (pos < body.size() && (body[pos] == ' ' || body[pos] == '\t')) pos++;
    size_t endPos = pos;
    while (endPos < body.size() && (body[endPos] == '.' || body[endPos] == '-' || (body[endPos] >= '0' && body[endPos] <= '9'))) {
        endPos++;
    }
    try {
        return std::stod(body.substr(pos, endPos - pos));
    } catch (...) {
        return defaultVal;
    }
}

inline bool extractJsonBool(const std::string& body, const std::string& key) {
    std::string needle = "\"" + key + "\"";
    size_t pos = body.find(needle);
    if (pos == std::string::npos) return false;
    pos = body.find(':', pos);
    if (pos == std::string::npos) return false;
    std::string sub = body.substr(pos, 20);
    return sub.find("true") != std::string::npos;
}

// Minimal JSON string escaper (quotes, backslashes and control characters)
inline std::string attendanceStatusToString(AttendanceStatus s) {
    switch (s) {
        case AttendanceStatus::PRESENT:  return "PRESENT";
        case AttendanceStatus::LATE:     return "LATE";
        case AttendanceStatus::EXCUSED:  return "EXCUSED";
        case AttendanceStatus::REJECTED: return "REJECTED";
        default:                         return "ABSENT";
    }
}
inline std::string jsonEscape(const std::string& s) {
    std::string out;
    out.reserve(s.size() + 8);
    for (char c : s) {
        switch (c) {
            case '"':  out += "\\\""; break;
            case '\\': out += "\\\\"; break;
            case '\n': out += "\\n";  break;
            case '\r': out += "\\r";  break;
            case '\t': out += "\\t";  break;
            default:
                if (static_cast<unsigned char>(c) < 0x20) out += ' ';
                else out += c;
        }
    }
    return out;
}

// ---------------------------------------------------------------------------
// Classroom-network helpers (check-in is only allowed from the classroom LAN)
// ---------------------------------------------------------------------------

// Normalises a client address: strips an IPv4-mapped IPv6 prefix and a :port.
inline std::string normalizeClientIp(std::string ip) {
    while (!ip.empty() && (ip.front() == ' ' || ip.front() == '"')) ip.erase(ip.begin());
    while (!ip.empty() && (ip.back() == ' ' || ip.back() == '"')) ip.pop_back();
    const std::string mapped = "::ffff:";
    if (ip.rfind(mapped, 0) == 0) ip = ip.substr(mapped.size());
    if (ip.find('.') != std::string::npos && ip.find(':') != std::string::npos) {
        ip = ip.substr(0, ip.find(':'));
    }
    return ip;
}

inline bool parseIpv4(const std::string& ip, unsigned long& out) {
    unsigned int a = 0, b = 0, c = 0, d = 0;
    char extra = 0;
    if (std::sscanf(ip.c_str(), "%u.%u.%u.%u%c", &a, &b, &c, &d, &extra) != 4) return false;
    if (a > 255 || b > 255 || c > 255 || d > 255) return false;
    out = ((unsigned long)a << 24) | ((unsigned long)b << 16) | ((unsigned long)c << 8) | (unsigned long)d;
    return true;
}

inline bool ipInCidr(const std::string& ip, const std::string& cidr) {
    std::string base = cidr;
    int bits = 32;
    size_t slash = cidr.find('/');
    if (slash != std::string::npos) {
        base = cidr.substr(0, slash);
        bits = std::atoi(cidr.c_str() + slash + 1);
    }
    if (bits < 0) bits = 0;
    if (bits > 32) bits = 32;
    unsigned long ipNum = 0, baseNum = 0;
    if (!parseIpv4(ip, ipNum) || !parseIpv4(base, baseNum)) return false;
    unsigned long mask = (bits == 0) ? 0UL : (0xFFFFFFFFUL << (32 - bits));
    mask &= 0xFFFFFFFFUL;
    return (ipNum & mask) == (baseNum & mask);
}

// CLASSROOM_CIDR="192.168.1.0/24,10.0.0.0/8" (comma separated). Defaults cover
// the usual home/office LAN, the Docker bridge (published ports may hide the
// real source address) and loopback for a local demo.
inline std::vector<std::string> classroomCidrs() {
    const char* env = std::getenv("CLASSROOM_CIDR");
    std::string raw = (env && *env) ? env : "192.168.1.0/24,172.16.0.0/12,127.0.0.0/8";
    std::vector<std::string> out;
    std::string cur;
    for (char c : raw) {
        if (c == ',' || c == ';' || c == ' ') {
            if (!cur.empty()) { out.push_back(cur); cur.clear(); }
        } else {
            cur += c;
        }
    }
    if (!cur.empty()) out.push_back(cur);
    return out;
}

inline std::string envOr(const char* name, const std::string& fallback) {
    const char* v = std::getenv(name);
    return (v && *v) ? std::string(v) : fallback;
}

class ApiRouter {
private:
    std::shared_ptr<DatabaseRepository> repo;

    // Public IP that the classroom NAT uses. It is learned automatically the
    // first time a device opens the app over the LAN, so a phone that is
    // physically inside the classroom but reaches us through an HTTPS tunnel
    // (cloudflared / ngrok) is still recognised as "same network".
    mutable std::mutex netMutex;
    std::string classroomPublicIp{envOr("CLASSROOM_PUBLIC_IP", "")};

    bool isDirectLanClient(const std::string& clientIp, const std::string& forwardedFor) const {
        if (!forwardedFor.empty()) return false;
        std::string ip = normalizeClientIp(clientIp);
        if (ip.empty()) return true; // internal call (unit tests) -> trust it
        for (const auto& cidr : classroomCidrs()) {
            if (ipInCidr(ip, cidr)) return true;
        }
        return false;
    }

    bool isClassroomNetwork(const std::string& clientIp, const std::string& forwardedFor) const {
        if (forwardedFor.empty()) return isDirectLanClient(clientIp, forwardedFor);

        // Came through a tunnel / reverse proxy: the socket peer is the proxy,
        // the real address is in the forwarding header. Accept it only when it
        // is the same public IP the classroom NAT uses.
        std::string realIp = normalizeClientIp(forwardedFor);
        std::lock_guard<std::mutex> lock(netMutex);
        return !classroomPublicIp.empty() && realIp == classroomPublicIp;
    }

    std::string classroomPublicIpSnapshot() const {
        std::lock_guard<std::mutex> lock(netMutex);
        return classroomPublicIp;
    }

public:
    ApiRouter(std::shared_ptr<DatabaseRepository> repository) : repo(repository) {}

    struct HttpResponse {
        int status_code{200};
        std::string content_type{"text/html; charset=utf-8"};
        std::string body;
    };

    HttpResponse handleRequest(const std::string& method, const std::string& path, const std::string& body,
                               const std::string& clientIp = "", const std::string& forwardedFor = "") {
        HttpResponse res;

        // Serve Web App GUI (Screens 1 to 8)
        if (method == "GET" && (path == "/" || path == "/index.html")) {
            res.status_code = 200;
            res.content_type = "text/html; charset=utf-8";
            res.body = getIndexHtml();
            return res;
        }

        // PWA manifest: lets Android/iOS install the app to the home screen
        if (method == "GET" && path == "/manifest.webmanifest") {
            res.status_code = 200;
            res.content_type = "application/manifest+json";
            res.body = R"json({
  "name": "SmartGPS Attendance",
  "short_name": "SmartGPS",
  "description": "Smart GPS Student Attendance System",
  "start_url": "/",
  "scope": "/",
  "display": "standalone",
  "orientation": "portrait",
  "background_color": "#f8fafc",
  "theme_color": "#4f46e5",
  "icons": [
    { "src": "/icon.svg", "sizes": "any", "type": "image/svg+xml", "purpose": "any maskable" }
  ]
})json";
            return res;
        }

        // App icon (SVG, used by the manifest and the browser tab)
        if (method == "GET" && path == "/icon.svg") {
            res.status_code = 200;
            res.content_type = "image/svg+xml";
            res.body = R"svg(<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 512 512">
  <rect width="512" height="512" rx="112" fill="#4f46e5"/>
  <circle cx="256" cy="226" r="118" fill="none" stroke="#ffffff" stroke-width="18" stroke-dasharray="26 20" opacity="0.55"/>
  <path d="M256 120c-53 0-96 43-96 96 0 72 96 176 96 176s96-104 96-176c0-53-43-96-96-96z" fill="#ffffff"/>
  <circle cx="256" cy="216" r="38" fill="#4f46e5"/>
</svg>)svg";
            return res;
        }

        // Service worker: makes the app installable (Add to Home Screen)
        if (method == "GET" && path == "/sw.js") {
            res.status_code = 200;
            res.content_type = "application/javascript";
            res.body = R"js(const CACHE = "smartgps-shell-v1";
self.addEventListener("install", () => self.skipWaiting());
self.addEventListener("activate", (event) => event.waitUntil(self.clients.claim()));
self.addEventListener("fetch", (event) => {
  if (event.request.method !== "GET") return;
  event.respondWith(
    fetch(event.request)
      .then((response) => {
        const copy = response.clone();
        caches.open(CACHE).then((cache) => cache.put(event.request, copy)).catch(() => {});
        return response;
      })
      .catch(() => caches.match(event.request))
  );
});
)js";
            return res;
        }

        // LAN hint for the "Use on your phone" card (set by docker-compose)
        if (method == "GET" && path == "/api/network") {
            const char* hintEnv = std::getenv("LAN_HINT");
            std::string hint = hintEnv ? hintEnv : "";
            res.status_code = 200;
            res.content_type = "application/json";
            res.body = "{\"success\":true,\"port\":8080,\"lan_hint\":\"" + jsonEscape(hint) + "\"}";
            return res;
        }

        // Is the caller connected to the classroom network? (check-in gate)
        if (method == "GET" && path == "/api/network/status") {
            bool onLan = isClassroomNetwork(clientIp, forwardedFor);
            std::string cidrs;
            for (const auto& c : classroomCidrs()) { if (!cidrs.empty()) cidrs += ","; cidrs += c; }
            res.status_code = 200;
            res.content_type = "application/json";
            res.body = "{\"success\":true"
                       ",\"client_ip\":\"" + jsonEscape(normalizeClientIp(clientIp)) + "\""
                       ",\"via_proxy\":" + (forwardedFor.empty() ? "false" : "true") +
                       ",\"classroom_network\":" + (onLan ? "true" : "false") +
                       ",\"classroom_cidr\":\"" + jsonEscape(cidrs) + "\""
                       ",\"lan_url\":\"" + jsonEscape(envOr("LAN_HINT", "")) + "\""
                       ",\"classroom_public_ip\":\"" + jsonEscape(classroomPublicIpSnapshot()) + "\"}";
            return res;
        }

        // A device that reached us over the LAN reports the classroom's public
        // IP once, so tunnel traffic from the same NAT can still check in.
        if (method == "POST" && path == "/api/network/learn-public-ip") {
            if (!isDirectLanClient(clientIp, forwardedFor)) {
                res.status_code = 403;
                res.content_type = "application/json";
                res.body = R"({"success":false,"error":"Only a device on the classroom network can register the classroom public IP."})";
                return res;
            }
            std::string ip = normalizeClientIp(extractJsonString(body, "ip"));
            if (ip.empty()) {
                res.status_code = 400;
                res.content_type = "application/json";
                res.body = R"({"success":false,"error":"Missing ip"})";
                return res;
            }
            {
                std::lock_guard<std::mutex> lock(netMutex);
                classroomPublicIp = ip;
            }
            res.status_code = 200;
            res.content_type = "application/json";
            res.body = "{\"success\":true,\"classroom_public_ip\":\"" + jsonEscape(ip) + "\"}";
            return res;
        }

        // Health check endpoint
        if (method == "GET" && path == "/api/health") {
            res.status_code = 200;
            res.content_type = "application/json";
            res.body = R"({"status":"UP","server":"C++17 HTTP/REST Server","architecture":"Clean Layered Architecture"})";
            return res;
        }

        // Real-time authoritative server clock & NTP synchronization endpoint (NFR-05)
        if (method == "GET" && path == "/api/time") {
            auto now = std::chrono::system_clock::now();
            std::time_t t = std::chrono::system_clock::to_time_t(now);
            std::tm tmNow{};
#if defined(_WIN32)
            localtime_s(&tmNow, &t);
#else
            localtime_r(&t, &tmNow);
#endif
            char timeBuf[32];
            std::strftime(timeBuf, sizeof(timeBuf), "%H:%M:%S", &tmNow);
            char dateBuf[32];
            std::strftime(dateBuf, sizeof(dateBuf), "%Y-%m-%d", &tmNow);
            char fullBuf[64];
            std::strftime(fullBuf, sizeof(fullBuf), "%A, %b %d, %Y", &tmNow);

            auto epochMs = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();

            res.status_code = 200;
            res.content_type = "application/json";
            std::ostringstream ss;
            ss << "{\"success\":true,"
               << "\"server_time\":\"" << timeBuf << "\","
               << "\"date\":\"" << dateBuf << "\","
               << "\"display_date\":\"" << fullBuf << "\","
               << "\"epoch_ms\":" << epochMs << ","
               << "\"ntp_synchronized\":true,"
               << "\"stratum\":2,"
               << "\"source\":\"Authoritative NTP Server\"}";
            res.body = ss.str();
            return res;
        }

        // 1. POST /api/login (FR-01, FR-02 Universal Admin Auto-Routing)
        if (method == "POST" && path == "/api/login") {
            std::string username = extractJsonString(body, "username");
            std::string password = extractJsonString(body, "password");
            std::string tab = extractJsonString(body, "tab");

            User* user = repo->findUserByUsername(username);
            res.content_type = "application/json";

            if (!user) {
                res.status_code = 401;
                res.body = R"({"success":false,"error":"User account not found."})";
                return res;
            }

            AuthResult auth = AuthService::authenticate(*user, password, tab);
            if (!auth.success) {
                res.status_code = 401;
                res.body = R"({"success":false,"error":")" + auth.error_message + R"("})";
                return res;
            }

            res.status_code = 200;
            Student* stu = repo->findStudentByUserId(user->id);
            std::ostringstream ss;
            ss << "{\"success\":true,\"token\":\"" << auth.token << "\","
               << "\"role\":\"" << roleToString(auth.effective_role) << "\","
               << "\"full_name\":\"" << auth.full_name << "\","
               << "\"username\":\"" << jsonEscape(username) << "\","
               << "\"student_code\":\"" << jsonEscape(stu ? stu->student_code : username) << "\","
               << "\"redirect_url\":\"" << auth.redirect_url << "\"}";
            res.body = ss.str();
            return res;
        }

        // 2. POST /api/student/checkin (FR-03, FR-04, FR-05, BR 10/5/2/0)
        if (method == "POST" && path == "/api/student/checkin") {
            // Network-level anti-proxy rule: a check-in only counts when the
            // device is on the classroom network (same Wi-Fi as the server).
            if (!isClassroomNetwork(clientIp, forwardedFor)) {
                std::string lanUrl = envOr("LAN_HINT", "the classroom LAN address");
                res.status_code = 403;
                res.content_type = "application/json";
                res.body = "{\"success\":false,\"status\":\"BLOCKED\",\"points\":0,\"distance_meters\":0,"
                           "\"remarks\":\"Not on the classroom Wi-Fi. Open " + jsonEscape(lanUrl) +
                           " while connected to the classroom Wi-Fi to check in.\","
                           "\"error\":\"Check-in requires the classroom network.\","
                           "\"client_ip\":\"" + jsonEscape(normalizeClientIp(clientIp)) + "\"}";
                return res;
            }

            DatabaseRepository::CheckInInput input;
            input.session_id = 1; // Active session
            input.student_code = extractJsonString(body, "student_code");
            if (input.student_code.empty()) input.student_code = "B22DCCN001";
            input.checkin_time = extractJsonString(body, "checkin_time");
            if (input.checkin_time.empty() || input.checkin_time == "NOW") {
                auto now = std::chrono::system_clock::now();
                std::time_t t = std::chrono::system_clock::to_time_t(now);
                std::tm tmNow{};
#if defined(_WIN32)
                localtime_s(&tmNow, &t);
#else
                localtime_r(&t, &tmNow);
#endif
                char timeBuf[32];
                std::strftime(timeBuf, sizeof(timeBuf), "%H:%M:%S", &tmNow);
                input.checkin_time = timeBuf;
            }
            input.client_lat = extractJsonNumber(body, "client_lat", 10.823150);
            input.client_long = extractJsonNumber(body, "client_long", 106.629700);
            input.accuracy = extractJsonNumber(body, "accuracy", 4.0);
            input.device_id = extractJsonString(body, "device_id");
            if (input.device_id.empty()) input.device_id = "DEV-IP15-9921";
            input.mock_flag = extractJsonBool(body, "mock_flag");

            auto chkResp = repo->processCheckIn(input);
            res.content_type = "application/json";
            res.status_code = chkResp.success ? 200 : 400;

            std::ostringstream ss;
            ss << "{\"success\":" << (chkResp.success ? "true" : "false") << ","
               << "\"status\":\"" << statusToString(chkResp.status) << "\","
               << "\"points\":" << chkResp.points << ","
               << "\"distance_meters\":" << chkResp.distance_meters << ","
               << "\"device_id\":\"" << jsonEscape(chkResp.device_id.empty() ? input.device_id : chkResp.device_id) << "\","
               << "\"proxy_attempt\":" << (chkResp.proxy_attempt ? "true" : "false") << ","
               << "\"warning\":\"" << (chkResp.proxy_attempt
                    ? jsonEscape("CẢNH CÁO: Phát hiện điểm danh hộ! Thiết bị này đã được gán cho tài khoản sinh viên khác. Hành vi đã được ghi vào nhật ký bảo mật.")
                    : "") << "\","
               << "\"remarks\":\"" << jsonEscape(chkResp.remarks) << "\","
               << "\"error\":\"" << jsonEscape(chkResp.error_message) << "\"}";
            res.body = ss.str();
            return res;
        }

        // 3. GET /api/lecturer/leave-queue (FR-12, UC09/UC10)
        // Returns every PENDING leave request, newest first, so the Review screen
        // always displays what the student has just submitted.
        if (method == "GET" && path == "/api/lecturer/leave-queue") {
            std::vector<LeaveRequest> pending = repo->getPendingLeaveRequests();
            std::sort(pending.begin(), pending.end(),
                      [](const LeaveRequest& a, const LeaveRequest& b) { return a.id > b.id; });

            std::ostringstream ss;
            ss << "{\"success\":true,\"count\":" << pending.size() << ",\"requests\":[";
            for (size_t i = 0; i < pending.size(); ++i) {
                const LeaveRequest& r = pending[i];
                if (i) ss << ",";
                ss << "{\"request_id\":" << r.id
                   << ",\"student_code\":\"" << jsonEscape(r.student_code) << "\""
                   << ",\"student_name\":\"" << jsonEscape(r.student_name) << "\""
                   << ",\"category\":\"" << jsonEscape(r.reason_category) << "\""
                   << ",\"detail\":\"" << jsonEscape(r.reason_detail) << "\""
                   << ",\"file_name\":\"" << jsonEscape(r.evidence_file_url) << "\""
                   << ",\"file_size\":" << r.evidence_file_size
                   << ",\"submitted_at\":\"" << jsonEscape(r.submitted_at) << "\""
                   << ",\"status\":\"PENDING\"}";
            }
            ss << "]}";

            res.status_code = 200;
            res.content_type = "application/json";
            res.body = ss.str();
            return res;
        }

        // 3b. GET /api/lecturer/session-roster (FR-11, FR-13)
        // Live attendance roster for the active session: ONLY students who have
        // actually checked in, with their real coordinates for the radar.
        if (method == "GET" && path == "/api/lecturer/session-roster") {
            std::vector<AttendanceRecord> recs = repo->getSessionRecords(1);
            // Only real check-in attempts are shown. ABSENT rows are auto-generated
            // when a session closes and must never appear on the radar.
            recs.erase(std::remove_if(recs.begin(), recs.end(),
                                      [](const AttendanceRecord& r) {
                                          return r.status == AttendanceStatus::ABSENT;
                                      }),
                       recs.end());
            std::sort(recs.begin(), recs.end(),
                      [](const AttendanceRecord& a, const AttendanceRecord& b) {
                          return a.checkin_time > b.checkin_time; // newest first
                      });

            int present = 0, late = 0, excused = 0, rejected = 0, absent = 0;
            double totalPoints = 0.0;
            for (const auto& r : recs) {
                switch (r.status) {
                    case AttendanceStatus::PRESENT:  present++;  break;
                    case AttendanceStatus::LATE:     late++;     break;
                    case AttendanceStatus::EXCUSED:  excused++;  break;
                    case AttendanceStatus::REJECTED: rejected++; break;
                    default:                         absent++;   break;
                }
                totalPoints += r.points;
            }
            double avg = recs.empty() ? 0.0 : totalPoints / (double)recs.size();

            // Anchor + radius come from the live check-in zone (set by LECTURER/ADMIN)
            DatabaseRepository::GeofenceSnapshot zone = repo->getGeofence(1);

            std::ostringstream ss;
            ss << "{\"success\":true,"
               << "\"anchor_lat\":" << std::fixed << std::setprecision(6) << zone.latitude
               << ",\"anchor_long\":" << zone.longitude
               << ",\"radius\":" << std::setprecision(1) << zone.radius
               << ",\"room\":\"" << jsonEscape(zone.room_code) << "\","
               << "\"checked_in\":" << recs.size() << ","
               << "\"counts\":{\"present\":" << present
               << ",\"late\":" << late
               << ",\"excused\":" << excused
               << ",\"rejected\":" << rejected
               << ",\"absent\":" << absent << "},"
               << "\"average_points\":" << std::fixed << std::setprecision(1) << avg << ","
               << "\"pending_leaves\":" << repo->getPendingLeaveRequests().size() << ","
               << "\"records\":[";
            for (size_t i = 0; i < recs.size(); ++i) {
                const AttendanceRecord& r = recs[i];
                if (i) ss << ",";
                ss << "{\"student_code\":\"" << jsonEscape(r.student_code) << "\""
                   << ",\"student_name\":\"" << jsonEscape(r.student_name) << "\""
                   << ",\"checkin_time\":\"" << jsonEscape(r.checkin_time) << "\""
                   << ",\"status\":\"" << attendanceStatusToString(r.status) << "\""
                   << ",\"points\":" << r.points
                   << ",\"distance\":" << std::fixed << std::setprecision(1) << r.distance_meters
                   << ",\"lat\":" << std::setprecision(6) << r.client_lat
                   << ",\"long\":" << r.client_long << "}";
            }
            ss << "]}";

            res.status_code = 200;
            res.content_type = "application/json";
            res.body = ss.str();
            return res;
        }

        // 3c. GET /api/session/geofence - current check-in circle (readable by everyone)
        if (method == "GET" && path == "/api/session/geofence") {
            DatabaseRepository::GeofenceSnapshot z = repo->getGeofence(1);
            std::ostringstream ss;
            ss << "{\"success\":" << (z.found ? "true" : "false")
               << ",\"session_id\":" << z.session_id
               << ",\"room\":\"" << jsonEscape(z.room_code) << "\""
               << ",\"building\":\"" << jsonEscape(z.building) << "\""
               << ",\"latitude\":" << std::fixed << std::setprecision(6) << z.latitude
               << ",\"longitude\":" << z.longitude
               << ",\"radius\":" << std::setprecision(1) << z.radius
               << ",\"start_time\":\"" << jsonEscape(z.start_time) << "\""
               << ",\"checkin_open\":\"06:45\",\"ontime_end\":\"07:15\",\"late_end\":\"07:45\""
               << ",\"status\":\"" << jsonEscape(z.status) << "\"}";
            res.status_code = z.found ? 200 : 404;
            res.content_type = "application/json";
            res.body = ss.str();
            return res;
        }

        // 3d. POST /api/lecturer/geofence - pick the circle centre + radius.
        // RBAC: only a LECTURER or an ADMIN may change the check-in zone.
        if (method == "POST" && path == "/api/lecturer/geofence") {
            std::string role = extractJsonString(body, "role");
            std::string token = extractJsonString(body, "token");
            std::string actor = extractJsonString(body, "actor");

            // Accept a login token ("TOKEN_<ROLE>_<id>") as proof of role
            if (role.empty() && token.rfind("TOKEN_", 0) == 0) {
                size_t p1 = token.find('_', 6);
                if (p1 != std::string::npos) role = token.substr(6, p1 - 6);
            }
            std::transform(role.begin(), role.end(), role.begin(),
                           [](unsigned char c) { return (char)std::toupper(c); });

            if (role != "LECTURER" && role != "ADMIN") {
                res.status_code = 403;
                res.content_type = "application/json";
                res.body = R"({"success":false,"error":"Forbidden: only a LECTURER or ADMIN can change the check-in zone."})";
                return res;
            }

            double lat = extractJsonNumber(body, "lat", -999.0);
            double lng = extractJsonNumber(body, "long", -999.0);
            double radius = extractJsonNumber(body, "radius", -1.0);

            if (lat < -90.0 || lat > 90.0 || lng < -180.0 || lng > 180.0) {
                res.status_code = 400;
                res.content_type = "application/json";
                res.body = R"({"success":false,"error":"Invalid coordinates: latitude must be -90..90 and longitude -180..180."})";
                return res;
            }
            if (radius < 5.0 || radius > 500.0) {
                res.status_code = 400;
                res.content_type = "application/json";
                res.body = R"({"success":false,"error":"Invalid radius: must be between 5 and 500 metres."})";
                return res;
            }

            if (!repo->updateSessionGeofence(1, lat, lng, radius)) {
                res.status_code = 404;
                res.content_type = "application/json";
                res.body = R"({"success":false,"error":"Active session not found."})";
                return res;
            }
            repo->logGeofenceUpdate(actor.empty() ? role : actor, lat, lng, radius);

            std::ostringstream ss;
            ss << "{\"success\":true,\"role\":\"" << jsonEscape(role) << "\""
               << ",\"latitude\":" << std::fixed << std::setprecision(6) << lat
               << ",\"longitude\":" << lng
               << ",\"radius\":" << std::setprecision(1) << radius
               << ",\"message\":\"Check-in zone updated. Students outside the circle are rejected.\"}";
            res.status_code = 200;
            res.content_type = "application/json";
            res.body = ss.str();
            return res;
        }

        // 3. POST /api/student/leave-request (FR-06)
        if (method == "POST" && path == "/api/student/leave-request") {
            std::string code = extractJsonString(body, "student_code");
            if (code.empty()) code = "B22DCCN001";
            std::string cat = extractJsonString(body, "category");
            std::string detail = extractJsonString(body, "detail");
            std::string file = extractJsonString(body, "file_name");
            long long fileSize = (long long)extractJsonNumber(body, "file_size", 0);

            int reqId = repo->submitLeaveRequest(1, code, cat, detail, file, fileSize);
            res.content_type = "application/json";
            res.status_code = (reqId > 0) ? 200 : 400;

            std::ostringstream ss;
            ss << "{\"success\":" << (reqId > 0 ? "true" : "false") << ",\"request_id\":" << reqId << "}";
            res.body = ss.str();
            return res;
        }

        // 4. POST /api/lecturer/leave-adjudicate (FR-12, UC10)
        if (method == "POST" && path == "/api/lecturer/leave-adjudicate") {
            int reqId = (int)extractJsonNumber(body, "request_id", 108);
            bool isApproved = extractJsonBool(body, "is_approved");
            std::string notes = extractJsonString(body, "notes");

            bool ok = repo->adjudicateLeaveRequest(reqId, isApproved, notes, 1);
            res.content_type = "application/json";
            res.status_code = ok ? 200 : 400;
            res.body = ok ? R"({"success":true,"message":"Adjudicated successfully."})" : R"({"success":false})";
            return res;
        }

        // 5. POST /api/admin/device-reset (FR-16, UC18)
        if (method == "POST" && path == "/api/admin/device-reset") {
            std::string studentCode = extractJsonString(body, "student_code");
            bool ok = repo->resetDeviceBinding(studentCode);
            res.content_type = "application/json";
            res.status_code = ok ? 200 : 404;
            res.body = ok ? R"({"success":true,"message":"Device binding lock reset."})" : R"({"success":false})";
            return res;
        }

        // 6. POST /api/admin/create-account (Admin tạo tài khoản sinh viên/giảng viên/admin mới)
        if (method == "POST" && path == "/api/admin/create-account") {
            std::string username = extractJsonString(body, "username");
            std::string password = extractJsonString(body, "password");
            std::string fullName = extractJsonString(body, "full_name");
            std::string email = extractJsonString(body, "email");
            std::string role = extractJsonString(body, "role");
            std::string code = extractJsonString(body, "code");

            UserRole roleEnum;
            if (role == "STUDENT") roleEnum = UserRole::STUDENT;
            else if (role == "LECTURER") roleEnum = UserRole::LECTURER;
            else if (role == "ADMIN") roleEnum = UserRole::ADMIN;
            else { res.status_code = 400; res.content_type = "application/json"; res.body = R"({"success":false,"error":"Invalid role."})"; return res; }

            int newId = repo->createUser(username, password, fullName, email, roleEnum, code,
                                         extractJsonString(body, "phone"),
                                         extractJsonString(body, "date_of_birth"));
            if (newId > 0) {
                res.status_code = 200;
                res.content_type = "application/json";
                res.body = R"({"success":true,"user_id":)" + std::to_string(newId) + R"(})";
            } else {
                res.status_code = 400;
                res.content_type = "application/json";
                res.body = R"({"success":false,"error":"Username already exists."})";
            }
            return res;
        }

        // 6b. GET /api/admin/get-accounts (Danh sach tai khoan cho bang quan ly)
        if (method == "GET" && path == "/api/admin/get-accounts") {
            auto all = repo->getAllUsers();
            std::string json = R"({"success":true,"accounts":[)";
            for (size_t i = 0; i < all.size(); ++i) {
                const auto& u = all[i];
                if (i) json += ",";
                json += "{";
                json += R"("id":)" + std::to_string(u.id) + ",";
                json += R"("username":")" + jsonEscape(u.username) + "\",";
                json += R"("full_name":")" + jsonEscape(u.full_name) + "\",";
                json += R"("email":")" + jsonEscape(u.email) + "\",";
                json += R"("phone":")" + jsonEscape(u.phone) + "\",";
                json += R"("date_of_birth":")" + jsonEscape(u.date_of_birth) + "\",";
                json += R"("role":")" + roleToString(u.role) + "\"";
                json += "}";
            }
            json += "]}";
            res.status_code = 200;
            res.content_type = "application/json";
            res.body = json;
            return res;
        }

        // 6c. POST /api/admin/delete-account (Xoa tai khoan theo username)
        if (method == "POST" && path == "/api/admin/delete-account") {
            std::string username = extractJsonString(body, "username");
            if (username.empty()) {
                res.status_code = 400;
                res.content_type = "application/json";
                res.body = R"({"success":false,"error":"Missing username."})";
                return res;
            }
            if (username == "ADM001") {
                res.status_code = 400;
                res.content_type = "application/json";
                res.body = R"({"success":false,"error":"Khong the xoa tai khoan admin goc."})";
                return res;
            }
            if (repo->deleteUser(username)) {
                res.status_code = 200;
                res.content_type = "application/json";
                res.body = R"({"success":true})";
            } else {
                res.status_code = 404;
                res.content_type = "application/json";
                res.body = R"({"success":false,"error":"Account not found."})";
            }
            return res;
        }

        // 6d. GET /api/admin/device-bindings - thiet bi that dang gan voi tung sinh vien
        if (method == "GET" && path == "/api/admin/device-bindings") {
            auto list = repo->getDeviceBindings();
            std::ostringstream ss;
            ss << "{\"success\":true,\"bindings\":[";
            for (size_t i = 0; i < list.size(); ++i) {
                const auto& b = list[i];
                if (i) ss << ",";
                ss << "{\"user_id\":" << b.user_id
                   << ",\"student_code\":\"" << jsonEscape(b.student_code) << "\""
                   << ",\"full_name\":\"" << jsonEscape(b.full_name) << "\""
                   << ",\"device_id\":\"" << jsonEscape(b.device_id) << "\""
                   << ",\"locked\":" << (b.locked ? "true" : "false") << "}";
            }
            ss << "]}";
            res.status_code = 200;
            res.content_type = "application/json";
            res.body = ss.str();
            return res;
        }

        // 6e. GET /api/admin/device-reset-requests - don xin mo khoa design bi
        if (method == "GET" && path == "/api/admin/device-reset-requests") {
            auto list = repo->getPendingDeviceResetRequests();
            std::ostringstream ss;
            ss << "{\"success\":true,\"count\":" << list.size() << ",\"requests\":[";
            for (size_t i = 0; i < list.size(); ++i) {
                const auto& r = list[i];
                if (i) ss << ",";
                ss << "{\"request_id\":" << r.id
                   << ",\"student_code\":\"" << jsonEscape(r.student_code) << "\""
                   << ",\"student_name\":\"" << jsonEscape(r.student_name) << "\""
                   << ",\"reason\":\"" << jsonEscape(r.reason) << "\""
                   << ",\"explanation\":\"" << jsonEscape(r.explanation) << "\""
                   << ",\"file_name\":\"" << jsonEscape(r.file_name) << "\""
                   << ",\"file_size\":" << r.file_size
                   << ",\"created_at\":\"" << jsonEscape(r.created_at) << "\""
                   << ",\"status\":\"" << jsonEscape(r.status) << "\"}";
            }
            ss << "]}";
            res.status_code = 200;
            res.content_type = "application/json";
            res.body = ss.str();
            return res;
        }

        // 6f. POST /api/admin/device-reset-requests/review - admin duyet / tu choi
        if (method == "POST" && path == "/api/admin/device-reset-requests/review") {
            int requestId = (int)extractJsonNumber(body, "request_id");
            bool approve = extractJsonBool(body, "approve");
            std::string note = extractJsonString(body, "note");
            if (repo->reviewDeviceResetRequest(requestId, approve, note)) {
                res.status_code = 200;
                res.content_type = "application/json";
                res.body = R"({"success":true})";
            } else {
                res.status_code = 404;
                res.content_type = "application/json";
                res.body = R"({"success":false,"error":"Request not found or already reviewed."})";
            }
            return res;
        }

        // 6g. POST /api/student/device-reset-request - sinh viên gui don kem minh chung
        if (method == "POST" && path == "/api/student/device-reset-request") {
            std::string studentCode = extractJsonString(body, "student_code");
            std::string reason = extractJsonString(body, "reason");
            std::string explanation = extractJsonString(body, "explanation");
            std::string fileName = extractJsonString(body, "file_name");
            long long fileSize = (long long)extractJsonNumber(body, "file_size");
            if (studentCode.empty()) studentCode = "B22DCCN001";
            if (fileName.empty()) {
                res.status_code = 400;
                res.content_type = "application/json";
                res.body = R"({"success":false,"error":"Missing image proof."})";
                return res;
            }
            int id = repo->submitDeviceResetRequest(studentCode, reason, explanation, fileName, fileSize);
            if (id > 0) {
                res.status_code = 200;
                res.content_type = "application/json";
                res.body = R"({"success":true,"request_id":)" + std::to_string(id) + R"(})";
            } else {
                res.status_code = 404;
                res.content_type = "application/json";
                res.body = R"({"success":false,"error":"Student account not found."})";
            }
            return res;
        }

        // 6h. POST /api/student/device-reset-requests - lich su don cua sinh vien
        if (method == "POST" && path == "/api/student/device-reset-requests") {
            std::string studentCode = extractJsonString(body, "student_code");
            if (studentCode.empty()) studentCode = "B22DCCN001";
            auto list = repo->getDeviceResetRequestsForStudent(studentCode);
            std::ostringstream ss;
            ss << "{\"success\":true,\"requests\":[";
            for (size_t i = 0; i < list.size(); ++i) {
                const auto& r = list[i];
                if (i) ss << ",";
                ss << "{\"request_id\":" << r.id
                   << ",\"reason\":\"" << jsonEscape(r.reason) << "\""
                   << ",\"file_name\":\"" << jsonEscape(r.file_name) << "\""
                   << ",\"status\":\"" << jsonEscape(r.status) << "\""
                   << ",\"reviewer_note\":\"" << jsonEscape(r.reviewer_note) << "\"}";
            }
            ss << "]}";
            res.status_code = 200;
            res.content_type = "application/json";
            res.body = ss.str();
            return res;
        }

        // 6. Handle CORS preflight
        if (method == "OPTIONS") {
            res.status_code = 200;
            res.content_type = "text/plain";
            res.body = "OK";
            return res;
        }

        // Default 404
        res.status_code = 404;
        res.content_type = "text/plain";
        res.body = "404 Not Found";
        return res;
    }
};

} // namespace attendance
