#pragma once
#include <cmath>

namespace attendance {

class GeoService {
public:
    static constexpr double EARTH_RADIUS_METERS = 6371000.0;
    static constexpr double PI = 3.14159265358979323846;

    static double toRadians(double degrees) {
        return degrees * (PI / 180.0);
    }

    /**
     * NFR-04: Haversine Spherical Distance Calculation
     * Calculates geodesic distance d between (lat1, lon1) and (lat2, lon2)
     */
    static double calculateDistance(double lat1, double lon1, double lat2, double lon2) {
        double phi1 = toRadians(lat1);
        double phi2 = toRadians(lat2);
        double deltaPhi = toRadians(lat2 - lat1);
        double deltaLambda = toRadians(lon2 - lon1);

        double sinHalfDeltaPhi = std::sin(deltaPhi / 2.0);
        double sinHalfDeltaLambda = std::sin(deltaLambda / 2.0);

        double a = sinHalfDeltaPhi * sinHalfDeltaPhi +
                   std::cos(phi1) * std::cos(phi2) *
                   sinHalfDeltaLambda * sinHalfDeltaLambda;

        // Guard against precision clamping issues outside [0, 1]
        if (a > 1.0) a = 1.0;
        if (a < 0.0) a = 0.0;

        double c = 2.0 * std::asin(std::sqrt(a));
        return EARTH_RADIUS_METERS * c;
    }

    /**
     * Evaluates whether a client coordinate falls within the classroom geofence
     */
    static bool isWithinGeofence(double clientLat, double clientLon, 
                                 double anchorLat, double anchorLon, 
                                 double allowedRadiusMeters) {
        double dist = calculateDistance(clientLat, clientLon, anchorLat, anchorLon);
        return dist <= allowedRadiusMeters;
    }
};

} // namespace attendance
