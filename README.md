# Student Attendance Management System
### GPS-Based Student Attendance Web Application

**Group 01 · Software Engineering Course Project**

> **Project status:** Requirements analysis and system design. This repository documents the proposed application; the features below are **specified or designed**, not necessarily implemented or tested.

## 1. Project Overview

The **Student Attendance Management System** is a proposed web application for university attendance management using browser-provided location data and **GPS geofencing**. It aims to replace time-consuming paper-based or verbal attendance checks with a more consistent workflow for students, lecturers, and administrators.

When an attendance session is active, an eligible student requests their device's current location through the web browser. The server is designed to compare that location with the configured classroom geofence and the session time window, then record the attendance result. Lecturers can manage sessions, review absence requests, correct records, and inspect reports. Administrators maintain user accounts and classroom information.

**Important limitation:** Browser location data can help assess whether a reported position falls inside a geofence. By itself, it cannot prove a student's identity or physical presence inside a specific room, and it cannot reliably detect every form of location spoofing.

## 2. Project Objectives

- Simplify attendance check-in and reduce manual recordkeeping.
- Apply consistent time-based and location-based attendance rules.
- Let students review attendance history, points, and course summaries.
- Support documented leave requests and lecturer review.
- Provide lecturers with attendance oversight and reporting tools.
- Centralize academic attendance data and administrative management.

## 3. System Actors and Planned Features

| Actor | Planned capabilities |
|---|---|
| **Student** | Sign in; view today's schedule; check in using location; submit a leave request with evidence; review course attendance history and semester summaries. |
| **Lecturer** | View assigned classes; open/close sessions; configure session geofence; monitor attendance; review leave requests; adjust records with justification; export reports. |
| **Administrator** | Manage user accounts and classroom GPS locations; manage device-binding requests if that feature is adopted; inspect security/audit records. |

Authentication is designed around a login page with **Student** and **Lecturer** tabs. The stored account role, rather than the selected tab alone, determines access; the specification allows an administrator to sign in through either tab.

### Attendance classification (specified rules)

Let **S** be the scheduled class start time, **t** the server-recorded check-in time, **d** the computed distance to the classroom anchor, and **R** the allowed geofence radius.

| Condition | Result | Points |
|---|---|---:|
| Valid location (`d ≤ R`) and `S − 15 min ≤ t ≤ S + 15 min` | **Present** | **10/10** |
| Valid location (`d ≤ R`) during the specified late window, after the on-time period and through `S + 45 min` | **Late** | **5/10** |
| No accepted check-in by the effective session deadline | **Absent** | **0/10** |
| Leave request approved by the assigned lecturer | **Excused** | **2/10** |
| Check-in outside the geofence (`d > R`) | **Rejected check-in** | No successful attendance result |

**Rules requiring team confirmation:** The v3 document describes the late window as beginning at minute 16, which leaves a boundary ambiguity between 15 and 16 minutes. The final cutoff must also account for sessions ending before the nominal late deadline. The default geofence radius and exact treatment of failed check-ins should be confirmed before implementation.

### Proposed GPS check-in flow

1. A lecturer activates an attendance session for an assigned class.
2. An enrolled student opens the session and requests a GPS check-in.
3. The browser requests location permission and returns coordinates and an accuracy estimate, when available.
4. The server checks authorization, enrollment, session state, time, and the submitted location; it calculates distance using the **Haversine formula**.
5. If the request meets the rules, the system records a single attendance outcome for that student and session and displays the result.
6. If validation fails, the system displays the reason; an exception can be handled through the documented lecturer review/correction process.

Device fingerprinting, automatic Fake GPS detection, and a live location radar are **ideas described in the v3 specification**. Their feasibility, privacy implications, and final inclusion still require review. They must not be treated as demonstrated security guarantees.

## 4. Requirements and Design Documentation

The course deliverable focuses on **Stage 1 – Analysis and Requirements Specification** and **Stage 2 – Design**. The required document is organized into the following seven sections:

1. Functional Requirements
2. Non-Functional Requirements
3. Data Flow Diagram (DFD)
4. Use Case Diagram and detailed specifications
5. Class Diagram
6. Data Model / ERD
7. Interface Design Description

The reference document is **`RequirementAndDesignDocument_v3.docx`** (Group 01, SRS v2.0). It identifies **FR-01–FR-17**, **NFR-01–NFR-16**, and **UC01–UC21**, with DFD Level 0/1, UML models, a database design, and eight interface wireframes.

The v3 document contains detailed specifications for **UC01, UC04, UC05, and UC10**. A supplementary Markdown file covering the **17 other use cases** has been prepared on the `docs/use-case-specs-v3` branch for team review. Proposed steps in that supplement are **not approved business rules** until reviewed by Group 01.

### Repository documentation

| Location | Purpose |
|---|---|
| [`Stage 1/`](Stage%201/) | Requirements analysis, use-case material, and related diagrams. |
| [`Stage 1/use-case.md`](Stage%201/use-case.md) | Existing use-case description. |
| [`ui/`](ui/) | Interface-design notes and screen images. |
| [`ui/figma_design.md`](ui/figma_design.md) | UI design documentation. |
| `Stage 1/Use Case Specifications/Group01_17_UC_Specifications_FIXED.md` | Supplementary specifications for the 17 remaining use cases (available on the feature branch until merged). |

## 5. System Design Approach

The specification describes a browser-based **client–server application** with role-based access, server-side attendance validation, and a relational data model.

**Main conceptual components:**

- **Web interface:** login, student schedule/check-in, attendance summaries, lecturer views, and administration.
- **Application layer:** authentication/authorization, attendance sessions, geofence checks, leave review, reports, and audit events.
- **Data layer:** users, academic classes, classroom locations, sessions, attendance records, leave requests, and enrollment relationships.

The reference proposes several possible frontend, backend, and database technologies. **No single implementation stack is confirmed by this README**; the team should record the final choices when source code is added.

## 6. Current Scope and Progress

| Area | Status |
|---|---|
| Requirements catalogue and business rules | Documented in v3; selected rules pending team confirmation |
| DFD Level 0 and Level 1 | Designed |
| Use Case Diagram and UC01–UC21 inventory | Designed |
| Detailed Use Case Specifications | Four in v3; 17 supplementary drafts submitted for review |
| Class Diagram and ERD | Designed; cross-diagram consistency still requires review |
| UI wireframes | Eight screens described in v3 |
| Application implementation | Not established by the design documents alone |
| Automated tests and performance/security validation | Not established by the design documents alone |

The performance, availability, and anti-spoofing statements in the SRS are **requirements or proposals**, not measured outcomes. Once implementation begins, the team should add setup instructions, runnable commands, test results, and deployment details based on the actual code.

## 7. Collaboration Workflow

Use topic branches for changes, review the affected requirements and diagrams, and open pull requests before merging into `main`. Keep functional requirements, use-case specifications, database entities, and interface descriptions synchronized. Avoid describing unimplemented features as complete.

```bash
git clone https://github.com/qgiang-lgtm/Student-Attendance-System-.git
cd Student-Attendance-System-
```

The clone command downloads the repository; it does **not** start the application. Run instructions will be added after the implementation and technology stack are confirmed.

## 8. Project Team

**Group:** 01  
**Course:** Software Engineering  
**Project:** Student Attendance Management System – GPS-Based Student Attendance Web Application

Team member names, individual responsibilities, and instructor details should be filled in using the group's approved information.

---

*This README describes the Group 01 project based on its requirements and design materials. It is not a claim that the proposed application has already been built or deployed.*
