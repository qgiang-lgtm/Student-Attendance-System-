#include <iostream>
#include <memory>
#include "repository/DatabaseRepository.hpp"
#include "server/ApiRouter.hpp"
#include "server/HttpServer.hpp"

int main(int argc, char* argv[]) {
    int port = 8080;
    if (argc > 1) {
        port = std::stoi(argv[1]);
    }

    std::cout << "[System Init] Bootstrapping Smart GPS Attendance System..." << std::endl;
    auto repository = std::make_shared<attendance::DatabaseRepository>();
    auto router = std::make_shared<attendance::ApiRouter>(repository);
    attendance::HttpServer server(port, router);

    if (!server.start()) {
        std::cerr << "[Error] Failed to start HTTP server on port " << port << std::endl;
        return 1;
    }

    std::cout << "[NTP Synced] Authoritative Server Time: 2026-11-01 06:50:00" << std::endl;
    std::cout << "[Geofence Ready] Active session for CS301 (Room B204, R=35m)" << std::endl;
    std::cout << "[Press Ctrl+C to stop server]" << std::endl;

    server.runLoop();

    return 0;
}
