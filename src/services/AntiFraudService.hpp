#pragma once
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "../models/Models.hpp"

namespace attendance {

struct FraudValidationResult {
    bool is_valid{true};
    std::string error_code;
    std::string error_message;
};

class AntiFraudService {
public:
    static constexpr double MAX_ALLOWED_ACCURACY_METERS = 20.0;
    static constexpr double MAX_REASONABLE_WALKING_VELOCITY_MPS = 15.0; // ~54 km/h teleport threshold

    /**
     * NFR-09: Accuracy validation
     */
    static FraudValidationResult validateAccuracy(double accuracyMeters) {
        FraudValidationResult res;
        if (accuracyMeters <= 0.0 || accuracyMeters > MAX_ALLOWED_ACCURACY_METERS) {
            res.is_valid = false;
            res.error_code = "ERR_INSUFFICIENT_GPS_ACCURACY";
            res.error_message = "GPS accuracy radius (" + std::to_string((int)accuracyMeters) + 
                                "m) exceeds acceptable 20m threshold. Please retry in open area.";
        }
        return res;
    }

    /**
     * NFR-09: Mock Location and impossible velocity check
     */
    static FraudValidationResult checkMockLocation(bool browserMockFlag, bool isEmulated) {
        FraudValidationResult res;
        if (browserMockFlag || isEmulated) {
            res.is_valid = false;
            res.error_code = "ERR_MOCK_LOCATION_DETECTED";
            res.error_message = "Mock location provider or developer fake GPS spoofing detected!";
        }
        return res;
    }

    /**
     * FR-03 & UC21: Anti-Proxy Device Binding
     * 1 handset can only record attendance for 1 student per session.
     */
    static FraudValidationResult verifyDeviceBinding(
        int studentId,
        const std::string& studentCode,
        const std::string& deviceId,
        const std::string& boundDeviceId,
        bool isDeviceLocked,
        const std::unordered_map<std::string, std::string>& sessionDeviceToStudentMap) {
        
        FraudValidationResult res;

        // Check if device is already used by another student in this class session
        auto it = sessionDeviceToStudentMap.find(deviceId);
        if (it != sessionDeviceToStudentMap.end() && it->second != studentCode) {
            res.is_valid = false;
            res.error_code = "ERR_PROXY_DEVICE_DETECTED";
            res.error_message = "Proxy Attendance Blocked: Device " + deviceId + 
                                " has already recorded attendance for student " + it->second + "!";
            return res;
        }

        // Check student device lock binding
        if (isDeviceLocked && !boundDeviceId.empty() && boundDeviceId != deviceId) {
            res.is_valid = false;
            res.error_code = "ERR_DEVICE_MISMATCH";
            res.error_message = "Account " + studentCode + " is bound to device " + boundDeviceId + 
                                ". Please submit a Device Reset Ticket if you changed phones.";
            return res;
        }

        return res;
    }
};

} // namespace attendance
