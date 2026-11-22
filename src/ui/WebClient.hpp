#pragma once
#include <string>

namespace attendance {

inline std::string getIndexHtml() {
    return R"rawliteral(<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0, viewport-fit=cover">
  <meta name="theme-color" content="#4f46e5">
  <meta name="apple-mobile-web-app-capable" content="yes">
  <meta name="mobile-web-app-capable" content="yes">
  <meta name="apple-mobile-web-app-status-bar-style" content="black-translucent">
  <meta name="apple-mobile-web-app-title" content="SmartGPS">
  <link rel="manifest" href="/manifest.webmanifest">
  <link rel="icon" href="/icon.svg" type="image/svg+xml">
  <link rel="apple-touch-icon" href="/icon.svg">
  <title>Smart GPS Student Attendance System</title>
  <script src="https://cdn.tailwindcss.com"></script>
  <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.4.0/css/all.min.css">
  <style>
    @keyframes pulse-ring {
      0% { transform: scale(0.95); opacity: 0.8; }
      50% { transform: scale(1.05); opacity: 0.4; }
      100% { transform: scale(0.95); opacity: 0.8; }
    }
    .radar-pulse { animation: pulse-ring 3s infinite ease-in-out; }
    .badge-present { background-color: #dcfce7; color: #15803d; border: 1px solid #86efac; }
    .badge-late { background-color: #fef9c3; color: #a16207; border: 1px solid #fde047; }
    .badge-excused { background-color: #f3e8ff; color: #7e22ce; border: 1px solid #d8b4fe; }
    .badge-absent { background-color: #fee2e2; color: #b91c1c; border: 1px solid #fca5a5; }

    /* ---- Mobile / touch usability ---- */
    html { -webkit-text-size-adjust: 100%; }
    body { overflow-x: hidden; }
    button, a, select, input[type="submit"] { touch-action: manipulation; }
    .no-scrollbar { -ms-overflow-style: none; scrollbar-width: none; }
    .no-scrollbar::-webkit-scrollbar { display: none; }

    /* iOS Safari zooms the whole page when focusing an input under 16px */
    @media (max-width: 640px) {
      input, select, textarea { font-size: 16px !important; }
    }

    /* Keep wide data tables scrollable inside their card instead of the page */
    .table-scroll { -webkit-overflow-scrolling: touch; }
  </style>
</head>
<body class="bg-slate-50 text-slate-800 font-sans min-h-screen flex flex-col">

  <!-- Top Navigation Wireframe Selector -->
  <header class="bg-white border-b border-slate-200 sticky top-0 z-50 shadow-sm">
    <div class="max-w-7xl mx-auto px-3 sm:px-4 py-2.5 sm:py-3 flex flex-wrap items-center justify-between gap-2 sm:gap-3">
      <div class="flex items-center gap-2 min-w-0">
        <div class="w-9 h-9 shrink-0 rounded-lg bg-indigo-600 flex items-center justify-center text-white font-bold text-lg shadow-md">
          <i class="fa-solid fa-location-crosshairs"></i>
        </div>
        <div class="min-w-0">
          <span class="font-bold text-slate-900 text-base sm:text-lg tracking-tight whitespace-nowrap">SmartGPS<span class="hidden sm:inline"> Attendance</span></span>
          <span class="hidden sm:inline-block text-xs bg-indigo-100 text-indigo-700 font-semibold ml-2 px-2 py-0.5 rounded-full">v2.0 C++</span>
        </div>
      </div>

      <div class="flex items-center gap-1.5 sm:gap-2 text-xs text-slate-500 min-w-0">
        <!-- Live NTP / Demo Mode Switcher -->
        <button id="btnTimeMode" onclick="toggleTimeMode()" title="Click to toggle between Live Real-time NTP and Demo Scenario Mode" aria-label="Toggle between Live NTP and Demo mode"
                class="px-2.5 py-1.5 rounded-lg text-xs font-semibold bg-emerald-100 text-emerald-800 border border-emerald-300 hover:bg-emerald-200 transition flex items-center gap-1.5 shadow-sm whitespace-nowrap">
          <i class="fa-solid fa-satellite-dish animate-pulse text-emerald-600"></i>
          <span id="timeModeLabel" class="hidden sm:inline">Live NTP Clock</span>
        </button>
        <span class="hidden md:inline font-mono font-medium text-slate-700 bg-slate-100 px-2.5 py-1 rounded-lg border border-slate-200">
          <i class="fa-solid fa-clock mr-1 text-indigo-500"></i><span id="clock">Syncing NTP...</span>
        </span>
        <span id="activeUserBadge" class="bg-slate-200 text-slate-700 px-2 py-1 rounded font-mono font-medium max-w-[6rem] sm:max-w-none truncate">Guest</span>
      </div>

      <!-- Quick Switcher Tabs (Matching SRS Wireframes 1 to 8) -->
      <nav id="mainNav" class="order-3 w-full lg:order-none lg:w-auto flex gap-1 bg-slate-100 p-1 rounded-xl text-xs sm:text-sm font-medium overflow-x-auto no-scrollbar whitespace-nowrap">
        <button onclick="switchScreen(1)" id="tab-1" class="shrink-0 px-3 py-2 rounded-lg bg-indigo-600 text-white shadow-sm transition">1 Login</button>
        <button onclick="switchScreen(2)" id="tab-2" class="shrink-0 px-3 py-2 rounded-lg text-slate-600 hover:text-slate-900 transition">2 Today</button>
        <button onclick="switchScreen(3)" id="tab-3" class="shrink-0 px-3 py-2 rounded-lg text-slate-600 hover:text-slate-900 transition">3 GPS Scan</button>
        <button onclick="switchScreen(4)" id="tab-4" class="shrink-0 px-3 py-2 rounded-lg text-slate-600 hover:text-slate-900 transition">4 Leave</button>
        <button onclick="switchScreen(5)" id="tab-5" class="shrink-0 px-3 py-2 rounded-lg text-slate-600 hover:text-slate-900 transition">5 Summary</button>
        <button onclick="switchScreen(6)" id="tab-6" class="shrink-0 px-3 py-2 rounded-lg text-slate-600 hover:text-slate-900 transition">6 Radar</button>
        <button onclick="switchScreen(7)" id="tab-7" class="shrink-0 px-3 py-2 rounded-lg text-slate-600 hover:text-slate-900 transition">7 Review</button>
        <button onclick="switchScreen(8)" id="tab-8" class="shrink-0 px-3 py-2 rounded-lg text-slate-600 hover:text-slate-900 transition">8 Admin</button>
      </nav>
    </div>
  </header>

  <!-- Main Content Screens Container -->
  <main class="flex-1 max-w-5xl w-full mx-auto p-3 sm:p-6">

    <!-- ========================================== -->
    <!-- SCREEN 1: DUAL-ROLE AUTHENTICATION (WIRE 8.1) -->
    <!-- ========================================== -->
    <section id="screen-1" class="screen-view">
      <div class="max-w-md mx-auto mt-2 sm:mt-6 bg-white rounded-2xl shadow-xl border border-slate-200 overflow-hidden">
        <div class="p-6 sm:p-8 text-center bg-gradient-to-b from-indigo-50/50 to-white">
          <div class="w-14 h-14 mx-auto mb-3 rounded-2xl bg-indigo-600 text-white flex items-center justify-center text-2xl shadow-lg shadow-indigo-200">
            <i class="fa-solid fa-satellite-dish"></i>
          </div>
          <h2 class="text-2xl font-bold text-slate-900">Smart GPS Attendance</h2>
          <p class="text-xs text-slate-500 mt-1">University of Technology • Official System</p>
        </div>

        <div class="p-5 sm:p-6 pt-0">
          <!-- Role Selector Tabs -->
          <div class="grid grid-cols-2 gap-1 bg-slate-100 p-1 rounded-xl mb-5">
            <button id="authTabStudent" onclick="selectAuthTab('Student')" class="py-2 text-sm font-semibold rounded-lg bg-white shadow-sm text-indigo-700 transition">
              👨‍🎓 Student
            </button>
            <button id="authTabLecturer" onclick="selectAuthTab('Lecturer')" class="py-2 text-sm font-medium rounded-lg text-slate-600 hover:text-slate-900 transition">
              🧑‍🏫 Lecturer
            </button>
          </div>

          <!-- Login Form -->
          <form onsubmit="handleLogin(event)" class="space-y-4">
            <div>
              <label id="lblUsername" class="block text-xs font-semibold uppercase tracking-wider text-slate-600 mb-1">Student ID (MSSV)</label>
              <input type="text" id="loginUsername" value="B22DCCN001" required 
                     class="w-full px-4 py-2.5 rounded-lg border border-slate-300 focus:ring-2 focus:ring-indigo-500 focus:border-indigo-500 font-mono text-sm outline-none">
            </div>

            <div>
              <label class="block text-xs font-semibold uppercase tracking-wider text-slate-600 mb-1">Password</label>
              <input type="password" id="loginPassword" value="password123" required 
                     class="w-full px-4 py-2.5 rounded-lg border border-slate-300 focus:ring-2 focus:ring-indigo-500 focus:border-indigo-500 text-sm outline-none">
            </div>

            <div class="p-3 bg-amber-50 border border-amber-200 rounded-lg text-xs text-amber-800 flex items-start gap-2">
              <i class="fa-solid fa-circle-info mt-0.5 text-amber-600"></i>
              <span><strong>Notice:</strong> System Administrators may log in via either tab with universal auto-routing (MSSV/Code: <code class="bg-amber-100 px-1 rounded">ADM001</code> / pass: <code class="bg-amber-100 px-1 rounded">admin123</code>).</span>
            </div>

            <button type="submit" class="w-full py-3 bg-indigo-600 hover:bg-indigo-700 text-white rounded-lg font-semibold text-sm shadow-md shadow-indigo-200 transition flex items-center justify-center gap-2">
              <span>Log in to system</span>
              <i class="fa-solid fa-arrow-right"></i>
            </button>

            <div class="text-center pt-2 text-xs text-slate-400 flex items-center justify-center gap-1 font-mono">
              <i class="fa-solid fa-mobile-screen"></i>
              <span>Client Device ID: <strong class="text-slate-600" id="deviceFingerprintLabel">DEV-IP15-9921</strong></span>
            </div>
          </form>
        </div>
      </div>

      <!-- Phone / LAN access helper -->
      <div id="phoneAccessCard" class="max-w-md mx-auto mt-4 bg-white rounded-2xl border border-slate-200 shadow-sm p-4 sm:p-5">
        <div class="flex items-start gap-3">
          <div class="w-9 h-9 shrink-0 rounded-xl bg-emerald-100 text-emerald-700 flex items-center justify-center">
            <i class="fa-solid fa-mobile-screen-button"></i>
          </div>
          <div class="min-w-0 flex-1">
            <h3 class="text-sm font-bold text-slate-900">Dùng trên điện thoại</h3>
            <p id="phoneAccessText" class="text-xs text-slate-500 mt-1">Đang kiểm tra địa chỉ mạng…</p>
            <div class="mt-2 flex flex-wrap items-center gap-2">
              <a id="phoneAccessLink" href="#" class="hidden text-sm font-mono font-bold text-indigo-700 underline break-all"></a>
              <button onclick="copyPhoneUrl()" id="phoneCopyBtn" class="hidden px-2.5 py-1 bg-slate-100 hover:bg-slate-200 text-slate-700 rounded-lg text-xs font-semibold">
                <i class="fa-regular fa-copy"></i> Copy
              </button>
            </div>
            <p class="text-[11px] text-slate-400 mt-2">Điện thoại phải kết nối cùng Wi-Fi với máy tính. Mở link rồi chọn <strong>Add to Home Screen</strong> để dùng như app.</p>
          </div>
        </div>
      </div>
    </section>

    <!-- ========================================== -->
    <!-- SCREEN 2: TODAY'S SCHEDULE BANNER (WIRE 8.2) -->
    <!-- ========================================== -->
    <section id="screen-2" class="screen-view hidden space-y-6">
      <div class="bg-white p-4 sm:p-5 rounded-2xl border border-slate-200 shadow-sm flex flex-wrap items-center justify-between gap-3">
        <div>
          <h1 class="text-xl sm:text-2xl font-bold text-slate-900 flex items-center gap-2">
            <span>🔔 Today's Schedule</span>
          </h1>
          <p class="text-xs text-slate-500 mt-1"><span id="serverDateBanner">Today's Academic Schedule</span> • Server time <span id="serverTimeSubBanner" class="font-mono text-indigo-600 font-semibold">06:50:00</span></p>
        </div>
        <div class="flex items-center gap-2">
          <span class="text-xs bg-slate-100 text-slate-700 font-mono px-3 py-1.5 rounded-lg border border-slate-200">Enrolled: B22DCCN001</span>
        </div>
      </div>

      <!-- GPS availability notice (HTTP is not a secure context) -->
      <div id="gpsInsecureNotice" class="hidden p-3 rounded-xl bg-amber-50 border border-amber-200 text-xs text-amber-800 flex items-start gap-2">
        <i class="fa-solid fa-triangle-exclamation mt-0.5"></i>
        <span>Trình duyệt chặn GPS trên kết nối HTTP, nên hệ thống dùng toạ độ lớp học (chế độ demo). Muốn lấy GPS thật trên điện thoại cần HTTPS.</span>
      </div>

      <!-- Classroom network status: check-in only works on the classroom Wi-Fi -->
      <div id="wifiStatusBox" class="p-3 rounded-xl bg-slate-50 border border-slate-200 text-xs text-slate-600 flex items-start gap-2">
        <i class="fa-solid fa-wifi mt-0.5"></i>
        <span id="wifiStatusText" class="flex-1">Đang kiểm tra kết nối Wi-Fi lớp học…</span>
      </div>

      <!-- Active Check-in Hero Card (Class CS301) -->
      <div class="bg-white rounded-2xl border-2 border-emerald-500 shadow-lg p-4 sm:p-6 relative overflow-hidden">
        <div id="checkinBadge" class="mb-3 sm:mb-0 sm:absolute sm:top-0 sm:right-0 inline-flex bg-emerald-500 text-white text-[11px] sm:text-xs font-bold px-3 sm:px-4 py-1 rounded-full sm:rounded-tr-none sm:rounded-bl-xl uppercase tracking-wider items-center gap-1.5">
          <span class="w-2 h-2 rounded-full bg-white animate-ping"></span>
          <span id="checkinBadgeText">🟢 GPS Check-in Active</span>
        </div>

        <div class="sm:pr-36">
          <h2 class="text-lg sm:text-xl font-bold text-slate-900 leading-snug">[CS301] Software Engineering (Section 01)</h2>
          <p class="text-sm text-slate-600 mt-1 flex flex-wrap gap-x-4 gap-y-1">
            <span><i class="fa-regular fa-clock mr-1 text-slate-400"></i>Starts 07:00 AM</span>
            <span><i class="fa-solid fa-door-open mr-1 text-slate-400"></i>Room B204 (2nd flr)</span>
            <span><i class="fa-solid fa-user-tie mr-1 text-slate-400"></i>Dr. Alexander Vance</span>
            <span><i class="fa-solid fa-circle-dot mr-1 text-slate-400"></i>Radius 35 m</span>
          </p>
          <p id="checkinRuleHint" class="text-xs text-emerald-700 font-medium mt-2 bg-emerald-50 border border-emerald-200 rounded-lg p-2 inline-block">
            <i class="fa-solid fa-shield-halved mr-1"></i>Check-in window: <strong>06:45</strong> (15 min before) &rarr; <strong>07:15</strong> (15 min after) = <strong>10/10 points</strong>. 07:15&ndash;07:45 = late (5/10). After 07:45 = absent.
          </p>
        </div>

        <!-- Big Countdown Timer (updated live from the server clock) -->
        <div id="countdownBox" class="my-4 sm:my-6 p-3 sm:p-4 rounded-xl bg-emerald-50 border border-emerald-200 flex flex-wrap items-center justify-center gap-x-3 gap-y-1 text-emerald-700 font-mono text-2xl sm:text-3xl font-extrabold transition-colors">
          <i id="countdownIcon" class="fa-solid fa-hourglass-half text-amber-500"></i>
          <span id="countdownTimer">--:--:--</span>
          <span id="countdownLabel" class="text-xs sm:text-sm font-sans font-medium text-slate-500">left for on-time score</span>
        </div>

        <!-- Action Buttons -->
        <div class="grid sm:grid-cols-2 gap-3">
          <button id="checkInBtn" onclick="triggerCheckIn()" class="py-3.5 px-4 bg-emerald-600 hover:bg-emerald-700 text-white rounded-xl font-bold text-sm shadow-md shadow-emerald-200 flex items-center justify-center gap-2 transition transform active:scale-95 disabled:opacity-50 disabled:cursor-not-allowed disabled:hover:bg-emerald-600">
            <i class="fa-solid fa-location-dot"></i>
            <span id="checkInBtnText">📍 CHECK-IN VIA GPS NOW</span>
          </button>
          <button onclick="switchScreen(4)" class="py-3.5 px-4 bg-white hover:bg-slate-50 text-indigo-700 border-2 border-indigo-200 hover:border-indigo-400 rounded-xl font-semibold text-sm flex items-center justify-center gap-2 transition">
            <i class="fa-regular fa-file-lines"></i>
            <span>📄 Submit Leave Request & Proof</span>
          </button>
        </div>
      </div>

      <!-- Locked Subsequent Class Card (Class CS302) -->
      <div class="bg-slate-100 rounded-2xl border border-slate-300 p-5 opacity-80">
        <div class="flex flex-wrap items-center justify-between gap-2">
          <div class="flex items-center gap-2 text-slate-500 font-medium text-xs">
            <i class="fa-solid fa-lock text-slate-400"></i>
            <span>LOCKED UNTIL 09:15 AM</span>
          </div>
          <span class="text-xs bg-slate-200 text-slate-600 font-semibold px-2.5 py-0.5 rounded-full">Opens 15' prior</span>
        </div>
        <h3 class="text-base font-bold text-slate-700 mt-2">[CS302] Computer Networks</h3>
        <p class="text-xs text-slate-500 mt-1">
          09:30 – 11:50 AM • Room A301 • Dr. Robert Lang
        </p>
      </div>
    </section>

    <!-- ========================================== -->
    <!-- SCREEN 3: GPS SCANNER & CONFIRMATION (WIRE 8.3) -->
    <!-- ========================================== -->
    <section id="screen-3" class="screen-view hidden space-y-6">
      <div class="bg-white p-4 sm:p-5 rounded-2xl border border-slate-200 shadow-sm flex items-center justify-between">
        <div>
          <button onclick="switchScreen(2)" class="text-xs text-indigo-600 hover:underline flex items-center gap-1 font-semibold mb-1">
            <i class="fa-solid fa-arrow-left"></i> Back to Schedule
          </button>
          <h1 class="text-lg sm:text-xl font-bold text-slate-900">GPS Attendance Scanner</h1>
          <p class="text-xs text-slate-500">[CS301] Room B204 • Authoritative NTP Time: 06:52:10 AM</p>
        </div>
      </div>

      <!-- Radar Animation Container -->
      <div class="bg-white p-4 sm:p-8 rounded-2xl border border-slate-200 shadow-sm flex flex-col items-center justify-center text-center">
        <div class="relative w-56 h-56 sm:w-64 sm:h-64 flex items-center justify-center mb-6">
          <!-- Outer circle (Allowed Geofence R = 35m) -->
          <div class="absolute inset-0 rounded-full border-2 border-dashed border-indigo-400 radar-pulse bg-indigo-50/30"></div>
          <!-- Mid circle -->
          <div class="absolute w-36 h-36 sm:w-44 sm:h-44 rounded-full border border-indigo-200"></div>
          <!-- Room Anchor (Center Black dot) -->
          <div class="absolute w-5 h-5 rounded-full bg-slate-900 text-[10px] text-white flex items-center justify-center font-bold shadow" title="Room B204 Center">
            A
          </div>
          <!-- Student Position (Blue/Green Dot at d=12.5m) -->
          <div id="studentGpsDot" class="absolute w-4 h-4 rounded-full bg-indigo-600 ring-4 ring-indigo-200 shadow-md transform translate-x-3 translate-y-4" title="Student Position"></div>
        </div>

        <p class="text-xs text-slate-500 mb-1">
          Anchor: Room <span id="stuRoom">B204</span> center (<span id="stuAnchorLat">10.823098</span>, <span id="stuAnchorLng">106.629663</span>) • Your position: 📍
        </p>
        <div class="text-lg sm:text-2xl font-black text-slate-800 font-mono leading-snug">
          Measured distance: <span id="measuredDistLabel" class="text-indigo-600">12.5 m</span> 
          <span class="text-xs font-sans text-slate-500 font-normal">(allowed <span id="stuRadius">35</span> m)</span>
        </div>

        <!-- Telemetry checks -->
        <div class="mt-4 flex flex-col gap-1.5 text-xs text-emerald-700 font-medium">
          <div class="flex items-center justify-center gap-2">
            <i class="fa-solid fa-check text-emerald-600"></i>
            <span>GPS signal strong (accuracy: ±4 m &le; 20 m threshold)</span>
          </div>
          <div class="flex items-center justify-center gap-2">
            <i class="fa-solid fa-check text-emerald-600"></i>
            <span>Device verified: <strong id="deviceVerifiedLabel">DEV-IP15-9921</strong> (No proxy conflict)</span>
          </div>
          <div class="flex items-center justify-center gap-2">
            <i class="fa-solid fa-check text-emerald-600"></i>
            <span>Mock location provider: Absent (Verified authentic sensor)</span>
          </div>
        </div>
      </div>

      <!-- Hardware-device / proxy warning banner -->
      <div id="proxyWarningBox" class="hidden p-4 sm:p-5 rounded-2xl bg-amber-50 border-2 border-amber-500 text-amber-950 shadow-md">
        <div class="flex items-start gap-3">
          <i class="fa-solid fa-triangle-exclamation text-2xl sm:text-3xl text-amber-600 shrink-0"></i>
          <div class="min-w-0">
            <h3 class="text-base sm:text-lg font-black">⚠️ CẢNH CÁO: Phát hiện điểm danh hộ!</h3>
            <p id="proxyWarningDetail" class="text-xs sm:text-sm mt-1 font-semibold"></p>
            <p class="text-[11px] mt-1 text-amber-800">Sự việc đã được ghi vào nhật ký bảo mật (PROXY_ATTEMPT). Vui lòng dùng đúng thiết bị đã đăng ký cho tài khoản của bạn.</p>
          </div>
        </div>
      </div>

      <!-- Result Confirmation Banner -->
      <div id="checkinResultBox" class="p-4 sm:p-5 rounded-2xl bg-emerald-50 border-2 border-emerald-500 text-emerald-900 shadow-md">
        <div class="flex items-start justify-between gap-3">
          <div class="min-w-0">
            <span id="resultBadge" class="inline-block text-[10px] sm:text-xs uppercase font-extrabold tracking-wider text-emerald-700 bg-emerald-100 px-2.5 py-1 rounded-md">
              ATTENDANCE CONFIRMATION SUCCESS
            </span>
            <h3 id="resultTitle" class="text-base sm:text-lg font-bold mt-2">Status: PRESENT (ON-TIME) • Points: 10 / 10</h3>
            <p id="resultRemark" class="text-xs text-emerald-700 mt-1">
              Remark: Self-recorded via GPS (On-time) • Timestamp: 06:52:10 AM
            </p>
          </div>
          <div id="resultIcon" class="text-emerald-600 text-2xl sm:text-3xl shrink-0">
            <i class="fa-solid fa-circle-check"></i>
          </div>
        </div>
      </div>

      <!-- Device card: registered hardware + request unbind with proof -->
      <div class="bg-white p-4 sm:p-5 rounded-2xl border border-slate-200 shadow-sm space-y-3">
        <div class="flex flex-wrap items-center justify-between gap-2">
          <h3 class="text-sm font-bold text-slate-900 flex items-center gap-2">
            <i class="fa-solid fa-mobile-screen-button text-indigo-600"></i> Thiết bị phần cứng đã đăng ký
          </h3>
          <span id="deviceBoundBadge" class="text-[11px] font-semibold px-2 py-1 rounded-lg bg-slate-100 text-slate-600">Chưa ghi nhớ</span>
        </div>
        <p class="text-xs text-slate-500">Mỗi tài khoản chỉ dùng 1 thiết bị. Nếu bạn đổi điện thoại hoặc mất máy, hãy gửi đơn kèm ảnh chứng minh để Admin mở khóa.</p>
        <div class="p-3 rounded-xl bg-slate-50 border border-slate-200 font-mono text-xs text-slate-700 break-all" id="deviceBoundLabel">Chưa có mã thiết bị</div>
        <div id="deviceRequestStatus" class="text-[11px] text-slate-500"></div>
        <button onclick="openDeviceResetModal()" class="w-full py-2.5 bg-indigo-600 hover:bg-indigo-700 text-white rounded-xl text-sm font-semibold flex items-center justify-center gap-2">
          <i class="fa-solid fa-unlock-keyhole"></i> Yêu cầu reset thiết bị (nộp ảnh chứng minh)
        </button>
      </div>
    </section>
    <!-- ========================================== -->
    <section id="screen-4" class="screen-view hidden space-y-6">
      <div class="bg-white p-4 sm:p-5 rounded-2xl border border-slate-200 shadow-sm flex items-center justify-between">
        <div>
          <button onclick="switchScreen(2)" class="text-xs text-indigo-600 hover:underline flex items-center gap-1 font-semibold mb-1">
            <i class="fa-solid fa-arrow-left"></i> Back to Schedule
          </button>
          <h1 class="text-lg sm:text-xl font-bold text-slate-900">Submit Leave Application & Proof</h1>
          <p class="text-xs text-slate-500">Session 1 • Tue Nov 01, 2026 • [CS301] Software Engineering</p>
        </div>
      </div>

      <!-- Leave Grading Legend -->
      <div class="grid sm:grid-cols-2 gap-3">
        <div class="p-3 bg-purple-50 border border-purple-200 rounded-xl text-xs text-purple-900">
          <i class="fa-solid fa-stamp text-purple-600 mr-1"></i>
          <strong>Lecturer approves &rarr; Excused Absence:</strong> Awarded <strong>2 / 10</strong> session points.
        </div>
        <div class="p-3 bg-rose-50 border border-rose-200 rounded-xl text-xs text-rose-900">
          <i class="fa-solid fa-ban text-rose-600 mr-1"></i>
          <strong>Rejected / No proof &rarr; Unexcused Absence:</strong> Awarded <strong>0 / 10</strong> points.
        </div>
      </div>

      <!-- Submission Form -->
      <form onsubmit="handleLeaveSubmit(event)" class="bg-white p-4 sm:p-6 rounded-2xl border border-slate-200 shadow-sm space-y-5">
        <div>
          <label class="block text-xs font-bold uppercase text-slate-700 mb-1">1. Primary Reason Category *</label>
          <select id="leaveCategory" required class="w-full px-4 py-2.5 rounded-lg border border-slate-300 text-sm focus:ring-2 focus:ring-indigo-500 outline-none">
            <option value="Medical Illness / Hospitalization">Medical Illness / Hospitalization</option>
            <option value="Family Emergency">Family Emergency</option>
            <option value="University Official Convocations">University Official Convocations</option>
            <option value="Other Legitimate Reason">Other Legitimate Reason</option>
          </select>
        </div>

        <div>
          <label class="block text-xs font-bold uppercase text-slate-700 mb-1">2. Detailed Explanation *</label>
          <textarea id="leaveDetail" rows="3" required class="w-full px-4 py-2 rounded-lg border border-slate-300 text-sm focus:ring-2 focus:ring-indigo-500 outline-none"
                    placeholder="Provide explicit context for your instructor...">High fever and acute respiratory infection since last night. Admitted to University Medical Center.</textarea>
        </div>

        <div>
          <label class="block text-xs font-bold uppercase text-slate-700 mb-1">3. Digital Evidence Document (JPG, PNG, PDF) *</label>
          <input type="file" id="leaveFile" accept="image/jpeg,image/png,application/pdf,.jpg,.jpeg,.png,.pdf"
                 class="hidden" onchange="handleLeaveFileChange(this)">

          <div id="leaveDropZone" role="button" tabindex="0"
               onclick="document.getElementById('leaveFile').click()"
               onkeydown="if(event.key==='Enter'||event.key===' '){event.preventDefault();document.getElementById('leaveFile').click();}"
               ondragover="event.preventDefault();this.classList.add('border-indigo-500','bg-indigo-50');"
               ondragleave="this.classList.remove('border-indigo-500','bg-indigo-50');"
               ondrop="handleLeaveFileDrop(event)"
               class="border-2 border-dashed border-slate-300 hover:border-indigo-400 rounded-xl p-4 text-center cursor-pointer bg-slate-50 transition">

            <!-- Empty state -->
            <div id="leaveFileEmpty">
              <i class="fa-solid fa-cloud-arrow-up text-2xl text-indigo-500 mb-1"></i>
              <p class="text-xs font-semibold text-slate-700">Tap to choose a file, or drag &amp; drop it here</p>
              <p class="text-[11px] text-slate-400 mt-0.5">JPG, PNG or PDF &bull; maximum 5 MB</p>
            </div>

            <!-- Selected state -->
            <div id="leaveFileSelected" class="hidden text-left">
              <div class="flex items-center gap-3">
                <img id="leaveFileThumb" class="hidden w-11 h-11 rounded-lg object-cover border border-slate-200 shrink-0" alt="Selected file preview">
                <div id="leaveFileIcon" class="w-11 h-11 rounded-lg bg-indigo-100 text-indigo-600 flex items-center justify-center shrink-0">
                  <i class="fa-solid fa-file-lines text-lg"></i>
                </div>
                <div class="min-w-0 flex-1">
                  <p id="leaveFileName" onclick="previewLeaveFile(event)" title="Click to preview"
                     class="text-xs font-semibold text-slate-800 truncate hover:text-indigo-600 hover:underline">-</p>
                  <p id="leaveFileMeta" class="text-[11px] text-slate-400">-</p>
                </div>
                <button type="button" onclick="clearLeaveFile(event)" title="Remove file"
                        class="shrink-0 w-8 h-8 rounded-full bg-white border border-slate-200 text-slate-500 hover:text-rose-600 hover:border-rose-300 flex items-center justify-center">
                  <i class="fa-solid fa-xmark"></i>
                </button>
              </div>
              <p class="text-[11px] text-emerald-600 font-semibold mt-2 flex items-center gap-1">
                <i class="fa-solid fa-circle-check"></i> Attached &mdash; will be sent with your application
              </p>
            </div>
          </div>
          <p id="leaveFileError" class="hidden text-[11px] text-rose-600 font-semibold mt-1"></p>
        </div>

        <button type="submit" class="w-full py-3.5 bg-indigo-600 hover:bg-indigo-700 text-white rounded-xl font-bold text-sm shadow-md shadow-indigo-200 transition flex items-center justify-center gap-2">
          <i class="fa-solid fa-paper-plane"></i>
          <span>Transmit Leave Application to Instructor</span>
        </button>
      </form>
    </section>

    <!-- ========================================== -->
    <!-- SCREEN 5: ATTENDANCE SUMMARIES (WIRE 8.5) -->
    <!-- ========================================== -->
    <section id="screen-5" class="screen-view hidden space-y-6">
      <div class="bg-white p-4 sm:p-5 rounded-2xl border border-slate-200 shadow-sm flex flex-wrap items-center justify-between gap-3">
        <div>
          <h1 class="text-xl sm:text-2xl font-bold text-slate-900">Attendance Summary</h1>
          <p class="text-xs text-slate-500">Semester Overview & Full Course Chronological History</p>
        </div>
        <div class="flex gap-2">
          <button class="px-3 py-1.5 text-xs font-semibold rounded-lg bg-indigo-600 text-white">All Courses</button>
          <button class="px-3 py-1.5 text-xs font-semibold rounded-lg bg-slate-100 text-slate-600">This Course</button>
        </div>
      </div>

      <!-- All Courses Overview Table -->
      <div class="bg-white rounded-2xl border border-slate-200 shadow-sm overflow-hidden">
        <div class="px-5 py-3.5 bg-slate-50 border-b border-slate-200 flex justify-between items-center">
          <h3 class="text-xs font-bold uppercase tracking-wider text-slate-700">Semester Overview</h3>
          <span class="text-xs font-medium text-slate-500">Cumulative average: <strong class="text-indigo-600">89.2%</strong></span>
        </div>
        <div class="overflow-x-auto table-scroll">
          <table class="w-full text-left text-xs sm:text-sm">
            <thead class="bg-slate-50 text-slate-500 font-semibold uppercase text-[11px] border-b border-slate-200">
              <tr>
                <th class="py-3 px-3 sm:px-4">Course</th>
                <th class="py-3 px-3 sm:px-4 text-center hidden sm:table-cell">Taken</th>
                <th class="py-3 px-3 sm:px-4 text-center">Points</th>
                <th class="py-3 px-3 sm:px-4">Rate</th>
              </tr>
            </thead>
            <tbody class="divide-y divide-slate-100">
              <tr class="hover:bg-slate-50">
                <td class="py-3 px-3 sm:px-4 font-semibold text-slate-800">[CS301] Software Engineering</td>
                <td class="py-3 px-3 sm:px-4 text-center font-mono hidden sm:table-cell">5</td>
                <td class="py-3 px-3 sm:px-4 text-center font-mono font-medium">42 / 50</td>
                <td class="py-3 px-3 sm:px-4">
                  <div class="flex items-center gap-2">
                    <div class="w-24 bg-slate-200 h-2 rounded-full overflow-hidden">
                      <div class="bg-indigo-600 h-2 rounded-full" style="width: 84.0%"></div>
                    </div>
                    <span class="font-mono text-xs font-semibold text-slate-700">84.0%</span>
                  </div>
                </td>
              </tr>
              <tr class="hover:bg-slate-50">
                <td class="py-3 px-3 sm:px-4 font-semibold text-slate-800">[CS302] Computer Networks</td>
                <td class="py-3 px-3 sm:px-4 text-center font-mono hidden sm:table-cell">6</td>
                <td class="py-3 px-3 sm:px-4 text-center font-mono font-medium">55 / 60</td>
                <td class="py-3 px-3 sm:px-4">
                  <div class="flex items-center gap-2">
                    <div class="w-24 bg-slate-200 h-2 rounded-full overflow-hidden">
                      <div class="bg-indigo-600 h-2 rounded-full" style="width: 91.6%"></div>
                    </div>
                    <span class="font-mono text-xs font-semibold text-slate-700">91.6%</span>
                  </div>
                </td>
              </tr>
              <tr class="hover:bg-slate-50">
                <td class="py-3 px-3 sm:px-4 font-semibold text-slate-800">[CS303] Database Systems</td>
                <td class="py-3 px-3 sm:px-4 text-center font-mono hidden sm:table-cell">8</td>
                <td class="py-3 px-3 sm:px-4 text-center font-mono font-medium">80 / 80</td>
                <td class="py-3 px-3 sm:px-4">
                  <div class="flex items-center gap-2">
                    <div class="w-24 bg-slate-200 h-2 rounded-full overflow-hidden">
                      <div class="bg-emerald-600 h-2 rounded-full" style="width: 100%"></div>
                    </div>
                    <span class="font-mono text-xs font-semibold text-emerald-600">100%</span>
                  </div>
                </td>
              </tr>
              <tr class="hover:bg-slate-50">
                <td class="py-3 px-3 sm:px-4 font-semibold text-slate-800">[CS304] Algorithms Design</td>
                <td class="py-3 px-3 sm:px-4 text-center font-mono hidden sm:table-cell">7</td>
                <td class="py-3 px-3 sm:px-4 text-center font-mono font-medium">57 / 70</td>
                <td class="py-3 px-3 sm:px-4">
                  <div class="flex items-center gap-2">
                    <div class="w-24 bg-slate-200 h-2 rounded-full overflow-hidden">
                      <div class="bg-indigo-600 h-2 rounded-full" style="width: 81.4%"></div>
                    </div>
                    <span class="font-mono text-xs font-semibold text-slate-700">81.4%</span>
                  </div>
                </td>
              </tr>
            </tbody>
          </table>
        </div>
      </div>

      <!-- Detailed Session History for Selected Course -->
      <div class="bg-white rounded-2xl border border-slate-200 shadow-sm overflow-hidden">
        <div class="px-5 py-3.5 bg-slate-50 border-b border-slate-200 flex justify-between items-center">
          <h3 class="text-xs font-bold uppercase tracking-wider text-slate-700">[CS301] Session-by-Session Breakdown</h3>
          <span class="text-xs font-mono font-semibold text-slate-700">Total: 27 / 40 points (67.5%)</span>
        </div>
        <div class="overflow-x-auto table-scroll">
          <table class="w-full text-left text-xs sm:text-sm">
            <thead class="bg-slate-50 text-slate-500 font-semibold uppercase text-[11px] border-b border-slate-200">
              <tr>
                <th class="py-3 px-3 sm:px-4">Date</th>
                <th class="py-3 px-3 sm:px-4">Session</th>
                <th class="py-3 px-3 sm:px-4 text-center">Status</th>
                <th class="py-3 px-3 sm:px-4 text-center">Pts</th>
                <th class="py-3 px-3 sm:px-4 hidden sm:table-cell">Remarks</th>
              </tr>
            </thead>
            <tbody class="divide-y divide-slate-100">
              <tr>
                <td class="py-3 px-3 sm:px-4 font-mono whitespace-nowrap">04 Oct</td>
                <td class="py-3 px-3 sm:px-4 font-medium">W01 Intro/SRS</td>
                <td class="py-3 px-3 sm:px-4 text-center"><span class="badge-present px-2 py-0.5 rounded text-xs font-semibold">Present</span></td>
                <td class="py-3 px-4 text-center font-mono font-bold text-emerald-700">10/10</td>
                <td class="py-3 px-3 sm:px-4 text-xs text-slate-500 font-mono hidden sm:table-cell">06:55 (GPS)</td>
              </tr>
              <tr>
                <td class="py-3 px-3 sm:px-4 font-mono whitespace-nowrap">11 Oct</td>
                <td class="py-3 px-3 sm:px-4 font-medium">W02 DFD & UML</td>
                <td class="py-3 px-3 sm:px-4 text-center"><span class="badge-late px-2 py-0.5 rounded text-xs font-semibold">Late 18m</span></td>
                <td class="py-3 px-4 text-center font-mono font-bold text-amber-700">5/10</td>
                <td class="py-3 px-3 sm:px-4 text-xs text-slate-500 font-mono hidden sm:table-cell">07:18 (GPS)</td>
              </tr>
              <tr>
                <td class="py-3 px-3 sm:px-4 font-mono whitespace-nowrap">18 Oct</td>
                <td class="py-3 px-3 sm:px-4 font-medium">W03 Lab 01</td>
                <td class="py-3 px-3 sm:px-4 text-center"><span class="badge-present px-2 py-0.5 rounded text-xs font-semibold">Present</span></td>
                <td class="py-3 px-4 text-center font-mono font-bold text-emerald-700">10/10</td>
                <td class="py-3 px-3 sm:px-4 text-xs text-slate-500 font-mono hidden sm:table-cell">06:50 (GPS)</td>
              </tr>
              <tr>
                <td class="py-3 px-3 sm:px-4 font-mono whitespace-nowrap">25 Oct</td>
                <td class="py-3 px-3 sm:px-4 font-medium">W04 ERD Design</td>
                <td class="py-3 px-3 sm:px-4 text-center"><span class="badge-excused px-2 py-0.5 rounded text-xs font-semibold">Excused</span></td>
                <td class="py-3 px-4 text-center font-mono font-bold text-purple-700">2/10</td>
                <td class="py-3 px-3 sm:px-4 text-xs text-slate-500 hidden sm:table-cell">Medical slip</td>
              </tr>
              <tr>
                <td class="py-3 px-3 sm:px-4 font-mono whitespace-nowrap">01 Nov</td>
                <td class="py-3 px-3 sm:px-4 font-medium">W05 UI Mockups</td>
                <td class="py-3 px-3 sm:px-4 text-center"><span class="badge-absent px-2 py-0.5 rounded text-xs font-semibold">Absent</span></td>
                <td class="py-3 px-4 text-center font-mono font-bold text-rose-700">0/10</td>
                <td class="py-3 px-3 sm:px-4 text-xs text-slate-500 hidden sm:table-cell">Pending</td>
              </tr>
            </tbody>
          </table>
        </div>
      </div>
    </section>

    <!-- ========================================== -->
    <!-- SCREEN 6: LECTURER LIVE RADAR (WIRE 8.6) -->
    <!-- ========================================== -->
    <section id="screen-6" class="screen-view hidden space-y-6">
      <div class="bg-white p-4 sm:p-5 rounded-2xl border border-slate-200 shadow-sm flex flex-wrap items-center justify-between gap-3">
        <div>
          <div class="flex items-center gap-2">
            <span class="text-xs bg-indigo-100 text-indigo-700 font-bold px-2.5 py-0.5 rounded-full">Lecturer</span>
            <span class="text-xs bg-rose-100 text-rose-700 font-bold px-2 py-0.5 rounded-full flex items-center gap-1">
              <span class="w-1.5 h-1.5 rounded-full bg-rose-600 animate-pulse"></span> LIVE
            </span>
          </div>
          <h1 class="text-lg sm:text-xl font-bold text-slate-900 mt-1">Dr. Alexander Vance • CS301-G01</h1>
          <p class="text-xs text-slate-500">Active session: Room <span id="hdrRoom">B204</span> (07:00 – 09:20) • Window opened 06:45 • Geofence R = <span id="hdrRadius">35</span> m</p>
        </div>
        <div class="flex flex-wrap gap-2">
          <button id="btnEditZone" onclick="openGeofenceEditor()" class="px-3.5 py-2 bg-emerald-600 hover:bg-emerald-700 text-white rounded-xl text-xs font-semibold flex items-center gap-1.5 shadow-sm">
            <i class="fa-solid fa-draw-polygon"></i>
            <span>Set Check-in Zone</span>
          </button>
          <button onclick="switchScreen(7)" class="px-3.5 py-2 bg-indigo-600 hover:bg-indigo-700 text-white rounded-xl text-xs font-semibold flex items-center gap-1.5 shadow-sm">
            <i class="fa-solid fa-clipboard-check"></i>
            <span>Review Leave (<span id="pendingLeaveCount">0</span> pending)</span>
          </button>
          <button onclick="alert('Exported CS301 Attendance Gradebook to CS301_Attendance_2026.xlsx successfully!')" class="px-3.5 py-2 bg-white border border-slate-300 hover:bg-slate-50 text-slate-700 rounded-xl text-xs font-semibold flex items-center gap-1.5 shadow-sm">
            <i class="fa-solid fa-file-excel text-emerald-600"></i>
            <span>Export Gradebook</span>
          </button>
        </div>
      </div>

      <!-- Live Radar Canvas + Statistics Metrics -->
      <div class="grid md:grid-cols-2 gap-6">
        <!-- Live Geofence Canvas -->
        <div class="bg-white p-4 sm:p-6 rounded-2xl border border-slate-200 shadow-sm flex flex-col items-center">
          <h3 class="text-sm font-bold text-slate-800 mb-4">Live Classroom Radar (R = <span id="radarTitleRadius">35</span>m)</h3>
          
          <div class="relative w-56 h-56 sm:w-64 sm:h-64 flex items-center justify-center">
            <!-- Perimeter Boundary -->
            <div class="absolute inset-0 rounded-full border-2 border-dashed border-indigo-500 bg-indigo-50/20"></div>
            <!-- Center Classroom Pin -->
            <div class="absolute w-5 h-5 rounded-full bg-slate-900 shadow text-[9px] text-white flex items-center justify-center font-bold" title="Anchor: B204 Center">B</div>
            
            <!-- Student dots are rendered from real check-in records -->
            <div id="radarDots" class="absolute inset-0"></div>
            <div id="radarEmpty" class="absolute inset-0 hidden items-center justify-center">
              <span class="text-[11px] text-slate-400 font-semibold text-center px-8">No check-ins yet</span>
            </div>
          </div>

          <div class="mt-4 flex flex-wrap items-center justify-center gap-x-4 gap-y-2 text-xs font-medium">
            <span class="flex items-center gap-1.5"><span class="w-3 h-3 rounded-full bg-emerald-500"></span> On-Time (10)</span>
            <span class="flex items-center gap-1.5"><span class="w-3 h-3 rounded-full bg-amber-500"></span> Late (5)</span>
            <span class="flex items-center gap-1.5"><span class="w-3 h-3 rounded-full bg-rose-600"></span> Outside Perimeter</span>
          </div>
        </div>

        <!-- Session Real-time Statistics -->
        <div class="bg-white p-4 sm:p-6 rounded-2xl border border-slate-200 shadow-sm flex flex-col justify-between gap-4">
          <div>
            <h3 class="text-sm font-bold text-slate-800 mb-4">Session Statistics (<span id="statCheckedIn">0</span> checked in)</h3>
            <div class="space-y-3">
              <div class="flex items-center justify-between p-2.5 rounded-xl bg-emerald-50 border border-emerald-100">
                <span class="text-xs font-semibold text-emerald-800 flex items-center gap-2">
                  <span class="w-2.5 h-2.5 rounded-full bg-emerald-500"></span> Present on-time (10 pts)
                </span>
                <span id="statPresent" class="font-mono font-bold text-emerald-900 text-sm">0</span>
              </div>
              <div class="flex items-center justify-between p-2.5 rounded-xl bg-amber-50 border border-amber-100">
                <span class="text-xs font-semibold text-amber-800 flex items-center gap-2">
                  <span class="w-2.5 h-2.5 rounded-full bg-amber-500"></span> Late arrival (5 pts)
                </span>
                <span id="statLate" class="font-mono font-bold text-amber-900 text-sm">0</span>
              </div>
              <div class="flex items-center justify-between p-2.5 rounded-xl bg-purple-50 border border-purple-100">
                <span class="text-xs font-semibold text-purple-800 flex items-center gap-2">
                  <span class="w-2.5 h-2.5 rounded-full bg-purple-500"></span> Excused absence (2 pts)
                </span>
                <span id="statExcused" class="font-mono font-bold text-purple-900 text-sm">0</span>
              </div>
              <div class="flex items-center justify-between p-2.5 rounded-xl bg-rose-50 border border-rose-100">
                <span class="text-xs font-semibold text-rose-800 flex items-center gap-2">
                  <span class="w-2.5 h-2.5 rounded-full bg-rose-500"></span> Rejected / outside perimeter (0 pts)
                </span>
                <span id="statRejected" class="font-mono font-bold text-rose-900 text-sm">0</span>
              </div>
            </div>
          </div>

          <div class="pt-4 border-t border-slate-100 flex items-center justify-between">
            <span class="text-xs font-bold text-slate-600">Average score of check-ins:</span>
            <span id="statAverage" class="text-xl font-black font-mono text-indigo-600">--</span>
          </div>
        </div>
      </div>

      <!-- Latest Check-ins Stream -->
      <div class="bg-white rounded-2xl border border-slate-200 shadow-sm p-4 sm:p-5">
        <h3 class="text-xs font-bold uppercase text-slate-700 mb-3 tracking-wider">Latest Live Check-in Telemetry</h3>
        <div id="telemetryList" class="space-y-2 text-xs font-mono">
          <div class="p-2 bg-slate-50 rounded-lg text-slate-400">No check-ins recorded yet.</div>
        </div>
      </div>
    </section>

    <!-- ========================================== -->
    <!-- SCREEN 7: LEAVE ADJUDICATION (WIRE 8.7) -->
    <!-- ========================================== -->
    <section id="screen-7" class="screen-view hidden space-y-6">
      <div class="bg-white p-4 sm:p-5 rounded-2xl border border-slate-200 shadow-sm flex items-center justify-between">
        <div>
          <button onclick="switchScreen(6)" class="text-xs text-indigo-600 hover:underline flex items-center gap-1 font-semibold mb-1">
            <i class="fa-solid fa-arrow-left"></i> Back to Radar
          </button>
          <h1 class="text-lg sm:text-xl font-bold text-slate-900">Leave Application Review & Adjudication</h1>
          <p class="text-xs text-slate-500">[CS301-G01] Software Engineering &bull; Queue: <span id="reviewQueueCount">0</span> Pending Ticket(s)</p>
        </div>
        <button onclick="loadLeaveQueue()" class="text-xs font-semibold text-indigo-600 hover:underline flex items-center gap-1 shrink-0">
          <i class="fa-solid fa-rotate"></i> Refresh
        </button>
      </div>

      <!-- Empty state: no pending applications -->
      <div id="reviewEmpty" class="hidden bg-white p-8 rounded-2xl border border-slate-200 shadow-sm text-center">
        <i class="fa-solid fa-inbox text-3xl text-slate-300 mb-2"></i>
        <p class="text-sm font-semibold text-slate-700">No pending leave applications</p>
        <p class="text-xs text-slate-500 mt-1">New submissions from students will appear here automatically.</p>
      </div>

      <!-- Review Card (populated live from GET /api/lecturer/leave-queue) -->
      <div id="reviewCard" class="bg-white p-4 sm:p-6 rounded-2xl border border-slate-200 shadow-sm space-y-5">
        <div class="flex flex-wrap items-start justify-between gap-2 border-b border-slate-100 pb-3">
          <div>
            <span class="text-xs font-bold text-indigo-600 bg-indigo-50 px-2 py-0.5 rounded">Ticket #<span id="reviewTicketId">-</span></span>
            <h2 class="text-lg font-bold text-slate-900 mt-1">
              <span id="reviewStudentName">-</span>
              <span id="reviewStudentCode" class="text-sm font-normal text-slate-500"></span>
            </h2>
            <p class="text-xs text-slate-500">Session 1 &bull; Tuesday, Nov 01, 2026</p>
          </div>
          <span id="ticketStatusBadge" class="badge-late text-xs px-2.5 py-1 rounded-full font-bold">Pending Review</span>
        </div>

        <div>
          <label class="block text-xs font-bold uppercase text-slate-500 mb-1">Primary Reason Category:</label>
          <p id="reviewCategory" class="text-sm font-semibold text-indigo-700">-</p>
        </div>

        <div>
          <label class="block text-xs font-bold uppercase text-slate-500 mb-1">Student Explanation:</label>
          <p id="reviewExplanation" class="text-sm text-slate-800 bg-slate-50 p-3 rounded-xl border border-slate-200 whitespace-pre-line">-</p>
        </div>

        <div>
          <label class="block text-xs font-bold uppercase text-slate-500 mb-1">Attached Medical Proof Document:</label>
          <div class="p-3 bg-indigo-50/50 border border-indigo-200 rounded-xl flex flex-wrap items-center justify-between gap-2">
            <div class="flex items-center gap-2 text-xs font-semibold text-indigo-900 min-w-0">
              <i class="fa-solid fa-file-medical text-lg text-indigo-600 shrink-0"></i>
              <span id="reviewFileName" class="truncate">-</span>
            </div>
            <button onclick="previewReviewFile()" class="text-xs text-indigo-600 hover:underline font-bold shrink-0">
              Click to preview &rarr;
            </button>
          </div>
        </div>

        <!-- Instructor Adjudication Action -->
        <div class="grid sm:grid-cols-2 gap-3 pt-2">
          <button onclick="adjudicateLeave(true)" class="p-4 rounded-xl border-2 border-purple-400 bg-purple-50 hover:bg-purple-100 text-left transition">
            <div class="font-bold text-sm text-purple-900 flex items-center gap-1.5">
              <i class="fa-solid fa-circle-check text-purple-600"></i>
              <span>✔ Approve as Excused</span>
            </div>
            <p class="text-xs text-purple-700 mt-1">Recorded as EXCUSED &rarr; Awards <strong>2 / 10 pts</strong></p>
          </button>

          <button onclick="adjudicateLeave(false)" class="p-4 rounded-xl border-2 border-rose-300 bg-rose-50 hover:bg-rose-100 text-left transition">
            <div class="font-bold text-sm text-rose-900 flex items-center gap-1.5">
              <i class="fa-solid fa-circle-xmark text-rose-600"></i>
              <span>✖ Reject Application</span>
            </div>
            <p class="text-xs text-rose-700 mt-1">Recorded as UNEXCUSED &rarr; Awards <strong>0 / 10 pts</strong></p>
          </button>
        </div>

        <div>
          <label class="block text-xs font-bold uppercase text-slate-500 mb-1">Instructor Feedback Note:</label>
          <input type="text" id="instructorFeedback" value="Approved. Wishing you a speedy recovery." 
                 class="w-full px-4 py-2 rounded-lg border border-slate-300 text-sm outline-none focus:ring-2 focus:ring-indigo-500">
        </div>
      </div>
    </section>

    <!-- ========================================== -->
    <!-- SCREEN 8: ADMIN MASTER CONTROL (WIRE 8.8) -->
    <!-- ========================================== -->
    <section id="screen-8" class="screen-view hidden space-y-6">
      <div class="bg-white p-4 sm:p-5 rounded-2xl border border-slate-200 shadow-sm flex flex-wrap items-center justify-between gap-3">
        <div>
          <span class="text-xs bg-slate-900 text-white font-bold px-2.5 py-0.5 rounded-full">Administrator Portal</span>
          <h1 class="text-xl sm:text-2xl font-bold text-slate-900 mt-1">Admin Master Control Dashboard</h1>
          <p class="text-xs text-slate-500">Classroom Anchors • Device Locks • Security Audit Trail</p>
        </div>
        <div class="flex items-center gap-2 flex-wrap">
          <button id="btnAccountPanel" onclick="toggleAccountPanel()" class="px-3 py-1.5 bg-indigo-600 hover:bg-indigo-700 text-white rounded-lg text-xs font-semibold flex items-center gap-1 shadow-sm">
            <i class="fa-solid fa-user-gear"></i> Thêm / Xóa tài khoản
          </button>
          <button id="btnEditZoneAdmin" onclick="openGeofenceEditor()" class="px-3 py-1.5 bg-emerald-600 hover:bg-emerald-700 text-white rounded-lg text-xs font-semibold flex items-center gap-1 shadow-sm">
            <i class="fa-solid fa-draw-polygon"></i> Edit Check-in Zone
          </button>
          <span class="text-xs bg-emerald-50 text-emerald-700 font-semibold px-3 py-1.5 rounded-lg border border-emerald-200">
            <i class="fa-solid fa-shield-check"></i> System Integrity OK
          </span>
        </div>
      </div>

      <!-- 1. Classroom Directory & Geofence -->
      <div class="bg-white rounded-2xl border border-slate-200 shadow-sm p-4 sm:p-6 space-y-4">
        <div class="flex flex-wrap items-center justify-between gap-2">
          <h3 class="text-sm font-bold text-slate-900">1. Classroom Directory & Geofence Anchors</h3>
          <button onclick="calibrateRoom()" class="px-3 py-1.5 bg-indigo-50 text-indigo-700 hover:bg-indigo-100 rounded-lg text-xs font-semibold flex items-center gap-1 border border-indigo-200">
            <i class="fa-solid fa-crosshairs"></i> Calibrate GPS Anchor
          </button>
        </div>

        <div class="overflow-x-auto table-scroll">
          <table class="w-full text-left text-xs sm:text-sm">
            <thead class="bg-slate-50 text-slate-500 uppercase text-[11px]">
              <tr>
                <th class="py-2.5 px-2 sm:px-3">Room</th>
                <th class="py-2.5 px-2 sm:px-3 hidden sm:table-cell">Building</th>
                <th class="py-2.5 px-2 sm:px-3">Latitude</th>
                <th class="py-2.5 px-2 sm:px-3 hidden sm:table-cell">Longitude</th>
                <th class="py-2.5 px-2 sm:px-3">Radius</th>
              </tr>
            </thead>
            <tbody class="divide-y divide-slate-100 font-mono text-xs">
              <tr>
                <td class="py-2.5 px-2 sm:px-3 font-bold text-slate-800"><span id="adminRoomCode">B204</span></td>
                <td class="py-2.5 px-2 sm:px-3 font-sans hidden sm:table-cell"><span id="adminRoomBuilding">Bldg B (2nd flr)</span></td>
                <td class="py-2.5 px-2 sm:px-3 whitespace-nowrap"><span id="adminLat">10.823098</span></td>
                <td class="py-2.5 px-2 sm:px-3 hidden sm:table-cell"><span id="adminLng">106.629663</span></td>
                <td class="py-2.5 px-2 sm:px-3 text-indigo-600 font-bold"><span id="adminRadius">35</span> m</td>
              </tr>
              <tr>
                <td class="py-2.5 px-2 sm:px-3 font-bold text-slate-800">A301</td>
                <td class="py-2.5 px-2 sm:px-3 font-sans hidden sm:table-cell">Bldg A</td>
                <td class="py-2.5 px-2 sm:px-3 whitespace-nowrap">10.824150</td>
                <td class="py-2.5 px-2 sm:px-3 hidden sm:table-cell">106.630120</td>
                <td class="py-2.5 px-2 sm:px-3 text-indigo-600 font-bold">40 m</td>
              </tr>
            </tbody>
          </table>
        </div>
      </div>

      <!-- 2. Device Binding Management -->
      <div class="bg-white rounded-2xl border border-slate-200 shadow-sm p-4 sm:p-6 space-y-4">
        <h3 class="text-sm font-bold text-slate-900">2. Device Binding Management (Anti-Proxy Reset)</h3>
        <div id="deviceBindingsBody" class="space-y-2">
          <div class="p-3 text-xs text-slate-500">Đang tải danh sách thiết bị…</div>
        </div>
      </div>

      <!-- 3. Security Audit Logs -->
      <div class="bg-white rounded-2xl border border-slate-200 shadow-sm p-4 sm:p-6 space-y-4">
        <h3 class="text-sm font-bold text-slate-900">3. Security Audit Logs (Anti-Spoofing Trail)</h3>
        <div class="space-y-2 font-mono text-xs">
          <div class="p-3 rounded-xl bg-rose-50 border border-rose-200 flex items-start gap-2 text-rose-900">
            <span class="bg-rose-200 text-rose-800 px-1.5 py-0.5 rounded text-[10px] font-bold">BANNED</span>
            <span>06:55:10 • B22DCCN088 flagged for <strong>Mock Location</strong> injection. Check-in aborted.</span>
          </div>
          <div class="p-3 rounded-xl bg-amber-50 border border-amber-200 flex items-start gap-2 text-amber-900">
            <span class="bg-amber-200 text-amber-800 px-1.5 py-0.5 rounded text-[10px] font-bold">BLOCKED</span>
            <span>07:01:25 • Device DEV-IP15-9921 attempted 2nd check-in for B22DCCN099. Proxy attempt blocked.</span>
          </div>
        </div>
      </div>

      <!-- 4 + 5. Account management panel (hidden until the admin clicks "Thêm / Xóa tài khoản") -->
      <div id="accountPanel" class="hidden space-y-6">
      <!-- 4. Create New Account (Admin only) -->
      <div class="bg-white rounded-2xl border border-indigo-200 shadow-sm p-4 sm:p-6 space-y-4">
        <h3 class="text-sm font-bold text-slate-900">4. Create New Account</h3>
        <p class="text-xs text-slate-500 mb-3">Chỉ Admin mới tạo được tài khoản sinh viên, giảng viên hoặc admin mới.</p>
        <div class="space-y-2">
          <div class="p-3 bg-slate-50 border border-slate-200 rounded-xl">
            <span class="text-xs font-bold text-slate-800">Loại tài khoản</span>
            <select id="newAccountRole" onchange="syncUsernameToCode()" class="px-2 py-1 rounded text-slate-700 appearance-none bg-slate-100 focus:outline-none focus:bg-slate-200">
              <option value="STUDENT">Sinh viên (STUDENT)</option>
              <option value="LECTURER">Giảng viên (LECTURER)</option>
              <option value="ADMIN">Admin (ADMIN)</option>
            </select>
          </div>
          <div class="p-3 bg-slate-50 border border-slate-200 rounded-xl">
            <span class="text-xs font-bold text-slate-800">Mã đăng nhập (Username)</span>
            <input type="text" id="newAccountUsername" oninput="syncUsernameToCode()" class="w-full p-1 rounded border border-slate-300 text-slate-700" placeholder="VD: B22DCCN998 / GV102">
          </div>
          <div class="p-3 bg-slate-50 border border-slate-200 rounded-xl">
            <span class="text-xs font-bold text-slate-800">Mật khẩu</span>
            <input type="password" id="newAccountPassword" class="w-full p-1 rounded border border-slate-300 text-slate-700" placeholder="VD: password123">
          </div>
          <div class="p-3 bg-slate-50 border border-slate-200 rounded-xl">
            <span class="text-xs font-bold text-slate-800">Họ và tên</span>
            <input type="text" id="newAccountFullName" class="w-full p-1 rounded border border-slate-300 text-slate-700" placeholder="VD: Nguyễn Văn A">
          </div>
          <div class="p-3 bg-slate-50 border border-slate-200 rounded-xl">
            <span class="text-xs font-bold text-slate-800">Email</span>
            <input type="email" id="newAccountEmail" class="w-full p-1 rounded border border-slate-300 text-slate-700" placeholder="VD: vana@uni.edu">
          </div>
          <div class="p-3 bg-slate-50 border border-slate-200 rounded-xl">
            <span class="text-xs font-bold text-slate-800">Ngày sinh (ngày / tháng / năm)</span>
            <div class="grid grid-cols-3 gap-2 mt-1">
              <select id="newAccountDay" class="px-2 py-1.5 rounded border border-slate-300 text-slate-700 text-xs"></select>
              <select id="newAccountMonth" class="px-2 py-1.5 rounded border border-slate-300 text-slate-700 text-xs"></select>
              <select id="newAccountYear" class="px-2 py-1.5 rounded border border-slate-300 text-slate-700 text-xs"></select>
            </div>
          </div>
          <div class="p-3 bg-slate-50 border border-slate-200 rounded-xl">
            <span class="text-xs font-bold text-slate-800">Số điện thoại</span>
            <input type="tel" id="newAccountPhone" class="w-full p-1 rounded border border-slate-300 text-slate-700" placeholder="VD: 0987654321">
          </div>
          <div class="p-3 bg-slate-50 border border-slate-200 rounded-xl">
            <span id="codeFieldLabel" class="text-xs font-bold text-slate-800">MSSV (= Mã đăng nhập)</span>
            <input type="text" id="newAccountCode" readonly class="w-full p-1 rounded border border-slate-300 bg-slate-100 text-slate-600" placeholder="Tự động lấy từ Mã đăng nhập">
          </div>
          <button onclick="createNewAccount()" class="px-4 py-2 bg-indigo-600 hover:bg-indigo-700 text-white rounded-lg font-semibold mt-3 w-full">
            <i class="fa-solid fa-user-plus mr-2"></i> Tạo tài khoản
          </button>
          <div id="createAccountResult" class="mt-2 text-xs"></div>
        </div>
      </div>

      <!-- 5. Manage Existing Accounts (Admin only) -->
      <div class="bg-white rounded-2xl border border-indigo-200 shadow-sm p-4 sm:p-6 space-y-4">
        <h3 class="text-sm font-bold text-slate-900">5. Manage Existing Accounts</h3>
        <p class="text-xs text-slate-500 mb-3">Danh sách tài khoản hiện có trong hệ thống.</p>
        <div id="accountsTable" class="overflow-x-auto">
          <table class="w-full text-left text-xs sm:text-sm">
            <thead class="bg-slate-50 text-slate-500 uppercase text-[11px]">
              <tr>
                <th class="py-2.5 px-3">STT</th>
                <th class="py-2.5 px-3">Mã tài khoản</th>
                <th class="py-2.5 px-3">Họ và tên</th>
                <th class="py-2.5 px-3">Email</th>
                <th class="py-2.5 px-3">Số điện thoại</th>
                <th class="py-2.5 px-3">Ngày sinh</th>
                <th class="py-2.5 px-3">Vai trò</th>
                <th class="py-2.5 px-3">Hành động</th>
              </tr>
            </thead>
            <tbody id="accountsBody">
              <!-- Accounts will be loaded here by JavaScript -->
            </tbody>
          </table>
        </div>
      </div>
      </div>

      <!-- 6. Device Reset Requests (from students, with image proof) -->
      <div class="bg-white rounded-2xl border border-indigo-200 shadow-sm p-4 sm:p-6 space-y-4">
        <div class="flex flex-wrap items-center justify-between gap-2">
          <h3 class="text-sm font-bold text-slate-900">6. Yêu cầu mở khóa thiết bị (kèm minh chứng)</h3>
          <span id="deviceRequestCount" class="text-xs bg-amber-50 text-amber-700 border border-amber-200 font-semibold px-2.5 py-1 rounded-lg">0 chờ duyệt</span>
        </div>
        <p class="text-xs text-slate-500">Sinh viên gửi đơn khi đổi điện thoại / mất thiết bị. Admin duyệt sẽ tự động gỡ liên kết thiết bị cũ.</p>
        <div id="deviceRequestQueue" class="space-y-2">
          <div class="p-3 text-xs text-slate-500">Đang tải đơn yêu cầu…</div>
        </div>
      </div>
    </section>

  </main>

  <!-- DEVICE RESET REQUEST MODAL (student, requires image proof) -->
  <div id="deviceResetModal" class="fixed inset-0 z-50 hidden items-center justify-center bg-slate-900/70 backdrop-blur-sm p-3 sm:p-4">
    <div class="bg-white rounded-2xl shadow-2xl w-full max-w-lg max-h-[92vh] overflow-y-auto">
      <div class="flex items-start justify-between gap-3 p-4 sm:p-5 border-b border-slate-200">
        <div>
          <h3 class="text-base sm:text-lg font-bold text-slate-900 flex items-center gap-2">
            <i class="fa-solid fa-unlock-keyhole text-indigo-600"></i> Gửi đơn mở khóa thiết bị
          </h3>
          <p class="text-xs text-slate-500 mt-1">Áp dụng khi đổi điện thoại, mất hoặc hỏng thiết bị đã đăng ký.</p>
        </div>
        <button onclick="closeDeviceResetModal()" class="w-8 h-8 shrink-0 rounded-lg hover:bg-slate-100 text-slate-500 flex items-center justify-center" aria-label="Close">
          <i class="fa-solid fa-xmark"></i>
        </button>
      </div>
      <form onsubmit="handleDeviceResetSubmit(event)" class="p-4 sm:p-5 space-y-4">
        <div>
          <label class="block text-xs font-bold uppercase text-slate-500 mb-1">1. Lý do *</label>
          <select id="deviceResetReason" required class="w-full px-3 py-2.5 rounded-lg border border-slate-300 text-sm outline-none focus:ring-2 focus:ring-indigo-500">
            <option value="Đổi điện thoại mới">Đổi điện thoại mới</option>
            <option value="Mất / trộm thiết bị">Mất / trộm thiết bị</option>
            <option value="Thiết bị hỏng">Thiết bị hỏng</option>
            <option value="Lý do khác">Lý do khác</option>
          </select>
        </div>
        <div>
          <label class="block text-xs font-bold uppercase text-slate-500 mb-1">2. Giải trình chi tiết *</label>
          <textarea id="deviceResetExplanation" rows="3" required class="w-full px-3 py-2 rounded-lg border border-slate-300 text-sm outline-none focus:ring-2 focus:ring-indigo-500"
                    placeholder="Ví dụ: Đã đổi sang điện thoại mới, không còn dùng máy cũ..."></textarea>
        </div>
        <div>
          <label class="block text-xs font-bold uppercase text-slate-500 mb-1">3. Ảnh chứng minh (JPG / PNG) *</label>
          <input type="file" id="deviceResetFile" accept="image/jpeg,image/png,.jpg,.jpeg,.png" class="hidden" onchange="handleDeviceResetFile(this)">
          <div id="deviceResetDrop" role="button" tabindex="0"
               onclick="document.getElementById('deviceResetFile').click()"
               onkeydown="if(event.key==='Enter'||event.key===' '){event.preventDefault();document.getElementById('deviceResetFile').click();}"
               ondragover="event.preventDefault();this.classList.add('border-indigo-500','bg-indigo-50');"
               ondragleave="this.classList.remove('border-indigo-500','bg-indigo-50');"
               ondrop="deviceResetDropHandler(event)"
               class="border-2 border-dashed border-slate-300 hover:border-indigo-400 rounded-xl p-4 text-center cursor-pointer bg-slate-50 transition">
            <div id="deviceResetEmpty">
              <i class="fa-solid fa-cloud-arrow-up text-2xl text-indigo-500 mb-1"></i>
              <p class="text-xs font-semibold text-slate-700">Bấm để chọn ảnh hoặc kéo thả vào đây</p>
              <p class="text-[11px] text-slate-400 mt-0.5">JPG, PNG &bull; tối đa 5 MB</p>
            </div>
            <div id="deviceResetSelected" class="hidden text-left flex items-center gap-3">
              <img id="deviceResetThumb" class="w-12 h-12 rounded-lg object-cover border border-slate-200 shrink-0 hidden" alt="Preview">
              <div class="min-w-0 flex-1">
                <p id="deviceResetFileName" class="text-xs font-semibold text-slate-800 truncate">-</p>
                <p id="deviceResetFileMeta" class="text-[11px] text-slate-400">-</p>
              </div>
              <button type="button" onclick="clearDeviceResetFile(event)" class="w-8 h-8 rounded-full border border-slate-200 flex items-center justify-center text-slate-500 hover:text-rose-600">
                <i class="fa-solid fa-xmark"></i>
              </button>
            </div>
          </div>
          <p id="deviceResetFileError" class="hidden text-[11px] text-rose-600 font-semibold mt-1"></p>
        </div>
        <button type="submit" class="w-full py-3 bg-indigo-600 hover:bg-indigo-700 text-white rounded-xl text-sm font-bold flex items-center justify-center gap-2">
          <i class="fa-solid fa-paper-plane"></i> Gửi đơn tới Admin
        </button>
      </form>
    </div>
  </div>

  <!-- HARDWARE DEVICE CONSENT MODAL -->
  <div id="deviceConsentModal" class="fixed inset-0 z-50 hidden items-center justify-center bg-slate-900/70 backdrop-blur-sm p-3 sm:p-4">
    <div class="bg-white rounded-2xl shadow-2xl w-full max-w-md max-h-[92vh] overflow-y-auto">
      <div class="p-4 sm:p-5 border-b border-slate-200">
        <h3 class="text-base sm:text-lg font-bold text-slate-900 flex items-center gap-2">
          <i class="fa-solid fa-mobile-screen-button text-indigo-600"></i> Cho phép ghi nhớ thiết bị phần cứng
        </h3>
        <p class="text-xs text-slate-500 mt-1">Yêu cầu bảo mật trước khi điểm danh</p>
      </div>
      <div class="p-4 sm:p-5 space-y-3">
        <p class="text-sm text-slate-700 leading-relaxed">
          Để đảm bảo <strong>1 tài khoản chỉ điểm danh trên 1 thiết bị</strong>, hệ thống cần lưu một mã định danh ẩn danh của thiết bị này trên trình duyệt.
          Mã này giúp nhận diện và chặn hành vi <strong>điểm danh hộ</strong> khi thiết bị được dùng cho tài khoản thứ hai.
        </p>
        <div class="p-3 rounded-xl bg-slate-50 border border-slate-200">
          <span class="text-xs font-bold text-slate-500">Mã thiết bị sẽ được ghi nhớ:</span>
          <div id="consentDeviceId" class="font-mono text-sm font-bold text-indigo-700 mt-1 break-all">DEV-XXXXXXXX-XXXX</div>
        </div>
        <p class="text-[11px] text-slate-500">Bạn có thể thu hồi quyền này bất cứ lúc nào bằng cách xóa dữ liệu trang web (site data) trong trình duyệt.</p>
        <div class="grid grid-cols-2 gap-2 pt-2">
          <button onclick="rejectDeviceConsent()" class="py-2.5 px-3 bg-white border border-slate-300 hover:bg-slate-50 text-slate-700 rounded-xl text-sm font-semibold">
            Từ chối
          </button>
          <button onclick="acceptDeviceConsent()" class="py-2.5 px-3 bg-indigo-600 hover:bg-indigo-700 text-white rounded-xl text-sm font-semibold shadow-sm">
            <i class="fa-solid fa-check mr-1"></i> Đồng ý &amp; điểm danh
          </button>
        </div>
      </div>
    </div>
  </div>

  <!-- ========================================== -->
  <!-- CHECK-IN ZONE EDITOR (LECTURER / ADMIN ONLY) -->
  <!-- ========================================== -->
  <div id="geofenceModal" class="fixed inset-0 z-50 hidden items-center justify-center bg-slate-900/70 backdrop-blur-sm p-3 sm:p-4">
    <div class="bg-white rounded-2xl shadow-2xl w-full max-w-xl max-h-[92vh] overflow-y-auto">
      <div class="flex items-start justify-between gap-3 p-4 sm:p-5 border-b border-slate-200">
        <div>
          <h3 class="text-base sm:text-lg font-bold text-slate-900 flex items-center gap-2">
            <i class="fa-solid fa-draw-polygon text-emerald-600"></i> Check-in Zone (Geofence)
          </h3>
          <p class="text-xs text-slate-500 mt-0.5">Tap or drag on the map to move the circle. Only students inside the circle can check in.</p>
        </div>
        <button onclick="closeGeofenceEditor()" class="w-8 h-8 shrink-0 rounded-lg hover:bg-slate-100 text-slate-500 flex items-center justify-center" aria-label="Close">
          <i class="fa-solid fa-xmark"></i>
        </button>
      </div>

      <div class="p-4 sm:p-5 space-y-4">
        <div id="zoneRoleNotice" class="hidden text-xs font-semibold text-rose-700 bg-rose-50 border border-rose-200 rounded-xl p-3">
          <i class="fa-solid fa-lock mr-1"></i> Only a <strong>lecturer</strong> or an <strong>admin</strong> can change the check-in zone. Log in as GV101 or ADM001.
        </div>

        <!-- Interactive picker: the map centre is the classroom anchor -->
        <div id="zoneMap" class="relative w-full max-w-[320px] mx-auto aspect-square rounded-2xl bg-slate-50 border-2 border-slate-200 overflow-hidden cursor-crosshair select-none touch-none">
          <div class="absolute inset-0 opacity-60" style="background-image: linear-gradient(#e2e8f0 1px, transparent 1px), linear-gradient(90deg, #e2e8f0 1px, transparent 1px); background-size: 32px 32px;"></div>
          <div id="zoneCircle" class="absolute rounded-full border-2 border-emerald-500 bg-emerald-400/20 pointer-events-none" style="left:50%;top:50%;width:0;height:0;transform:translate(-50%,-50%);"></div>
          <div id="zoneCenter" class="absolute w-4 h-4 rounded-full bg-slate-900 ring-4 ring-white shadow pointer-events-none" style="left:50%;top:50%;transform:translate(-50%,-50%);"></div>
          <div class="absolute bottom-1.5 right-2 text-[10px] font-mono text-slate-400" id="zoneScaleLabel">200 m</div>
        </div>

        <div class="grid grid-cols-2 gap-3">
          <label class="block">
            <span class="text-[11px] font-bold uppercase tracking-wide text-slate-500">Latitude</span>
            <input id="zoneLat" type="number" step="0.000001" onchange="onZoneInputsChanged()" class="mt-1 w-full px-3 py-2 rounded-xl border border-slate-300 text-sm font-mono focus:outline-none focus:ring-2 focus:ring-emerald-400" />
          </label>
          <label class="block">
            <span class="text-[11px] font-bold uppercase tracking-wide text-slate-500">Longitude</span>
            <input id="zoneLng" type="number" step="0.000001" onchange="onZoneInputsChanged()" class="mt-1 w-full px-3 py-2 rounded-xl border border-slate-300 text-sm font-mono focus:outline-none focus:ring-2 focus:ring-emerald-400" />
          </label>
        </div>

        <div>
          <div class="flex items-center justify-between">
            <span class="text-[11px] font-bold uppercase tracking-wide text-slate-500">Radius</span>
            <span class="text-sm font-black font-mono text-emerald-700"><span id="zoneRadiusLabel">35</span> m</span>
          </div>
          <input id="zoneRadius" type="range" min="5" max="200" step="1" value="35" oninput="onZoneRadiusInput()" class="mt-2 w-full accent-emerald-600" />
        </div>

        <div class="flex flex-wrap gap-2">
          <button onclick="useMyLocation()" class="px-3 py-2 bg-slate-100 hover:bg-slate-200 text-slate-700 rounded-xl text-xs font-semibold flex items-center gap-1.5">
            <i class="fa-solid fa-location-crosshairs text-indigo-600"></i> Use my GPS
          </button>
          <button onclick="recenterZoneMap()" class="px-3 py-2 bg-slate-100 hover:bg-slate-200 text-slate-700 rounded-xl text-xs font-semibold flex items-center gap-1.5">
            <i class="fa-solid fa-arrows-to-dot text-slate-600"></i> Center on circle
          </button>
          <button onclick="resetZoneDraft()" class="px-3 py-2 bg-slate-100 hover:bg-slate-200 text-slate-700 rounded-xl text-xs font-semibold flex items-center gap-1.5">
            <i class="fa-solid fa-rotate-left text-slate-600"></i> Reset
          </button>
        </div>

        <div id="zoneStatus" class="text-xs font-medium text-slate-500"></div>

        <div class="flex flex-col sm:flex-row gap-2 pt-1">
          <button id="zoneSaveBtn" onclick="saveGeofence()" class="flex-1 py-2.5 px-4 bg-emerald-600 hover:bg-emerald-700 text-white rounded-xl text-sm font-bold flex items-center justify-center gap-2 shadow-sm disabled:opacity-50 disabled:cursor-not-allowed">
            <i class="fa-solid fa-floppy-disk"></i> Save check-in zone
          </button>
          <button onclick="closeGeofenceEditor()" class="py-2.5 px-4 bg-white border border-slate-300 hover:bg-slate-50 text-slate-700 rounded-xl text-sm font-semibold">Cancel</button>
        </div>
      </div>
    </div>
  </div>

  <!-- Footer -->
  <footer class="bg-white border-t border-slate-200 py-4 px-4 text-center text-xs text-slate-500" style="padding-bottom: calc(1rem + env(safe-area-inset-bottom));">
    Capstone Software Engineering Project (Group_01) • Specification v2.0 • Implemented in C++ &amp; Web Standards
  </footer>

  <!-- Client JavaScript Logic -->
  <script>
    let currentTab = "Student";
    let activeUser = "Guest";
    let activeRole = "GUEST";
    let activeToken = "";
    let activeUsername = "";
    let activeStudentCode = "";

    function selectAuthTab(role) {
      currentTab = role;
      const btnStu = document.getElementById("authTabStudent");
      const btnLec = document.getElementById("authTabLecturer");
      const lblUser = document.getElementById("lblUsername");
      const txtUser = document.getElementById("loginUsername");

      if (role === "Student") {
        btnStu.className = "py-2 text-sm font-semibold rounded-lg bg-white shadow-sm text-indigo-700 transition";
        btnLec.className = "py-2 text-sm font-medium rounded-lg text-slate-600 hover:text-slate-900 transition";
        lblUser.innerText = "Student ID (MSSV)";
        txtUser.value = "B22DCCN001";
      } else {
        btnLec.className = "py-2 text-sm font-semibold rounded-lg bg-white shadow-sm text-indigo-700 transition";
        btnStu.className = "py-2 text-sm font-medium rounded-lg text-slate-600 hover:text-slate-900 transition";
        lblUser.innerText = "Lecturer Code (MGV)";
        txtUser.value = "GV101";
      }
    }

    // Shared tab styling (kept in sync with the header markup)
    const TAB_BASE = "shrink-0 px-3 py-2 rounded-lg transition";
    const TAB_ACTIVE = TAB_BASE + " bg-indigo-600 text-white shadow-sm";
    const TAB_IDLE = TAB_BASE + " text-slate-600 hover:text-slate-900";

    function switchScreen(idx) {
      document.querySelectorAll(".screen-view").forEach(el => el.classList.add("hidden"));
      const target = document.getElementById("screen-" + idx);
      if (target) target.classList.remove("hidden");

      for (let i = 1; i <= 8; i++) {
        const btn = document.getElementById("tab-" + i);
        if (btn) btn.className = (i === idx) ? TAB_ACTIVE : TAB_IDLE;
      }

      // Mobile: horizontally center the active tab inside the scrollable tab bar
      const nav = document.getElementById("mainNav");
      const activeTab = document.getElementById("tab-" + idx);
      if (nav && activeTab) {
        const left = activeTab.offsetLeft - (nav.clientWidth - activeTab.offsetWidth) / 2;
        nav.scrollTo({ left: Math.max(0, left), behavior: "smooth" });
      }

      // Mobile: jump back to the top of the newly opened screen
      window.scrollTo({ top: 0, behavior: "smooth" });

      // Lecturer Review screen always reloads the live queue from the server
      if (idx === 7) loadLeaveQueue();
      if (idx === 6) { loadSessionRoster(); loadGeofence(); }
      if (idx === 8) { loadGeofence(); loadDeviceBindings(); loadDeviceRequests(); }
      if (idx === 3) updateDeviceCard();
      if (idx === 2) loadNetworkStatus();
      if (idx === 6 || idx === 8) refreshZoneButtons();
    }

    async function handleLogin(e) {
      e.preventDefault();
      const username = document.getElementById("loginUsername").value.trim();
      const password = document.getElementById("loginPassword").value.trim();

      try {
        const res = await fetch("/api/login", {
          method: "POST",
          headers: { "Content-Type": "application/json" },
          body: JSON.stringify({ username, password, tab: currentTab })
        });
        const data = await res.json();
        if (data.success) {
          activeUsername = data.username || username;
          activeStudentCode = username; // username chinh la MSSV
          activeUser = data.full_name + " (" + data.role + ")";
          activeRole = data.role;
          activeToken = data.token || "";
          document.getElementById("activeUserBadge").innerText = activeUser;
          if (data.role === "ADMIN") {
            alert("Universal Admin Auto-Routing triggered! Welcome Administrator.");
            switchScreen(8);
          } else if (data.role === "LECTURER") {
            switchScreen(6);
          } else {
            switchScreen(2);
          }
        } else {
          alert("Login error: " + data.error);
        }
      } catch (err) {
        // Fallback for direct offline simulation
        activeUsername = username;
        activeStudentCode = username;
        if (username === "ADM001") {
          alert("Universal Admin Auto-Routing simulated! Welcome Administrator.");
          document.getElementById("activeUserBadge").innerText = "Administrator (ADMIN)";
          activeRole = "ADMIN";
          activeToken = "TOKEN_ADMIN_1";
          switchScreen(8);
        } else if (username === "GV101") {
          document.getElementById("activeUserBadge").innerText = "Dr. Alexander Vance (LECTURER)";
          activeRole = "LECTURER";
          activeToken = "TOKEN_LECTURER_1";
          switchScreen(6);
        } else {
          document.getElementById("activeUserBadge").innerText = "Alexander Tran (STUDENT)";
          activeRole = "STUDENT";
          activeToken = "TOKEN_STUDENT_1";
          switchScreen(2);
        }
      }
    }

    let isRealTimeMode = true;
    let serverTimeDeltaMs = 0;

    // null = unknown (checked at boot), true = on the classroom network
    let classroomNetworkOk = null;
    let classroomLanUrl = "";

    async function syncServerTime() {
      try {
        const clientReqMs = Date.now();
        const res = await fetch("/api/time");
        if (res.ok) {
          const data = await res.json();
          const clientRecvMs = Date.now();
          const roundTrip = clientRecvMs - clientReqMs;
          const serverEpoch = data.epoch_ms + (roundTrip / 2);
          serverTimeDeltaMs = serverEpoch - Date.now();
        }
      } catch (e) {
        serverTimeDeltaMs = 0;
      }
    }

    // ---- Session check-in window (single source of truth on the client) ----
    // Class CS301 starts at 07:00. On-time = [06:45, 07:15] (15 min before -> 15 min after),
    // late = (07:15, 07:45], closed after 07:45. Mirrors AttendanceGradingService.
    const SESSION_START_MIN = 7 * 60;                    // 07:00
    const CHECKIN_OPEN_MIN = SESSION_START_MIN - 15;     // 06:45
    const ONTIME_END_MIN   = SESSION_START_MIN + 15;     // 07:15
    const LATE_END_MIN     = SESSION_START_MIN + 45;     // 07:45

    let demoOffsetMs = 0;   // demo mode pins the clock near 06:52 so the window is open

    function getEffectiveNow() {
      return new Date(Date.now() + (isRealTimeMode ? serverTimeDeltaMs : demoOffsetMs));
    }

    function updateLiveClock() {
      const clockEl = document.getElementById("clock");
      const subClock = document.getElementById("serverTimeSubBanner");
      if (!clockEl) return;

      const now = getEffectiveNow();
      const timeStr = now.toLocaleTimeString();
      clockEl.innerText = timeStr + (isRealTimeMode ? " (NTP Live)" : " (Demo)");
      if (subClock) subClock.innerText = timeStr;

      updateCheckInWindow(now);
    }

    function updateCheckInWindow(now) {
      const badge = document.getElementById("checkinBadge");
      const badgeText = document.getElementById("checkinBadgeText");
      const box = document.getElementById("countdownBox");
      const timer = document.getElementById("countdownTimer");
      const label = document.getElementById("countdownLabel");
      const icon = document.getElementById("countdownIcon");
      const btn = document.getElementById("checkInBtn");
      const btnText = document.getElementById("checkInBtnText");
      if (!timer || !badge || !btn) return;

      const nowSec = now.getHours() * 3600 + now.getMinutes() * 60 + now.getSeconds();
      const openSec = CHECKIN_OPEN_MIN * 60;
      const onTimeEndSec = ONTIME_END_MIN * 60;
      const lateEndSec = LATE_END_MIN * 60;

      const fmt = (secs) => {
        const s = Math.max(0, Math.floor(secs));
        const h = Math.floor(s / 3600);
        const m = Math.floor((s % 3600) / 60);
        const r = s % 60;
        return String(h).padStart(2, "0") + ":" + String(m).padStart(2, "0") + ":" + String(r).padStart(2, "0");
      };

      const setBadge = (cls, text) => { badge.className = cls; badgeText.innerText = text; };
      const setBox = (cls) => { box.className = cls; };
      const setBtn = (enabled, text) => {
        const blocked = (classroomNetworkOk === false);
        btn.disabled = !enabled || blocked;
        btnText.innerText = blocked ? "📵 CONNECT TO THE CLASSROOM WI-FI" : text;
      };

      if (nowSec < openSec) {
        // Too early: window has not opened yet
        setBadge("mb-3 sm:mb-0 sm:absolute sm:top-0 sm:right-0 inline-flex bg-slate-500 text-white text-[11px] sm:text-xs font-bold px-3 sm:px-4 py-1 rounded-full sm:rounded-tr-none sm:rounded-bl-xl uppercase tracking-wider items-center gap-1.5",
                 "⏳ Check-in opens 06:45 AM");
        setBox("my-4 sm:my-6 p-3 sm:p-4 rounded-xl bg-slate-100 border border-slate-300 flex flex-wrap items-center justify-center gap-x-3 gap-y-1 text-slate-600 font-mono text-2xl sm:text-3xl font-extrabold transition-colors");
        icon.className = "fa-solid fa-lock text-slate-400";
        timer.innerText = fmt(openSec - nowSec);
        label.innerText = "until the check-in window opens (06:45)";
        setBtn(false, "⏳ CHECK-IN OPENS AT 06:45 AM");
      } else if (nowSec <= onTimeEndSec) {
        // On-time window: 15 min before start -> 15 min after start
        setBadge("mb-3 sm:mb-0 sm:absolute sm:top-0 sm:right-0 inline-flex bg-emerald-500 text-white text-[11px] sm:text-xs font-bold px-3 sm:px-4 py-1 rounded-full sm:rounded-tr-none sm:rounded-bl-xl uppercase tracking-wider items-center gap-1.5",
                 "🟢 GPS Check-in Active (On-time)");
        setBox("my-4 sm:my-6 p-3 sm:p-4 rounded-xl bg-emerald-50 border border-emerald-200 flex flex-wrap items-center justify-center gap-x-3 gap-y-1 text-emerald-700 font-mono text-2xl sm:text-3xl font-extrabold transition-colors");
        icon.className = "fa-solid fa-hourglass-half text-amber-500";
        timer.innerText = fmt(onTimeEndSec - nowSec);
        label.innerText = "left for on-time score (10/10)";
        setBtn(true, "📍 CHECK-IN VIA GPS NOW");
      } else if (nowSec <= lateEndSec) {
        // Late window: 07:15 -> 07:45 (5 points)
        setBadge("mb-3 sm:mb-0 sm:absolute sm:top-0 sm:right-0 inline-flex bg-amber-500 text-white text-[11px] sm:text-xs font-bold px-3 sm:px-4 py-1 rounded-full sm:rounded-tr-none sm:rounded-bl-xl uppercase tracking-wider items-center gap-1.5",
                 "⚠️ Late Check-in (5/10)");
        setBox("my-4 sm:my-6 p-3 sm:p-4 rounded-xl bg-amber-50 border border-amber-300 flex flex-wrap items-center justify-center gap-x-3 gap-y-1 text-amber-700 font-mono text-2xl sm:text-3xl font-extrabold transition-colors");
        icon.className = "fa-solid fa-triangle-exclamation text-amber-600";
        timer.innerText = fmt(lateEndSec - nowSec);
        label.innerText = "left to check in late (5/10 points)";
        setBtn(true, "⚠️ CHECK-IN NOW (LATE - 5/10)");
      } else {
        // Window closed: counted as absent
        setBadge("mb-3 sm:mb-0 sm:absolute sm:top-0 sm:right-0 inline-flex bg-rose-600 text-white text-[11px] sm:text-xs font-bold px-3 sm:px-4 py-1 rounded-full sm:rounded-tr-none sm:rounded-bl-xl uppercase tracking-wider items-center gap-1.5",
                 "🔒 Check-in Closed");
        setBox("my-4 sm:my-6 p-3 sm:p-4 rounded-xl bg-rose-50 border border-rose-300 flex flex-wrap items-center justify-center gap-x-3 gap-y-1 text-rose-700 font-mono text-2xl sm:text-3xl font-extrabold transition-colors");
        icon.className = "fa-solid fa-lock text-rose-500";
        timer.innerText = "00:00:00";
        label.innerText = "check-in window closed at 07:45 AM — absent (0/10)";
        setBtn(false, "🔒 CHECK-IN WINDOW CLOSED");
      }
    }

    function toggleTimeMode() {
      isRealTimeMode = !isRealTimeMode;
      const btn = document.getElementById("btnTimeMode");
      const lbl = document.getElementById("timeModeLabel");
      if (!isRealTimeMode) {
        // Pin the demo clock to 06:52:00 (inside the on-time window) and let it tick
        const anchor = new Date();
        anchor.setHours(6, 52, 0, 0);
        demoOffsetMs = anchor.getTime() - Date.now();
      }
      if (isRealTimeMode) {
        btn.className = "px-2.5 py-1 rounded-lg text-xs font-semibold bg-emerald-100 text-emerald-800 border border-emerald-300 hover:bg-emerald-200 transition flex items-center gap-1.5 shadow-sm";
        lbl.innerText = "Live NTP Clock";
      } else {
        btn.className = "px-2.5 py-1 rounded-lg text-xs font-semibold bg-amber-100 text-amber-800 border border-amber-300 hover:bg-amber-200 transition flex items-center gap-1.5 shadow-sm";
        lbl.innerText = "Demo Mode (06:52)";
      }
      btn.setAttribute("aria-label", isRealTimeMode
        ? "Time mode: Live NTP. Tap to switch to Demo mode"
        : "Time mode: Demo. Tap to switch to Live NTP");
      updateLiveClock();
    }

    // Initialize clock
    syncServerTime();
    updateLiveClock();
    setInterval(updateLiveClock, 1000);
    setInterval(syncServerTime, 60000);

    function renderCheckInResult(data, sentTime) {
      const box = document.getElementById("checkinResultBox");
      const badge = document.getElementById("resultBadge");
      const title = document.getElementById("resultTitle");
      const remark = document.getElementById("resultRemark");
      const icon = document.getElementById("resultIcon");
      if (!box || !badge || !title || !remark || !icon) return;

      const dist = (data && data.distance_meters !== undefined)
        ? Number(data.distance_meters).toFixed(1) + " m"
        : "--";
      const status = (data && data.status) ? data.status : "ERROR";
      const points = (data && data.points !== undefined) ? data.points : 0;
      const detail = (data && (data.remarks || data.error)) ? (data.remarks || data.error) : "No response from the server.";
      const ok = !!(data && data.success);

      box.className = ok
        ? "p-4 sm:p-5 rounded-2xl bg-emerald-50 border-2 border-emerald-500 text-emerald-900 shadow-md"
        : "p-4 sm:p-5 rounded-2xl bg-rose-50 border-2 border-rose-500 text-rose-900 shadow-md";
      badge.className = ok
        ? "inline-block text-[10px] sm:text-xs uppercase font-extrabold tracking-wider text-emerald-700 bg-emerald-100 px-2.5 py-1 rounded-md"
        : "inline-block text-[10px] sm:text-xs uppercase font-extrabold tracking-wider text-rose-700 bg-rose-100 px-2.5 py-1 rounded-md";
      badge.innerText = ok ? "ATTENDANCE CONFIRMATION SUCCESS" : "CHECK-IN REJECTED";
      title.innerText = "Status: " + status + " • Points: " + points + " / 10";
      remark.innerText = "Remark: " + detail + " • Distance: " + dist + " • Timestamp: " + sentTime;
      icon.className = (ok ? "text-emerald-600" : "text-rose-600") + " text-2xl sm:text-3xl shrink-0";
      icon.innerHTML = ok
        ? '<i class="fa-solid fa-circle-check"></i>'
        : '<i class="fa-solid fa-circle-xmark"></i>';

      // Dedicated warning banner for proxy / device-misuse attempts.
      const warningBox = document.getElementById("proxyWarningBox");
      const warningDetail = document.getElementById("proxyWarningDetail");
      const detailText = ((data && (data.error || data.remarks)) || "").toString();
      const isProxy = !!(data && (data.proxy_attempt || detailText.indexOf("Cố tình điểm danh hộ") >= 0));
      if (warningBox && warningDetail) {
        warningBox.classList.toggle("hidden", !isProxy);
        if (isProxy) {
          warningDetail.innerText = (data.warning || detailText) +
            (data.device_id ? " (Thiết bị: " + data.device_id + ")" : "");
          badge.innerText = "CẢNH CÁO ĐIỂM DANH HỘ";
          title.innerText = "Status: BLOCKED • Points: 0 / 10";
        }
      }
      const devLabel = document.getElementById("deviceVerifiedLabel");
      if (devLabel) {
        const dev = (data && data.device_id) || getOrCreateDeviceId();
        devLabel.innerText = dev;
      }
    }

    // ---- Hardware device consent (1 account = 1 device) ----
    let pendingDeviceConsent = null;

    function getOrCreateDeviceId() {
      let id = "";
      try { id = localStorage.getItem("gps_device_id") || ""; } catch (e) { id = ""; }
      if (!id) {
        const bytes = new Uint8Array(4);
        if (window.crypto && window.crypto.getRandomValues) {
          window.crypto.getRandomValues(bytes);
        } else {
          for (let i = 0; i < bytes.length; i++) bytes[i] = Math.floor(Math.random() * 256);
        }
        const hex = Array.from(bytes, b => b.toString(16).padStart(2, "0").toUpperCase()).join("");
        id = "DEV-" + hex + "-" + Math.floor(1000 + Math.random() * 9000);
        try { localStorage.setItem("gps_device_id", id); } catch (e) { /* private mode */ }
      }
      return id;
    }

    function hasDeviceConsent() {
      try {
        return localStorage.getItem("gps_device_consent") === "1" &&
               !!localStorage.getItem("gps_device_id");
      } catch (e) { return false; }
    }

    function closeDeviceConsent() {
      const modal = document.getElementById("deviceConsentModal");
      if (!modal) return;
      modal.classList.add("hidden");
      modal.classList.remove("flex");
    }

    // Returns a Promise<boolean>: true only after the student accepts.
    function ensureDeviceConsent() {
      return new Promise(resolve => {
        if (hasDeviceConsent()) { resolve(true); return; }
        pendingDeviceConsent = resolve;
        const label = document.getElementById("consentDeviceId");
        if (label) label.innerText = getOrCreateDeviceId();
        const modal = document.getElementById("deviceConsentModal");
        if (!modal) { resolve(false); return; }
        modal.classList.remove("hidden");
        modal.classList.add("flex");
      });
    }

    function acceptDeviceConsent() {
      try { localStorage.setItem("gps_device_consent", "1"); } catch (e) { /* ignore */ }
      closeDeviceConsent();
      if (pendingDeviceConsent) { const r = pendingDeviceConsent; pendingDeviceConsent = null; r(true); }
    }

    function rejectDeviceConsent() {
      closeDeviceConsent();
      if (pendingDeviceConsent) { const r = pendingDeviceConsent; pendingDeviceConsent = null; r(false); }
    }

    async function triggerCheckIn() {
      // 1-Tap GPS Check-in
      const btn = document.getElementById("checkInBtn");
      if (btn && btn.disabled) return;

      const now = getEffectiveNow();
      const pad = (n) => String(n).padStart(2, "0");
      const demoTime = pad(now.getHours()) + ":" + pad(now.getMinutes()) + ":" + pad(now.getSeconds());
      let checkinTime = isRealTimeMode ? "NOW" : demoTime;
      let lat = 10.823150;
      let lon = 106.629700;
      let acc = 4.0;

      // Real HTML5 Geolocation request if in real-time mode
      if (isRealTimeMode && navigator.geolocation) {
        try {
          const pos = await new Promise((resolve, reject) => {
            navigator.geolocation.getCurrentPosition(resolve, reject, { enableHighAccuracy: true, timeout: 2500 });
          });
          lat = pos.coords.latitude;
          lon = pos.coords.longitude;
          acc = pos.coords.accuracy || 4.0;
        } catch (e) {
          // Uses classroom anchor coordinates if sensor is blocked on localhost desktop
          lat = 10.823150;
          lon = 106.629700;
        }
      }

      // Hardware-device consent: the check-in is only sent after the student
      // accepts that this device is remembered (1 account = 1 device).
      const deviceAllowed = await ensureDeviceConsent();
      if (!deviceAllowed) {
        const devLabel = document.getElementById("deviceVerifiedLabel");
        if (devLabel) devLabel.innerText = "Chưa được cấp quyền";
        renderCheckInResult({
          success: false, status: "DENIED", points: 0, distance_meters: 0,
          device_id: getOrCreateDeviceId(),
          remarks: "Bạn chưa đồng ý cho hệ thống ghi nhớ thiết bị phần cứng. Điểm danh đã bị hủy."
        }, demoTime);
        switchScreen(3);
        return;
      }

      const deviceId = getOrCreateDeviceId();
      const studentCode = (activeRole === "STUDENT" && activeStudentCode)
        ? activeStudentCode : "B22DCCN001";
      const devLabel = document.getElementById("deviceVerifiedLabel");
      if (devLabel) devLabel.innerText = deviceId;

      try {
        const res = await fetch("/api/student/checkin", {
          method: "POST",
          headers: { "Content-Type": "application/json" },
          body: JSON.stringify({
            student_code: studentCode,
            client_lat: lat,
            client_long: lon,
            accuracy: acc,
            device_id: deviceId,
            checkin_time: checkinTime
          })
        });
        const data = await res.json();
        if (data && data.distance_meters !== undefined) {
          const distLabel = document.getElementById("measuredDistLabel");
          if (distLabel) distLabel.innerText = Number(data.distance_meters).toFixed(1) + " m";
        }
        renderCheckInResult(data, demoTime);
        switchScreen(3);
      } catch (err) {
        renderCheckInResult({ success: false, status: "ERROR", points: 0, remarks: "Network error: could not reach the server." }, demoTime);
        switchScreen(3);
      }
    }

    // ============================================================
    // LEAVE PROOF FILE ATTACHMENT (real <input type="file">)
    // ============================================================
    const LEAVE_MAX_BYTES = 5 * 1024 * 1024;               // 5 MB
    const LEAVE_ALLOWED_EXT = ["jpg", "jpeg", "png", "pdf"];
    const LEAVE_ALLOWED_MIME = ["image/jpeg", "image/png", "application/pdf"];
    let leaveFile = null;
    let leaveFileUrl = null;

    function formatFileSize(bytes) {
      if (bytes >= 1024 * 1024) return (bytes / (1024 * 1024)).toFixed(2) + " MB";
      if (bytes >= 1024) return (bytes / 1024).toFixed(0) + " KB";
      return bytes + " B";
    }

    function showLeaveFileError(msg) {
      const box = document.getElementById("leaveFileError");
      box.innerText = msg;
      box.classList.remove("hidden");
    }

    function hideLeaveFileError() {
      document.getElementById("leaveFileError").classList.add("hidden");
    }

    function setLeaveFile(file) {
      hideLeaveFileError();

      const name = file.name || "document";
      const ext = name.indexOf(".") >= 0 ? name.split(".").pop().toLowerCase() : "";
      const typeOk = LEAVE_ALLOWED_MIME.indexOf(file.type) >= 0 ||
                     (file.type === "" && LEAVE_ALLOWED_EXT.indexOf(ext) >= 0);

      if (!typeOk) {
        showLeaveFileError("Unsupported file type. Please choose a JPG, PNG or PDF file.");
        return;
      }
      if (file.size > LEAVE_MAX_BYTES) {
        showLeaveFileError("File is too large (" + formatFileSize(file.size) + "). Maximum allowed size is 5 MB.");
        return;
      }

      if (leaveFileUrl) URL.revokeObjectURL(leaveFileUrl);
      leaveFile = file;
      leaveFileUrl = URL.createObjectURL(file);

      document.getElementById("leaveFileName").innerText = name;
      document.getElementById("leaveFileMeta").innerText =
        formatFileSize(file.size) + " \u2022 " + (ext === "pdf" ? "PDF document" : "Image");

      const thumb = document.getElementById("leaveFileThumb");
      const icon = document.getElementById("leaveFileIcon");
      if (file.type.indexOf("image/") === 0) {
        thumb.src = leaveFileUrl;
        thumb.classList.remove("hidden");
        icon.classList.add("hidden");
      } else {
        thumb.classList.add("hidden");
        icon.classList.remove("hidden");
        icon.innerHTML = '<i class="fa-solid fa-file-pdf text-lg"></i>';
      }

      document.getElementById("leaveFileEmpty").classList.add("hidden");
      document.getElementById("leaveFileSelected").classList.remove("hidden");
    }

    function handleLeaveFileChange(input) {
      if (input.files && input.files[0]) setLeaveFile(input.files[0]);
      input.value = "";   // allows re-selecting the same file
    }

    function handleLeaveFileDrop(e) {
      e.preventDefault();
      e.currentTarget.classList.remove("border-indigo-500", "bg-indigo-50");
      if (e.dataTransfer && e.dataTransfer.files && e.dataTransfer.files[0]) {
        setLeaveFile(e.dataTransfer.files[0]);
      }
    }

    function clearLeaveFile(e) {
      if (e) e.stopPropagation();
      if (leaveFileUrl) URL.revokeObjectURL(leaveFileUrl);
      leaveFile = null;
      leaveFileUrl = null;
      document.getElementById("leaveFileEmpty").classList.remove("hidden");
      document.getElementById("leaveFileSelected").classList.add("hidden");
      hideLeaveFileError();
    }

    function previewLeaveFile(e) {
      if (e) e.stopPropagation();
      if (leaveFileUrl) window.open(leaveFileUrl, "_blank");
    }

    async function handleLeaveSubmit(e) {
      e.preventDefault();

      if (!leaveFile) {
        showLeaveFileError("Please attach your evidence document (JPG, PNG or PDF) before submitting.");
        document.getElementById("leaveDropZone").classList.add("border-rose-400");
        return;
      }
      document.getElementById("leaveDropZone").classList.remove("border-rose-400");

      const cat = document.getElementById("leaveCategory").value;
      const detail = document.getElementById("leaveDetail").value;
      try {
        const res = await fetch("/api/student/leave-request", {
          method: "POST",
          headers: { "Content-Type": "application/json" },
          body: JSON.stringify({
            student_code: (activeRole === "STUDENT" && activeStudentCode) ? activeStudentCode : "B22DCCN001",
            category: cat,
            detail: detail,
            file_name: leaveFile.name,
            file_size: leaveFile.size,
            file_type: leaveFile.type || "application/octet-stream"
          })
        });
        const data = await res.json().catch(() => ({}));
        if (data && data.success === false) {
          alert("Submission failed. Please try again.");
          return;
        }
      } catch (err) {}

      alert("Leave application transmitted successfully to Dr. Alexander Vance!\nAttached: " +
            leaveFile.name + " (" + formatFileSize(leaveFile.size) + ")\nStatus: PENDING");
      switchScreen(5);
    }

    // ============================================================
    // LECTURER RADAR: only students who actually checked in
    // ============================================================
    const RADAR_MAX_PX = 100;   // radar circle radius in pixels (224px container)

    function rosterStatusInfo(status) {
      switch (status) {
        case "PRESENT":  return { label: "Present",  color: "bg-emerald-500", ring: "ring-emerald-200" };
        case "LATE":     return { label: "Late",     color: "bg-amber-500",   ring: "ring-amber-200" };
        case "EXCUSED":  return { label: "Excused",  color: "bg-purple-500",  ring: "ring-purple-200" };
        case "REJECTED": return { label: "Rejected", color: "bg-rose-600",    ring: "ring-rose-200" };
        default:         return { label: "Absent",   color: "bg-slate-400",   ring: "ring-slate-200" };
      }
    }

    async function loadSessionRoster() {
      let data;
      try {
        const res = await fetch("/api/lecturer/session-roster", { cache: "no-store" });
        data = await res.json();
      } catch (e) { return; }

      const recs = data.records || [];
      const counts = data.counts || {};

      document.getElementById("statCheckedIn").innerText = recs.length;
      document.getElementById("statPresent").innerText = counts.present || 0;
      document.getElementById("statLate").innerText = counts.late || 0;
      document.getElementById("statExcused").innerText = counts.excused || 0;
      document.getElementById("statRejected").innerText = counts.rejected || 0;
      document.getElementById("statAverage").innerText =
        recs.length ? (data.average_points || 0).toFixed(1) + " / 10" : "--";

      const pendingEl = document.getElementById("pendingLeaveCount");
      if (pendingEl) pendingEl.innerText = data.pending_leaves || 0;

      // ---- Radar dots placed from the real GPS coordinates ----
      const dots = document.getElementById("radarDots");
      const empty = document.getElementById("radarEmpty");
      dots.innerHTML = "";
      if (recs.length) {
        empty.classList.add("hidden");
        empty.classList.remove("flex");
      } else {
        empty.classList.remove("hidden");
        empty.classList.add("flex");
      }

      const anchorLat = data.anchor_lat || 10.823098;
      const anchorLong = data.anchor_long || 106.629663;
      const radius = data.radius || 35;
      const pxPerMeter = RADAR_MAX_PX / radius;

      recs.forEach((r) => {
        const dy = -((r.lat - anchorLat) * 110540) * pxPerMeter;
        const dx = ((r.long - anchorLong) * 111320 * Math.cos(anchorLat * Math.PI / 180)) * pxPerMeter;
        const dist = Math.sqrt(dx * dx + dy * dy);
        const k = (dist > RADAR_MAX_PX && dist > 0) ? (RADAR_MAX_PX / dist) : 1;
        const info = rosterStatusInfo(r.status);

        const dot = document.createElement("div");
        dot.className = "absolute w-3.5 h-3.5 rounded-full ring-2 -translate-x-1/2 -translate-y-1/2 " +
                        info.color + " " + info.ring;
        dot.style.left = "calc(50% + " + (dx * k).toFixed(1) + "px)";
        dot.style.top = "calc(50% + " + (dy * k).toFixed(1) + "px)";
        dot.title = r.student_name + " (" + r.student_code + ") - " + info.label +
                    " " + r.points + "/10, d = " + r.distance + " m";
        dots.appendChild(dot);
      });

      // ---- Telemetry stream (only real check-ins) ----
      const list = document.getElementById("telemetryList");
      list.innerHTML = "";
      if (!recs.length) {
        const none = document.createElement("div");
        none.className = "p-2 bg-slate-50 rounded-lg text-slate-400";
        none.textContent = "No check-ins recorded yet.";
        list.appendChild(none);
      }

      recs.forEach((r) => {
        const info = rosterStatusInfo(r.status);
        const row = document.createElement("div");
        row.className = "p-2 bg-slate-50 rounded-lg flex flex-wrap items-center justify-between gap-x-2 gap-y-0.5";

        const left = document.createElement("span");
        left.innerHTML = '<span class="text-slate-500">' + r.checkin_time + "</span> &bull; ";
        const strong = document.createElement("strong");
        strong.textContent = r.student_code + " " + r.student_name;
        left.appendChild(strong);
        left.appendChild(document.createTextNode(" → " + info.label + " (" + r.points + "/10)"));

        const right = document.createElement("span");
        right.className = "text-slate-500";
        right.textContent = "d = " + r.distance + " m";

        row.appendChild(left);
        row.appendChild(right);
        list.appendChild(row);
      });
    }

    // Keep the radar in sync while the lecturer screen is open
    setInterval(() => {
      const s6 = document.getElementById("screen-6");
      if (s6 && !s6.classList.contains("hidden")) loadSessionRoster();
    }, 5000);

    // ============================================================
    // LECTURER REVIEW: live queue of pending leave applications
    // ============================================================
    let currentLeaveRequestId = null;

    async function loadLeaveQueue() {
      const card = document.getElementById("reviewCard");
      const empty = document.getElementById("reviewEmpty");
      try {
        const res = await fetch("/api/lecturer/leave-queue", { cache: "no-store" });
        const data = await res.json();
        const list = (data && data.requests) ? data.requests : [];

        document.getElementById("reviewQueueCount").innerText = list.length;

        if (!list.length) {
          currentLeaveRequestId = null;
          card.classList.add("hidden");
          empty.classList.remove("hidden");
          return;
        }

        const r = list[0];   // newest submission first
        currentLeaveRequestId = r.request_id;

        document.getElementById("reviewTicketId").innerText = r.request_id;
        document.getElementById("reviewStudentName").innerText = r.student_name || "Unknown student";
        document.getElementById("reviewStudentCode").innerText = r.student_code ? "(" + r.student_code + ")" : "";
        document.getElementById("reviewCategory").innerText = r.category || "-";
        document.getElementById("reviewExplanation").innerText = r.detail ? '"' + r.detail + '"' : "-";

        const sizeTxt = (r.file_size && r.file_size > 0) ? " (" + formatFileSize(r.file_size) + ")" : "";
        document.getElementById("reviewFileName").innerText = (r.file_name || "No file attached") + sizeTxt;

        const badge = document.getElementById("ticketStatusBadge");
        badge.className = "badge-late text-xs px-2.5 py-1 rounded-full font-bold";
        badge.innerText = "Pending Review";

        card.classList.remove("hidden");
        empty.classList.add("hidden");
      } catch (err) {
        // Server unreachable: keep whatever is currently on screen
      }
    }

    function previewReviewFile() {
      const name = document.getElementById("reviewFileName").innerText;
      alert("Attached proof document: " + name +
            "\n\nThis demo server stores the file name and size. " +
            "The student can open the original file from their own device.");
    }

    async function adjudicateLeave(isApproved) {
      if (!currentLeaveRequestId) {
        alert("There is no pending leave application to review.");
        return;
      }
      const notes = document.getElementById("instructorFeedback").value;
      const studentName = document.getElementById("reviewStudentName").innerText;
      const requestId = currentLeaveRequestId;

      try {
        const res = await fetch("/api/lecturer/leave-adjudicate", {
          method: "POST",
          headers: { "Content-Type": "application/json" },
          body: JSON.stringify({
            request_id: requestId,
            is_approved: isApproved,
            notes: notes
          })
        });
        const data = await res.json().catch(() => ({}));
        if (data && data.success === false) {
          alert("Could not update this application. Please refresh and try again.");
          return;
        }
      } catch (e) {}

      const badge = document.getElementById("ticketStatusBadge");
      if (isApproved) {
        badge.className = "badge-excused text-xs px-2.5 py-1 rounded-full font-bold";
        badge.innerText = "Approved (Excused: 2/10)";
        alert("Ticket #" + requestId + " approved. " + studentName + " is recorded as EXCUSED (2 / 10 points).");
      } else {
        badge.className = "badge-absent text-xs px-2.5 py-1 rounded-full font-bold";
        badge.innerText = "Rejected (Unexcused: 0/10)";
        alert("Ticket #" + requestId + " rejected. " + studentName + " is recorded as UNEXCUSED (0 / 10 points).");
      }

      // Remove the adjudicated ticket from the live queue
      setTimeout(loadLeaveQueue, 400);
    }

    async function resetDeviceLock(mssv) {
      try {
        await fetch("/api/admin/device-reset", {
          method: "POST",
          headers: { "Content-Type": "application/json" },
          body: JSON.stringify({ student_code: mssv })
        });
      } catch (e) {}
      alert("Hardware device binding reset successfully for student " + mssv + ". Device lock cleared.");
      loadDeviceBindings();
    }

    // ============================================================
    // CHECK-IN ZONE (GEOFENCE) EDITOR — LECTURER / ADMIN ONLY
    // ============================================================
    const ZONE_MAP_SPAN_M = 200;         // metres across the picker
    const ZONE_LAT_M_PER_DEG = 110540;   // metres per degree of latitude
    const ZONE_LNG_M_PER_DEG = 111320;   // metres per degree of longitude (equator)

    let zoneDraft = { lat: 10.823098, lng: 106.629663, radius: 35 };
    let zoneBase = { lat: 10.823098, lng: 106.629663 };    // coordinate at the picker centre
    let zoneOriginal = { lat: 10.823098, lng: 106.629663, radius: 35 };
    let zoneLive = { lat: 10.823098, lng: 106.629663, radius: 35, room: "B204", building: "Bldg B" };
    let zoneDragging = false;

    function canManageZone() {
      return activeRole === "LECTURER" || activeRole === "ADMIN";
    }

    function refreshZoneButtons() {
      const allowed = canManageZone();
      ["btnEditZone", "btnEditZoneAdmin"].forEach((id) => {
        const b = document.getElementById(id);
        if (!b) return;
        b.classList.toggle("opacity-60", !allowed);
        b.title = allowed
          ? "Pick the circular check-in zone"
          : "Only a lecturer or an admin can change the check-in zone";
      });
    }

    function applyGeofence(z) {
      const prev = zoneLive;
      zoneLive = {
        lat: Number(z.latitude),
        lng: Number(z.longitude),
        radius: Number(z.radius),
        room: z.room || prev.room,
        building: z.building || prev.building
      };
      const set = (id, v) => { const el = document.getElementById(id); if (el) el.innerText = v; };
      set("hdrRoom", zoneLive.room);
      set("hdrRadius", zoneLive.radius.toFixed(0));
      set("radarTitleRadius", zoneLive.radius.toFixed(0));
      set("adminRoomCode", zoneLive.room);
      set("adminRoomBuilding", zoneLive.building);
      set("adminLat", zoneLive.lat.toFixed(6));
      set("adminLng", zoneLive.lng.toFixed(6));
      set("adminRadius", zoneLive.radius.toFixed(0));
      set("stuRoom", zoneLive.room);
      set("stuAnchorLat", zoneLive.lat.toFixed(6));
      set("stuAnchorLng", zoneLive.lng.toFixed(6));
      set("stuRadius", zoneLive.radius.toFixed(0));
    }

    async function loadGeofence() {
      try {
        const res = await fetch("/api/session/geofence", { cache: "no-store" });
        if (!res.ok) return;
        const z = await res.json();
        if (z && z.success) applyGeofence(z);
      } catch (e) {}
    }

    function setZoneStatus(text, kind) {
      const el = document.getElementById("zoneStatus");
      if (!el) return;
      el.innerText = text || "";
      el.className = "text-xs font-medium " +
        (kind === "ok" ? "text-emerald-700" : kind === "err" ? "text-rose-700" : "text-slate-500");
    }

    function syncZoneInputs() {
      const lat = document.getElementById("zoneLat");
      const lng = document.getElementById("zoneLng");
      const rad = document.getElementById("zoneRadius");
      if (lat) lat.value = zoneDraft.lat.toFixed(6);
      if (lng) lng.value = zoneDraft.lng.toFixed(6);
      if (rad) rad.value = Math.round(zoneDraft.radius);
      const lbl = document.getElementById("zoneRadiusLabel");
      if (lbl) lbl.innerText = Math.round(zoneDraft.radius);
      renderZoneMap();
    }

    function renderZoneMap() {
      const map = document.getElementById("zoneMap");
      if (!map) return;
      const w = Math.max(1, map.clientWidth);
      const mpp = ZONE_MAP_SPAN_M / w;

      const eastM = (zoneDraft.lng - zoneBase.lng) * ZONE_LNG_M_PER_DEG * Math.cos(zoneBase.lat * Math.PI / 180);
      const northM = (zoneDraft.lat - zoneBase.lat) * ZONE_LAT_M_PER_DEG;
      const x = eastM / mpp;
      const y = -northM / mpp;

      const circle = document.getElementById("zoneCircle");
      const pin = document.getElementById("zoneCenter");
      const d = (2 * zoneDraft.radius) / mpp;

      if (circle) {
        circle.style.left = "calc(50% + " + x.toFixed(1) + "px)";
        circle.style.top = "calc(50% + " + y.toFixed(1) + "px)";
        circle.style.width = d.toFixed(1) + "px";
        circle.style.height = d.toFixed(1) + "px";
      }
      if (pin) {
        pin.style.left = "calc(50% + " + x.toFixed(1) + "px)";
        pin.style.top = "calc(50% + " + y.toFixed(1) + "px)";
      }
    }

    function zonePointFromEvent(ev) {
      const map = document.getElementById("zoneMap");
      const rect = map.getBoundingClientRect();
      const mpp = ZONE_MAP_SPAN_M / Math.max(1, rect.width);
      const mx = (ev.clientX - rect.left) - rect.width / 2;
      const my = (ev.clientY - rect.top) - rect.height / 2;
      return {
        lat: zoneBase.lat + (-my * mpp) / ZONE_LAT_M_PER_DEG,
        lng: zoneBase.lng + (mx * mpp) / (ZONE_LNG_M_PER_DEG * Math.cos(zoneBase.lat * Math.PI / 180))
      };
    }

    function initZoneMap() {
      const map = document.getElementById("zoneMap");
      if (!map || map.dataset.bound === "1") return;
      map.dataset.bound = "1";

      const move = (ev) => {
        if (!canManageZone()) return;
        const p = zonePointFromEvent(ev);
        zoneDraft.lat = p.lat;
        zoneDraft.lng = p.lng;
        syncZoneInputs();
      };

      map.addEventListener("pointerdown", (ev) => {
        if (!canManageZone()) {
          setZoneStatus("Read-only: log in as GV101 (lecturer) or ADM001 (admin) to move the circle.", "err");
          return;
        }
        zoneDragging = true;
        try { map.setPointerCapture(ev.pointerId); } catch (e) {}
        move(ev);
        ev.preventDefault();
      });
      map.addEventListener("pointermove", (ev) => {
        if (!zoneDragging) return;
        move(ev);
        ev.preventDefault();
      });
      map.addEventListener("pointerup", () => { zoneDragging = false; });
      map.addEventListener("pointercancel", () => { zoneDragging = false; });
    }

    function onZoneRadiusInput() {
      const rad = document.getElementById("zoneRadius");
      if (!rad) return;
      zoneDraft.radius = Number(rad.value);
      syncZoneInputs();
    }

    function onZoneInputsChanged() {
      const lat = parseFloat(document.getElementById("zoneLat").value);
      const lng = parseFloat(document.getElementById("zoneLng").value);
      if (!isNaN(lat) && lat >= -90 && lat <= 90) zoneDraft.lat = lat;
      if (!isNaN(lng) && lng >= -180 && lng <= 180) zoneDraft.lng = lng;
      syncZoneInputs();
    }

    function recenterZoneMap() {
      zoneBase = { lat: zoneDraft.lat, lng: zoneDraft.lng };
      syncZoneInputs();
    }

    function resetZoneDraft() {
      zoneDraft = { lat: zoneOriginal.lat, lng: zoneOriginal.lng, radius: zoneOriginal.radius };
      zoneBase = { lat: zoneOriginal.lat, lng: zoneOriginal.lng };
      syncZoneInputs();
      setZoneStatus("Reverted to the saved zone.", "muted");
    }

    function useMyLocation() {
      if (!navigator.geolocation) {
        setZoneStatus("Geolocation is not available in this browser.", "err");
        return;
      }
      setZoneStatus("Reading GPS…");
      navigator.geolocation.getCurrentPosition(
        (pos) => {
          zoneDraft.lat = pos.coords.latitude;
          zoneDraft.lng = pos.coords.longitude;
          zoneBase = { lat: pos.coords.latitude, lng: pos.coords.longitude };
          syncZoneInputs();
          setZoneStatus("Centred on your position (accuracy ±" + Math.round(pos.coords.accuracy || 0) + " m).", "ok");
        },
        () => setZoneStatus("Could not read GPS. Enter the coordinates manually.", "err"),
        { enableHighAccuracy: true, timeout: 4000 }
      );
    }

    async function openGeofenceEditor() {
      const modal = document.getElementById("geofenceModal");
      if (!modal) return;
      modal.classList.remove("hidden");
      modal.classList.add("flex");

      const allowed = canManageZone();
      document.getElementById("zoneRoleNotice").classList.toggle("hidden", allowed);
      document.getElementById("zoneSaveBtn").disabled = !allowed;
      const map = document.getElementById("zoneMap");
      map.classList.toggle("cursor-not-allowed", !allowed);
      map.classList.toggle("cursor-crosshair", allowed);

      await loadGeofence();
      zoneDraft = { lat: zoneLive.lat, lng: zoneLive.lng, radius: zoneLive.radius };
      zoneBase = { lat: zoneLive.lat, lng: zoneLive.lng };
      zoneOriginal = { lat: zoneLive.lat, lng: zoneLive.lng, radius: zoneLive.radius };
      initZoneMap();
      syncZoneInputs();
      setZoneStatus(allowed ? "" : "Read-only preview. Log in as GV101 (lecturer) or ADM001 (admin).",
                    allowed ? "muted" : "err");
    }

    function closeGeofenceEditor() {
      const modal = document.getElementById("geofenceModal");
      if (!modal) return;
      modal.classList.add("hidden");
      modal.classList.remove("flex");
    }

    async function saveGeofence() {
      if (!canManageZone()) {
        setZoneStatus("Forbidden: only a lecturer or an admin can change the zone.", "err");
        return;
      }
      const btn = document.getElementById("zoneSaveBtn");
      btn.disabled = true;
      setZoneStatus("Saving…");
      try {
        const res = await fetch("/api/lecturer/geofence", {
          method: "POST",
          headers: { "Content-Type": "application/json" },
          body: JSON.stringify({
            role: activeRole,
            token: activeToken,
            actor: activeUser,
            lat: zoneDraft.lat,
            long: zoneDraft.lng,
            radius: zoneDraft.radius
          })
        });
        const data = await res.json();
        if (res.ok && data.success) {
          applyGeofence(data);
          zoneOriginal = { lat: Number(data.latitude), lng: Number(data.longitude), radius: Number(data.radius) };
          setZoneStatus("Saved. R = " + Number(data.radius).toFixed(0) +
                        " m — only students inside the circle can check in.", "ok");
          loadSessionRoster();
        } else {
          setZoneStatus(data.error || "Could not save the zone.", "err");
        }
      } catch (e) {
        setZoneStatus("Network error while saving the zone.", "err");
      } finally {
        btn.disabled = !canManageZone();
      }
    }

    function calibrateRoom() {
      openGeofenceEditor();
    }

    // ============================================================
    // PHONE / LAN ACCESS + PWA
    // ============================================================
    async function loadPhoneAccess() {
      const text = document.getElementById("phoneAccessText");
      const link = document.getElementById("phoneAccessLink");
      const copyBtn = document.getElementById("phoneCopyBtn");
      if (!text || !link || !copyBtn) return;

      const host = location.hostname;
      const isLocal = host === "localhost" || host === "127.0.0.1" || host === "::1" || host === "";

      let hint = "";
      try {
        const res = await fetch("/api/network", { cache: "no-store" });
        if (res.ok) hint = (await res.json()).lan_hint || "";
      } catch (e) {}

      const show = (url, msg) => {
        text.innerText = msg;
        link.href = url;
        link.innerText = url;
        link.classList.remove("hidden");
        copyBtn.classList.remove("hidden");
      };

      if (!isLocal) {
        show(location.origin, "Đang truy cập qua mạng LAN — điện thoại dùng được.");
      } else if (hint) {
        show(hint, "Mở địa chỉ này trên điện thoại (cùng Wi-Fi):");
      } else {
        text.innerText = "Mở http://<IP-máy-tính>:8080 trên điện thoại. Chạy scripts/phone-url.ps1 để lấy IP chính xác.";
      }
    }

    // ============================================================
    // CLASSROOM NETWORK GATE (check-in only on the classroom Wi-Fi)
    // ============================================================
    async function loadNetworkStatus() {
      try {
        const res = await fetch("/api/network/status", { cache: "no-store" });
        const data = await res.json();
        classroomNetworkOk = !!data.classroom_network;
        classroomLanUrl = data.lan_url || classroomLanUrl;
        renderNetworkStatus(data);
        if (data.classroom_network && !data.via_proxy && !data.classroom_public_ip) {
          learnClassroomPublicIp();
        }
      } catch (e) {
        classroomNetworkOk = null;
      }
      if (typeof updateCheckInWindow === "function") updateCheckInWindow(getEffectiveNow());
    }

    function renderNetworkStatus(data) {
      const box = document.getElementById("wifiStatusBox");
      const text = document.getElementById("wifiStatusText");
      if (!box || !text) return;

      if (data.classroom_network) {
        box.className = "p-3 rounded-xl bg-emerald-50 border border-emerald-200 text-xs text-emerald-800 flex items-start gap-2";
        text.innerHTML = '<strong>Wi-Fi lớp học: đã kết nối</strong> — bạn có thể điểm danh. ' +
          '<span class="font-mono text-[11px] opacity-70">IP ' + (data.client_ip || "?") + '</span>';
      } else {
        const url = data.lan_url || classroomLanUrl || "";
        const link = url ? ' Hãy kết nối Wi-Fi của lớp rồi mở <a class="font-mono underline font-bold" href="' + url + '">' + url + '</a>.' : ' Hãy kết nối Wi-Fi của lớp để điểm danh.';
        box.className = "p-3 rounded-xl bg-rose-50 border border-rose-200 text-xs text-rose-800 flex items-start gap-2";
        text.innerHTML = '<strong>Chưa kết nối Wi-Fi lớp học</strong> — chỉ xem được thông tin, không thể điểm danh.' + link;
      }
    }

    // The classroom NAT public IP is learned once from a device on the LAN,
    // so an HTTPS tunnel (cloudflared) still accepts in-class check-ins while
    // rejecting phones on 4G or another Wi-Fi.
    async function learnClassroomPublicIp() {
      try {
        const ctrl = new AbortController();
        const timer = setTimeout(() => ctrl.abort(), 3000);
        const res = await fetch("https://api.ipify.org?format=json", { signal: ctrl.signal, cache: "no-store" });
        clearTimeout(timer);
        if (!res.ok) return;
        const data = await res.json();
        if (data && data.ip) {
          await fetch("/api/network/learn-public-ip", {
            method: "POST",
            headers: { "Content-Type": "application/json" },
            body: JSON.stringify({ ip: data.ip })
          });
        }
      } catch (e) {
        // Offline: the LAN-address check still works.
      }
    }

    // Show/hide the account management panel from the admin header button.
    function toggleAccountPanel() {
      const panel = document.getElementById("accountPanel");
      if (!panel) return;
      const willShow = panel.classList.contains("hidden");
      panel.classList.toggle("hidden");
      if (willShow) {
        initDobSelects();
        loadAccounts();
        panel.scrollIntoView({ behavior: "smooth", block: "start" });
      }
    }

    // MSSV / ma nhan vien trung voi ten dang nhap.
    function syncUsernameToCode() {
      const uEl = document.getElementById("newAccountUsername");
      const cEl = document.getElementById("newAccountCode");
      const rEl = document.getElementById("newAccountRole");
      const lEl = document.getElementById("codeFieldLabel");
      if (!uEl || !cEl) return;
      cEl.value = uEl.value.trim();
      if (lEl && rEl) {
        lEl.innerText = rEl.value === "STUDENT" ? "MSSV (= Mã đăng nhập)"
          : (rEl.value === "LECTURER" ? "Mã giảng viên (= Mã đăng nhập)" : "Mã admin (= Mã đăng nhập)");
      }
    }

    // Fill the birth-date selects (day / month / year) once on boot.
    function initDobSelects() {
      const day = document.getElementById("newAccountDay");
      const month = document.getElementById("newAccountMonth");
      const year = document.getElementById("newAccountYear");
      if (!day || !month || !year || day.options.length > 0) return;
      day.innerHTML = '<option value="">Ngày</option>' +
        Array.from({ length: 31 }, (_, i) => '<option value="' + String(i + 1).padStart(2, "0") + '">' + (i + 1) + '</option>').join("");
      month.innerHTML = '<option value="">Tháng</option>' +
        Array.from({ length: 12 }, (_, i) => '<option value="' + String(i + 1).padStart(2, "0") + '">Tháng ' + (i + 1) + '</option>').join("");
      year.innerHTML = '<option value="">Năm</option>' +
        Array.from({ length: 31 }, (_, i) => 1995 + i).map(y => '<option value="' + y + '">' + y + '</option>').join("");
    }

    function accountsResult(message, isError) {
      const box = document.getElementById("createAccountResult");
      if (!box) return;
      box.innerText = message;
      box.className = "mt-2 text-xs " + (isError ? "text-rose-600" : "text-emerald-600");
    }

    // Load every account from the server and render the management table.
    async function loadAccounts() {
      const tbody = document.getElementById("accountsBody");
      if (!tbody) return;
      try {
        const res = await fetch("/api/admin/get-accounts", { cache: "no-store" });
        const data = await res.json();
        const list = (data && data.accounts) ? data.accounts : [];
        if (list.length === 0) {
          tbody.innerHTML = '<tr><td colspan="8" class="py-4 text-center text-slate-500">Không có tài khoản nào trong hệ thống</td></tr>';
          return;
        }
        const roleLabels = { STUDENT: "Sinh viên", LECTURER: "Giảng viên", ADMIN: "Admin" };
        let rows = "";
        list.forEach((acc, idx) => {
          const dob = acc.date_of_birth ? acc.date_of_birth : "--";
          rows += '<tr class="border-b border-slate-200">' +
            '<td class="py-2.5 px-3 font-mono">' + (idx + 1) + '</td>' +
            '<td class="py-2.5 px-3 font-mono font-semibold">' + (acc.username || "--") + '</td>' +
            '<td class="py-2.5 px-3">' + (acc.full_name || "--") + '</td>' +
            '<td class="py-2.5 px-3">' + (acc.email || "--") + '</td>' +
            '<td class="py-2.5 px-3">' + (acc.phone || "--") + '</td>' +
            '<td class="py-2.5 px-3">' + dob + '</td>' +
            '<td class="py-2.5 px-3">' + (roleLabels[acc.role] || acc.role || "--") + '</td>' +
            '<td class="py-2.5 px-3">' +
              '<button onclick="deleteAccount(\'' + acc.username + '\')" class="text-rose-600 hover:text-rose-900 text-xs font-semibold">Xóa</button>' +
            '</td></tr>';
        });
        tbody.innerHTML = rows;
      } catch (e) {
        tbody.innerHTML = '<tr><td colspan="8" class="py-4 text-center text-rose-600">Lỗi tải danh sách tài khoản</td></tr>';
      }
    }

    // Admin creates a new account.
    async function createNewAccount() {
      const val = (id) => { const el = document.getElementById(id); return el ? el.value.trim() : ""; };
      const role = val("newAccountRole");
      const username = val("newAccountUsername");
      const password = val("newAccountPassword");
      const fullName = val("newAccountFullName");
      const email = val("newAccountEmail");
      const day = val("newAccountDay");
      const month = val("newAccountMonth");
      const year = val("newAccountYear");
      const phone = val("newAccountPhone");
      const code = val("newAccountCode");

      if (!username || !password || !fullName || !email || !code) {
        accountsResult("Vui lòng điền đầy đủ: Mã đăng nhập, Mật khẩu, Họ và tên, Email, MSSV.", true);
        return;
      }
      if (!day || !month || !year) {
        accountsResult("Vui lòng chọn đầy đủ ngày / tháng / năm sinh.", true);
        return;
      }
      if (!/^[0-9]{9,11}$/.test(phone)) {
        accountsResult("Số điện thoại phải gồm 9-11 chữ số.", true);
        return;
      }

      const body = JSON.stringify({
        username: username,
        password: password,
        full_name: fullName,
        email: email,
        role: role,
        code: code,
        phone: phone,
        date_of_birth: day + "/" + month + "/" + year
      });

      try {
        const res = await fetch("/api/admin/create-account", {
          method: "POST",
          headers: { "Content-Type": "application/json" },
          body: body
        });
        const data = await res.json();
        if (res.ok && data.success) {
          accountsResult("Tạo tài khoản thành công: " + username, false);
          ["newAccountUsername", "newAccountPassword", "newAccountFullName", "newAccountEmail",
           "newAccountDay", "newAccountMonth", "newAccountYear", "newAccountPhone", "newAccountCode"]
            .forEach(id => { const el = document.getElementById(id); if (el) el.value = ""; });
          await loadAccounts();
        } else {
          accountsResult(data.error || "Lỗi tạo tài khoản.", true);
        }
      } catch (e) {
        accountsResult("Không thể kết nối với server.", true);
      }
    }

    // Admin deletes an account after confirmation.
    async function deleteAccount(username) {
      if (!confirm("Xóa tài khoản \"" + username + "\"? Thao tác này không thể hoàn tác.")) return;
      try {
        const res = await fetch("/api/admin/delete-account", {
          method: "POST",
          headers: { "Content-Type": "application/json" },
          body: JSON.stringify({ username: username })
        });
        const data = await res.json();
        if (res.ok && data.success) {
          accountsResult("Đã xóa tài khoản: " + username, false);
          await loadAccounts();
        } else {
          accountsResult(data.error || "Không thể xóa tài khoản.", true);
        }
      } catch (e) {
        accountsResult("Không thể kết nối với server.", true);
      }
    }

    // ---- Admin: thiet bi that dang gan voi tung sinh vien ----
    async function loadDeviceBindings() {
      const el = document.getElementById("deviceBindingsBody");
      if (!el) return;
      try {
        const res = await fetch("/api/admin/device-bindings", { cache: "no-store" });
        const data = await res.json();
        const list = data.bindings || [];
        if (!list.length) {
          el.innerHTML = '<div class="p-3 text-xs text-slate-500">Chưa có sinh viên nào trong hệ thống.</div>';
          return;
        }
        let html = "";
        list.forEach(b => {
          const dev = b.device_id || "Chưa gắn thiết bị";
          const color = b.locked ? "text-indigo-600" : "text-slate-400";
          html += '<div class="p-3 bg-slate-50 border border-slate-200 rounded-xl flex flex-wrap items-center justify-between gap-2">' +
            '<div class="text-xs min-w-0">' +
              '<span class="font-bold text-slate-800">' + b.student_code + '</span> (' + (b.full_name || "--") + ')' +
              '<div class="font-mono ' + color + ' text-[11px] mt-0.5 break-all">' + dev + '</div>' +
              (b.locked ? '<span class="inline-block mt-1 text-[10px] bg-indigo-100 text-indigo-700 px-1.5 py-0.5 rounded font-bold">ĐANG KHÓA</span>' : '') +
            '</div>' +
            '<button onclick="resetDeviceLock(\'' + b.student_code + '\')" class="px-2.5 py-1 bg-white hover:bg-slate-100 text-slate-700 border border-slate-300 rounded text-xs font-semibold flex items-center gap-1 shadow-sm">' +
              '<i class="fa-solid fa-arrows-rotate text-indigo-600"></i> Reset Lock' +
            '</button>' +
          '</div>';
        });
        el.innerHTML = html;
      } catch (e) {
        el.innerHTML = '<div class="p-3 text-xs text-rose-600">Lỗi tải danh sách thiết bị</div>';
      }
    }

    // ---- Admin: don xin mo khoa thiet bi ----
    async function loadDeviceRequests() {
      const el = document.getElementById("deviceRequestQueue");
      if (!el) return;
      try {
        const res = await fetch("/api/admin/device-reset-requests", { cache: "no-store" });
        const data = await res.json();
        const list = data.requests || [];
        const cnt = document.getElementById("deviceRequestCount");
        if (cnt) cnt.innerText = list.length + " chờ duyệt";
        if (!list.length) {
          el.innerHTML = '<div class="p-3 rounded-xl bg-slate-50 border border-slate-200 text-xs text-slate-500">Không có đơn nào chờ duyệt.</div>';
          return;
        }
        let html = "";
        list.forEach(r => {
          html += '<div class="p-3 rounded-xl bg-amber-50 border border-amber-200 text-xs">' +
            '<div class="flex flex-wrap items-center justify-between gap-2">' +
              '<span class="font-bold text-amber-900">#' + r.request_id + ' • ' + r.student_code + ' (' + (r.student_name || "--") + ')</span>' +
              '<span class="text-[10px] text-amber-700">' + (r.created_at || "") + '</span>' +
            '</div>' +
            '<div class="mt-1 text-amber-900"><strong>Lý do:</strong> ' + (r.reason || "--") + '</div>' +
            '<div class="mt-1 text-amber-900"><strong>Giải trình:</strong> ' + (r.explanation || "--") + '</div>' +
            '<div class="mt-1 font-mono text-[11px] text-amber-800"><i class="fa-solid fa-image mr-1"></i>Ảnh chứng minh: ' + (r.file_name || "--") + '</div>' +
            '<div class="mt-2 flex flex-wrap gap-2">' +
              '<button onclick="reviewDeviceRequest(' + r.request_id + ', true)" class="px-3 py-1.5 bg-emerald-600 hover:bg-emerald-700 text-white rounded-lg text-xs font-semibold">Duyệt &amp; mở khóa</button>' +
              '<button onclick="reviewDeviceRequest(' + r.request_id + ', false)" class="px-3 py-1.5 bg-white border border-rose-300 text-rose-600 hover:bg-rose-50 rounded-lg text-xs font-semibold">Từ chối</button>' +
            '</div>' +
          '</div>';
        });
        el.innerHTML = html;
      } catch (e) {
        el.innerHTML = '<div class="p-3 text-xs text-rose-600">Lỗi tải đơn yêu cầu</div>';
      }
    }

    async function reviewDeviceRequest(id, approve) {
      if (!confirm(approve ? "Duyệt đơn #" + id + " và mở khóa thiết bị của sinh viên?" : "Từ chối đơn #" + id + "?")) return;
      try {
        const res = await fetch("/api/admin/device-reset-requests/review", {
          method: "POST",
          headers: { "Content-Type": "application/json" },
          body: JSON.stringify({ request_id: id, approve: approve, note: approve ? "Admin approved" : "Admin rejected" })
        });
        const data = await res.json();
        if (!data.success) { alert(data.error || "Không thể xử lý đơn."); return; }
        alert(approve ? "Đã duyệt và mở khóa thiết bị." : "Đã từ chối đơn.");
        await loadDeviceRequests();
        await loadDeviceBindings();
      } catch (e) { alert("Không thể kết nối với server."); }
    }

    // ---- Student: yeu cau reset thiet bi kem anh Chung minh ----
    let deviceResetFile = null;

    function updateDeviceCard() {
      const id = hasDeviceConsent() ? getOrCreateDeviceId() : "";
      const label = document.getElementById("deviceBoundLabel");
      const badge = document.getElementById("deviceBoundBadge");
      if (label) label.innerText = id || "Chưa có mã thiết bị";
      if (badge) {
        badge.innerText = id ? "Đã ghi nhớ" : "Chưa ghi nhớ";
        badge.className = "text-[11px] font-semibold px-2 py-1 rounded-lg " +
          (id ? "bg-indigo-50 text-indigo-700" : "bg-slate-100 text-slate-600");
      }
      loadDeviceRequestStatus();
    }

    async function loadDeviceRequestStatus() {
      const el = document.getElementById("deviceRequestStatus");
      if (!el) return;
      const code = (activeRole === "STUDENT" && activeStudentCode) ? activeStudentCode : "B22DCCN001";
      try {
        const res = await fetch("/api/student/device-reset-requests", {
          method: "POST",
          headers: { "Content-Type": "application/json" },
          body: JSON.stringify({ student_code: code })
        });
        const data = await res.json();
        const list = data.requests || [];
        if (!list.length) { el.innerText = "Chưa có đơn yêu cầu nào."; return; }
        const r = list[0];
        el.innerText = "Đơn gần nhất: " + r.reason + " — " + r.status +
          (r.reviewer_note ? " (" + r.reviewer_note + ")" : "");
      } catch (e) { el.innerText = ""; }
    }

    function openDeviceResetModal() {
      deviceResetFile = null;
      document.getElementById("deviceResetReason").value = "Đổi điện thoại mới";
      document.getElementById("deviceResetExplanation").value = "";
      document.getElementById("deviceResetEmpty").classList.remove("hidden");
      document.getElementById("deviceResetSelected").classList.add("hidden");
      document.getElementById("deviceResetFileError").classList.add("hidden");
      const m = document.getElementById("deviceResetModal");
      m.classList.remove("hidden"); m.classList.add("flex");
    }

    function closeDeviceResetModal() {
      const m = document.getElementById("deviceResetModal");
      m.classList.add("hidden"); m.classList.remove("flex");
    }

    function setDeviceResetFile(file) {
      const err = document.getElementById("deviceResetFileError");
      err.classList.add("hidden");
      if (!file) return;
      const name = file.name || "image";
      const ext = name.split(".").pop().toLowerCase();
      if (["jpg", "jpeg", "png"].indexOf(ext) < 0 && file.type.indexOf("image/") !== 0) {
        err.innerText = "Chỉ chấp nhận ảnh JPG hoặc PNG.";
        err.classList.remove("hidden");
        return;
      }
      if (file.size > 5 * 1024 * 1024) {
        err.innerText = "Ảnh quá lớn (tối đa 5 MB).";
        err.classList.remove("hidden");
        return;
      }
      deviceResetFile = file;
      document.getElementById("deviceResetEmpty").classList.add("hidden");
      document.getElementById("deviceResetSelected").classList.remove("hidden");
      document.getElementById("deviceResetFileName").innerText = name;
      document.getElementById("deviceResetFileMeta").innerText = formatFileSize(file.size);
      const thumb = document.getElementById("deviceResetThumb");
      thumb.src = URL.createObjectURL(file);
      thumb.classList.remove("hidden");
    }

    function handleDeviceResetFile(input) { setDeviceResetFile(input.files && input.files[0]); }
    function deviceResetDropHandler(e) {
      e.preventDefault();
      e.currentTarget.classList.remove("border-indigo-500", "bg-indigo-50");
      setDeviceResetFile(e.dataTransfer.files && e.dataTransfer.files[0]);
    }
    function clearDeviceResetFile(e) {
      if (e) { e.preventDefault(); e.stopPropagation(); }
      deviceResetFile = null;
      document.getElementById("deviceResetEmpty").classList.remove("hidden");
      document.getElementById("deviceResetSelected").classList.add("hidden");
    }

    async function handleDeviceResetSubmit(e) {
      e.preventDefault();
      if (!deviceResetFile) {
        const err = document.getElementById("deviceResetFileError");
        err.innerText = "Vui lòng đính kèm ảnh chứng minh.";
        err.classList.remove("hidden");
        return;
      }
      const code = (activeRole === "STUDENT" && activeStudentCode) ? activeStudentCode : "B22DCCN001";
      const body = JSON.stringify({
        student_code: code,
        reason: document.getElementById("deviceResetReason").value,
        explanation: document.getElementById("deviceResetExplanation").value,
        file_name: deviceResetFile.name,
        file_size: deviceResetFile.size
      });
      try {
        const res = await fetch("/api/student/device-reset-request", {
          method: "POST",
          headers: { "Content-Type": "application/json" },
          body: body
        });
        const data = await res.json();
        if (!data.success) { alert(data.error || "Gửi đơn thất bại."); return; }
        closeDeviceResetModal();
        alert("Đã gửi đơn #" + data.request_id + " tới Admin kèm ảnh chứng minh.\nTrạng thái: PENDING");
        loadDeviceRequestStatus();
      } catch (err2) { alert("Không thể kết nối với server."); }
    }

    function copyPhoneUrl() {
      const link = document.getElementById("phoneAccessLink");
      const btn = document.getElementById("phoneCopyBtn");
      if (!link || !btn || !link.innerText) return;
      const done = () => {
        const old = btn.innerHTML;
        btn.innerHTML = '<i class="fa-solid fa-check"></i> Copied';
        setTimeout(() => { btn.innerHTML = old; }, 1500);
      };
      if (navigator.clipboard && navigator.clipboard.writeText) {
        navigator.clipboard.writeText(link.innerText).then(done).catch(() => {});
      } else {
        done();
      }
    }

    function updateGpsNotice() {
      const el = document.getElementById("gpsInsecureNotice");
      if (!el) return;
      el.classList.toggle("hidden", !!window.isSecureContext);
    }

    if ("serviceWorker" in navigator) {
      window.addEventListener("load", () => {
        navigator.serviceWorker.register("/sw.js").catch(() => {});
      });
    }

    // Boot: load the live check-in zone and reflect role permissions
    loadGeofence();
    refreshZoneButtons();
    loadPhoneAccess();
    loadNetworkStatus();
    updateGpsNotice();
    initDobSelects();
    syncUsernameToCode();
    loadAccounts();
    loadDeviceBindings();
    loadDeviceRequests();
    updateDeviceCard();
    (function initDeviceLabels() {
      const id = hasDeviceConsent() ? getOrCreateDeviceId() : "Chưa ghi nhớ thiết bị";
      const header = document.getElementById("deviceFingerprintLabel");
      const scan = document.getElementById("deviceVerifiedLabel");
      if (header) header.innerText = id;
      if (scan) scan.innerText = id;
    })();
  </script>
</body>
</html>
)rawliteral";
}

} // namespace attendance
