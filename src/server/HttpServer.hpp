#pragma once
#include <iostream>
#include <string>
#include <thread>
#include <atomic>
#include <memory>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <cctype>
#include <algorithm>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#define SOCKET int
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define closesocket close
#endif

#include "ApiRouter.hpp"

namespace attendance {

class HttpServer {
private:
    int port{8080};
    std::shared_ptr<ApiRouter> router;
    std::atomic<bool> isRunning{false};
    SOCKET serverSocket{INVALID_SOCKET};

    void handleClient(SOCKET clientSocket, std::string clientIp) {
        char buffer[8192];
        std::string rawRequest;
        size_t headerEnd = std::string::npos;
        size_t contentLength = 0;
        const size_t MAX_REQUEST_BYTES = 2 * 1024 * 1024; // 2 MB safety cap

        // Read until the whole request has arrived. A single recv() is not
        // enough: TCP can deliver the headers and the JSON body in separate
        // segments, which silently produced an empty body (and therefore
        // empty category/detail/file_name in the leave-request endpoint).
        while (true) {
            std::memset(buffer, 0, sizeof(buffer));
            int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
            if (bytesReceived <= 0) break;
            rawRequest.append(buffer, (size_t)bytesReceived);

            if (headerEnd == std::string::npos) {
                headerEnd = rawRequest.find("\r\n\r\n");
                if (headerEnd != std::string::npos) {
                    std::string headers = rawRequest.substr(0, headerEnd);
                    std::transform(headers.begin(), headers.end(), headers.begin(),
                                   [](unsigned char c) { return (char)std::tolower(c); });
                    size_t p = headers.find("content-length:");
                    if (p != std::string::npos) {
                        contentLength = (size_t)std::strtoul(headers.c_str() + p + 15, nullptr, 10);
                    }
                }
            }

            if (headerEnd != std::string::npos &&
                rawRequest.size() >= headerEnd + 4 + contentLength) {
                break;
            }
            if (rawRequest.size() > MAX_REQUEST_BYTES) break;
        }

        if (rawRequest.empty()) {
            closesocket(clientSocket);
            return;
        }

        std::string raw = rawRequest;

        // Parse Method and Path from HTTP request line: "GET /path HTTP/1.1"
        std::istringstream reqStream(raw);
        std::string method, path, httpVersion;
        reqStream >> method >> path >> httpVersion;

        // Find body after double CRLF
        std::string body;
        size_t bodyPos = raw.find("\r\n\r\n");
        if (bodyPos != std::string::npos) {
            body = raw.substr(bodyPos + 4);
        }

        // Extract the real client address when the request came through a
        // tunnel / reverse proxy (cloudflared, ngrok, nginx, ...). Those
        // proxies terminate the connection locally, so the socket peer is
        // 127.0.0.1 and the real address arrives in a forwarding header.
        std::string forwardedFor;
        {
            std::string headerBlock = (bodyPos == std::string::npos) ? raw : raw.substr(0, bodyPos);
            std::string lower = headerBlock;
            std::transform(lower.begin(), lower.end(), lower.begin(),
                           [](unsigned char c) { return (char)std::tolower(c); });

            auto headerValue = [&](const std::string& name) -> std::string {
                size_t p = lower.find("\r\n" + name + ":");
                if (p == std::string::npos) return "";
                p += name.size() + 3;
                size_t e = headerBlock.find("\r\n", p);
                std::string v = headerBlock.substr(p, (e == std::string::npos) ? std::string::npos : e - p);
                while (!v.empty() && (v.front() == ' ' || v.front() == '\t')) v.erase(v.begin());
                while (!v.empty() && (v.back() == ' ' || v.back() == '\t')) v.pop_back();
                return v;
            };

            forwardedFor = headerValue("cf-connecting-ip");
            if (forwardedFor.empty()) forwardedFor = headerValue("x-real-ip");
            if (forwardedFor.empty()) forwardedFor = headerValue("x-forwarded-for");
            // "client, proxy1, proxy2" -> keep the first hop only
            size_t comma = forwardedFor.find(',');
            if (comma != std::string::npos) forwardedFor = forwardedFor.substr(0, comma);
            while (!forwardedFor.empty() && forwardedFor.back() == ' ') forwardedFor.pop_back();
        }

        // Route Request
        ApiRouter::HttpResponse response = router->handleRequest(method, path, body, clientIp, forwardedFor);

        // Format HTTP Response
        std::ostringstream respStream;
        respStream << "HTTP/1.1 " << response.status_code << " ";
        if (response.status_code == 200) respStream << "OK";
        else if (response.status_code == 400) respStream << "Bad Request";
        else if (response.status_code == 401) respStream << "Unauthorized";
        else if (response.status_code == 404) respStream << "Not Found";
        else respStream << "Status";
        respStream << "\r\n";

        respStream << "Content-Type: " << response.content_type << "\r\n";
        respStream << "Content-Length: " << response.body.size() << "\r\n";
        respStream << "Access-Control-Allow-Origin: *\r\n";
        respStream << "Access-Control-Allow-Methods: GET, POST, OPTIONS\r\n";
        respStream << "Access-Control-Allow-Headers: Content-Type\r\n";
        respStream << "Connection: close\r\n\r\n";
        respStream << response.body;

        std::string finalResp = respStream.str();
        send(clientSocket, finalResp.c_str(), (int)finalResp.size(), 0);
        closesocket(clientSocket);
    }

public:
    HttpServer(int listenPort, std::shared_ptr<ApiRouter> apiRouter) 
        : port(listenPort), router(apiRouter) {}

    ~HttpServer() {
        stop();
    }

    bool start() {
#ifdef _WIN32
        WSADATA wsaData;
        int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (iResult != 0) {
            std::cerr << "[HttpServer] WSAStartup failed: " << iResult << std::endl;
            return false;
        }
#endif

        serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (serverSocket == INVALID_SOCKET) {
            std::cerr << "[HttpServer] Error creating socket." << std::endl;
            return false;
        }

        sockaddr_in service;
        service.sin_family = AF_INET;
        service.sin_addr.s_addr = INADDR_ANY;
        service.sin_port = htons(port);

        if (bind(serverSocket, (sockaddr*)&service, sizeof(service)) == SOCKET_ERROR) {
            std::cerr << "[HttpServer] Bind failed on port " << port << std::endl;
            closesocket(serverSocket);
            return false;
        }

        if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
            std::cerr << "[HttpServer] Listen failed." << std::endl;
            closesocket(serverSocket);
            return false;
        }

        isRunning = true;
        std::cout << "===========================================================" << std::endl;
        std::cout << "   SMART GPS STUDENT ATTENDANCE SYSTEM - C++ SERVER" << std::endl;
        std::cout << "===========================================================" << std::endl;
        std::cout << " [Architecture] Clean Layered Architecture (Domain / Repo / API / UI)" << std::endl;
        std::cout << " [Server Status] Listening on 0.0.0.0:" << port
                  << " (LAN: http://<PC-IPv4>:" << port << ")" << std::endl;
        std::cout << " [SRS Wireframes] Serving Screens 1 to 8 at http://localhost:" << port << "/" << std::endl;
        std::cout << "===========================================================" << std::endl;

        return true;
    }

    void runLoop() {
        while (isRunning) {
            sockaddr_in clientAddr;
#ifdef _WIN32
            int clientAddrLen = sizeof(clientAddr);
#else
            socklen_t clientAddrLen = sizeof(clientAddr);
#endif
            SOCKET clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientAddrLen);
            if (clientSocket != INVALID_SOCKET) {
                char ipBuf[INET_ADDRSTRLEN] = {0};
#ifdef _WIN32
                inet_ntop(AF_INET, &clientAddr.sin_addr, ipBuf, (size_t)INET_ADDRSTRLEN);
#else
                inet_ntop(AF_INET, &clientAddr.sin_addr, ipBuf, (socklen_t)INET_ADDRSTRLEN);
#endif
                std::thread(&HttpServer::handleClient, this, clientSocket,
                            std::string(ipBuf)).detach();
            }
        }
    }

    void stop() {
        isRunning = false;
        if (serverSocket != INVALID_SOCKET) {
            closesocket(serverSocket);
            serverSocket = INVALID_SOCKET;
        }
#ifdef _WIN32
        WSACleanup();
#endif
    }
};

} // namespace attendance
