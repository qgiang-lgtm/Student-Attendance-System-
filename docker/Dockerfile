# =========================================================================
# SMART GPS STUDENT ATTENDANCE SYSTEM - MULTI-STAGE DOCKERFILE (C++17)
# Group: Group_01 | Course: Software Engineering Capstone Project
# =========================================================================

# -------------------------------------------------------------------------
# Stage 1: Build & Automated Unit Test Verification
# -------------------------------------------------------------------------
FROM gcc:13-bookworm AS builder

WORKDIR /app

# Copy all source code, tests, headers and the vendored Google Test framework
COPY src/ /app/src/
COPY tests/ /app/tests/
COPY third_party/ /app/third_party/

# Compile the automated Unit Test Suite with C++17 and pthread
# (-static-libstdc++ / -static-libgcc so the GCC 13 runtime works on the older
#  libstdc++ shipped by debian:bookworm-slim in the runtime stage)
RUN g++ -std=c++17 -O2 -Isrc -Itests \
    -Ithird_party/googletest/include -Ithird_party/googletest \
    -pthread -static-libstdc++ -static-libgcc \
    tests/test_suite.cpp third_party/googletest/src/gtest-all.cc \
    -o /app/tests.out

# Execute automated tests during build stage (Build fails immediately if any test fails)
RUN /app/tests.out

# Compile the main high-performance C++ HTTP Server & REST API
RUN g++ -std=c++17 -O2 -Isrc -pthread -static-libstdc++ -static-libgcc \
    src/main.cpp \
    -o /app/server.out

# -------------------------------------------------------------------------
# Stage 2: Minimal Production Runtime
# -------------------------------------------------------------------------
FROM debian:bookworm-slim AS runtime

WORKDIR /app

# Install runtime dependencies: curl for healthcheck, ca-certificates
RUN apt-get update && apt-get install -y --no-install-recommends \
    curl \
    ca-certificates \
    tzdata \
    && rm -rf /var/lib/apt/lists/*

# Set standard timezone
ENV TZ=Asia/Ho_Chi_Minh
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

# Copy compiled binaries from builder stage
COPY --from=builder /app/server.out /app/server
COPY --from=builder /app/tests.out /app/tests

# Expose HTTP Server Port (default: 8080)
EXPOSE 8080

# Container Healthcheck (NFR-06 Reliability: 99.8% Uptime verification)
HEALTHCHECK --interval=30s --timeout=5s --start-period=5s --retries=3 \
    CMD curl -f http://localhost:8080/api/health || exit 1

# Launch the Smart GPS Attendance HTTP Server
CMD ["/app/server"]
