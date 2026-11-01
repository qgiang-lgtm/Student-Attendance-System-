# Student Attendance Management System

### GPS-Based Student Attendance Web Application

**Software Engineering Group Project | Group 01**

---

## 1. Project Overview

The **Student Attendance Management System** is a web-based application designed to improve student attendance management in universities using **GPS (Global Positioning System)** and **Geofencing** technology.

Traditional attendance methods, such as manual roll calls, may consume valuable class time and make attendance records difficult to manage.

This project aims to develop a convenient attendance management platform where students can check in through a web browser using their mobile devices, while lecturers can monitor attendance records and administrators can manage the information required for the system.

The system verifies the student's location against the configured classroom area and evaluates the attendance time to determine the appropriate attendance status.

The existing university E-Learning attendance interface is used as a reference for the project's user interface. Direct integration with the university LMS or SSO is not included in the confirmed implementation scope.

## 2. Project Objectives

The main objectives of the project are:

- Develop a web-based student attendance management system.
- Support student self-check-in using GPS location verification.
- Apply Geofencing to validate student locations.
- Reduce manual attendance procedures for lecturers.
- Support time-based attendance classification.
- Allow students to view attendance history and results.
- Provide an explanation and leave request process.
- Help lecturers manage attendance sessions and review student requests.
- Support administrators in managing users, courses, classes, schedules, and classroom locations.
- Provide a centralized platform for attendance information and reporting.

## 3. User Roles and Planned Features

The system includes **three actual user roles**: Student, Lecturer, and Administrator.

The UML Use Case Diagram also includes **System User**, a generalized actor representing common functions such as Sign In and Sign Out. System User is not a separate account role.

### 3.1 Student

Students are the primary users who perform GPS-based attendance check-ins.

**Planned functions:**

- Sign in and sign out.
- View today's class schedule.
- View available attendance sessions.
- Perform GPS-based attendance check-in.
- View attendance history for individual courses.
- View attendance summaries and attendance points.
- Submit attendance explanations or leave requests.
- Attach supporting evidence when necessary.
- Track the status of submitted requests.

### 3.2 Lecturer

Lecturers are responsible for managing attendance activities in their assigned classes.

**Planned functions:**

- Sign in and sign out.
- View assigned courses and classes.
- Create, configure, open, and close attendance sessions.
- Configure or confirm classroom attendance locations.
- Monitor student attendance results.
- View students who are present, late, or absent.
- Review attendance explanations and leave requests.
- Approve or reject student requests.
- Make authorized attendance adjustments when necessary.
- View attendance statistics and reports.
- Manage descriptions of teaching sessions.

### 3.3 Administrator

Administrators are responsible for managing academic information and system configuration.

**Planned functions:**

- Sign in and sign out.
- Manage user accounts and access permissions.
- Manage student and lecturer information.
- Manage courses, classes, and enrollments.
- Manage classroom information and GPS coordinates.
- Manage academic schedules and attendance-related configurations.
- Maintain information required for the attendance management process.

The login interface provides **Student** and **Lecturer** options. Administrator accounts can authenticate through either option, with access permissions determined by the account's actual role.

## 4. Core System Features

### 4.1 GPS-Based Attendance Check-in

Students can check in through the website using a GPS-enabled device.

The system performs the following operations:

1. Authenticates the student.
2. Displays the student's scheduled classes.
3. Allows the student to select an available attendance session.
4. Requests permission to access the device's location.
5. Obtains the current GPS coordinates.
6. Validates the student's location using Geofencing.
7. Checks the attendance session and check-in time.
8. Records a valid attendance result.
9. Displays the result to the student.

### 4.2 Geofencing Validation

Geofencing creates a virtual geographic boundary around a configured classroom location.

The system compares the student's GPS coordinates with the classroom coordinates.

The **Haversine Formula** can be used to calculate the geographic distance between these locations.

A check-in is accepted only when the required location and attendance conditions are satisfied.

### 4.3 Attendance Classification

The system supports four main attendance statuses:

| Status | Description |
|---|---|
| **Present** | The student completes a valid check-in during the permitted on-time period. |
| **Late** | The student completes a valid check-in after the on-time deadline while late check-in is still permitted. |
| **Absent** | The student does not have a valid attendance result or an approved absence. |
| **Excused Absence** | The student's absence has been approved by the responsible lecturer. |

**Business rule confirmation:** The team is reviewing the final check-in time windows and Late scoring policy. The proposed attendance policy includes Present = 10/10, Excused Absence = 2/10, and Absent = 0/10. Final rules must be synchronized across all requirements, diagrams, and implementation documents.

### 4.4 Attendance Explanation and Leave Requests

Students can submit explanations or leave requests when they cannot complete a regular attendance check-in or have a valid reason for absence.

The workflow includes:

1. Student selects the relevant class session.
2. Student enters a reason and attaches evidence if necessary.
3. The request is submitted to the responsible lecturer.
4. Lecturer reviews the request.
5. Lecturer approves or rejects the request.
6. The system updates the request status and applicable attendance result.

Request statuses include:

- Pending
- Approved
- Rejected

### 4.5 Attendance History and Reports

Students can view attendance information by course and class session.

Attendance information may include:

- Course name.
- Session date and time.
- Attendance status.
- Attendance points.
- Attendance remarks.
- Attendance percentage.

Lecturers can view attendance results for their assigned classes and generate attendance reports.

### 4.6 Academic and Classroom Management

Administrators maintain the information necessary for attendance operations, including:

- User accounts.
- Courses and class sections.
- Student enrollments.
- Class schedules.
- Classroom locations.
- GPS configuration information.

Additional features described in the Functional Requirements draft, such as device binding, live attendance visualization, and timetable synchronization, remain subject to the team's implementation scope and technical feasibility review.

## 5. Proposed Technologies

The following technologies are identified in the group's current project documentation.

| Component | Proposed Technology |
|---|---|
| Application Type | Web Application |
| Frontend | HTML5, CSS3, JavaScript |
| Backend | Python or Java |
| Database | MySQL |
| Location Verification | Browser Geolocation API |
| Geographic Validation | GPS, Geofencing, Haversine Formula |
| Architecture | 3-Tier Layered Architecture |
| Diagram Design | UML, DFD, draw.io |
| Version Control | Git and GitHub |

The final implementation technologies will be confirmed during system development.

## 6. Software Architecture

The project proposes a **3-Tier Layered Architecture** consisting of:

**Presentation Layer**

Provides the web interfaces for Students, Lecturers, and Administrators.

**Business Logic Layer**

Handles authentication, user permissions, attendance validation, GPS verification, attendance classification, and leave request processing.

**Data Access Layer**

Manages the storage and retrieval of users, courses, class sessions, attendance records, and related information using a database management system.

This architecture separates user interaction, application logic, and data management.

## 7. Project Documentation

According to the Software Engineering project requirements, the final Specification and Design report must contain seven main sections:

| No. | Required Section | Description |
|---|---|---|
| 1 | Functional Requirements | Functional behavior and user requirements |
| 2 | Non-Functional Requirements | Quality attributes and system constraints |
| 3 | Data Flow Diagram | DFD Level 0 and Level 1 |
| 4 | Use Case Diagram | Overall diagram, actor list, use case list, and detailed specifications |
| 5 | Class Diagram | UML classes, attributes, operations, and relationships |
| 6 | Data Model | Data entities, relationships, and database design |
| 7 | Interface Design Description | User interface designs and interaction descriptions |

### 7.1 Available Repository Files

The following files have been uploaded to the repository:

- [Use Case Diagram – Editable draw.io File](Student_Attendance_UseCase.drawio)
- [Use Case Diagram – PNG Preview](use%20case.drawio.png)

### 7.2 Other Project Documents

The team is also preparing the following documents:

- Functional Requirements.
- Non-Functional Requirements.
- DFD Level 0 and Level 1.
- Detailed Use Case Specifications.
- UML Class Diagram.
- Data Model and database design.
- Interface Design Description.
- Software Architecture documentation.

These documents will be linked here after they are committed to the repository.

**Note:** A document not yet uploaded to GitHub should not be considered incomplete solely because its repository link is unavailable.

## 8. Team Responsibilities

The project is developed collaboratively by **Group 01**, consisting of six members.

| Member | Main Responsibility |
|---|---|
| Member 1 – Team Leader | Functional Requirements and report integration |
| Member 2 | Non-Functional Requirements and Software Architecture |
| Member 3 | DFD Level 0 and Level 1 |
| Member 4 | Use Case Diagram and Detailed Use Case Specifications |
| Member 5 | Class Diagram and Data Model |
| Member 6 | Interface Design and Use Case documentation support |

All members contribute to reviewing the documentation and maintaining consistency across project deliverables.

## 9. Project Development Stages

### Stage 1 – Requirements Analysis and Specification

- Define functional requirements.
- Define non-functional requirements.
- Analyze system processes.
- Develop DFD diagrams.
- Develop UML Use Case and Class Diagrams.
- Prepare use case specifications.

### Stage 2 – System Design

- Design user interfaces.
- Define software architecture.
- Design the database and data model.
- Prepare system design documentation.

### Stage 3 – Implementation

- Develop frontend interfaces.
- Implement backend functions.
- Connect the application to the database.
- Implement GPS-based attendance validation.
- Implement role-based access control.
- Implement attendance records and request processing.

### Stage 4 – Testing

- Prepare test cases.
- Test user authentication and authorization.
- Test GPS location validation.
- Test attendance time rules.
- Test attendance history and reports.
- Test explanation and leave request processing.
- Record test results and correct identified defects.

## 10. Current Project Status

**Status: Requirements Analysis and System Design**

The team is preparing the documentation and diagrams required for the Software Engineering project.

The available Use Case Diagram has been uploaded to the repository. Other members' documents will be added progressively.

System implementation and software testing are subsequent development stages.

The project documentation and source code will be updated as the team completes and reviews each deliverable.

## 11. Collaboration and Version Control

The team uses **GitHub
