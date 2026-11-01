## III. NON-FUNCTIONAL REQUIREMENTS (NATURAL LANGUAGE SPECIFICATION)

### 1. Usability
* **Responsive Web Design:** Fully responsive mobile-first architecture optimized for smartphone browsers (Mobile Safari on iOS, Google Chrome on Android), tablets, and desktop workstations. Supports accessible contrast ratios, Light Mode, and Dark Mode consistent with modern institutional portals.
* **1-Tap Check-in Experience:** Streamlined user journey requiring zero unnecessary navigation; opening the homepage immediately surfaces the active countdown banner and a single action button to complete check-in within 3 seconds.
* **Visual Status Feedback:** Clear color-coded alerts (Green = Success 10/10; Yellow = Late 5/10; Red = Geofence rejection or absence 0/10; Purple = Pending leave review).

### 2. Reliability & Accuracy
* **Haversine Spherical Distance Calculation:**
  Distance $d$ between the student's reported coordinates $(lat_1, lon_1)$ and the classroom anchor $(lat_2, lon_2)$ is mathematically computed server-side:
  $$d = 2 \cdot R_{earth} \cdot \arcsin\left(\sqrt{\sin^2\left(\frac{\Delta \varphi}{2}\right) + \cos(\varphi_1)\cdot\cos(\varphi_2)\cdot\sin^2\left(\frac{\Delta \lambda}{2}\right)}\right)$$
  *(Where: $R_{earth} = 6,371,000\text{ m}$; $\Delta \varphi = \varphi_2 - \varphi_1$; $\Delta \lambda = \lambda_2 - \lambda_1$ in radians).*
* **Authoritative Server Time Synchronization:** All check-in windows, late cutoffs, and timestamps are validated exclusively against server-side Network Time Protocol (NTP) clocks, rendering client device clock tampering impossible.
* **System Availability:** Guarantees $\ge 99.8\%$ uptime during peak morning and afternoon class changeover periods.

### 3. Performance
* **High Concurrency Throughput:** The backend architecture must reliably absorb and process simultaneous check-in spikes of $\ge 5,000$ to $10,000$ concurrent student requests during the 15-minute start window without degraded response times or service interruptions.
* **Response Latency:** Server turnaround time from GPS coordinate transmission to visual confirmation must not exceed $1.5$ seconds under standard 4G/5G/Campus Wi-Fi connectivity.

### 4. Security & Anti-Spoofing
* **Anti-Mock Geolocation Detection:** Rigorous validation checking for browser mock location flags, impossible transit velocities between consecutive check-ins (speed anomaly detection), and verification against campus subnet IP pools.
* **Anti-Proxy Device Fingerprinting:** Unique browser hardware signatures bound to active user tokens, blocking multi-account check-ins originating from the same physical handset.
* **Data Transport Encryption:** Mandatory HTTPS transport with TLS 1.3 protocol. User passwords securely hashed using one-way salted algorithms (`bcrypt` or `Argon2`).
* **Role-Based Access Control (RBAC):** Cryptographically signed JSON Web Tokens (JWT) enforcing route protection across Student, Lecturer, and Admin tiers.

### 5. Design & Technology Constraints
* **Architecture:** Decoupled Client-Server RESTful API architecture following clean, multi-tier software engineering standards.
* **Frontend:** Modern Web Stack (HTML5, Tailwind CSS, TypeScript / React / Vue, Leaflet.js for interactive geofence radar visualization).
* **Backend:** Node.js (NestJS / Express) or Python (FastAPI) optimized for asynchronous non-blocking spatial queries.
* **Database:** Relational Database Management System (PostgreSQL with PostGIS spatial extensions, or MySQL 8.0 with Spatial Indexes).

---