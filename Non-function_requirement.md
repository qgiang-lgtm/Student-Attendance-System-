Non-Functional Requirements

**Product Requirements**:

\-Performance:

The system must respond promptly, supporting a minimum of 10,000 concurrent users during peak periods such as class attendance check-ins at the start of a session.

The average response time for a user request must not exceed 5 seconds.

\-Reliability:

The system operates continuously with 99.9% availability, 24/7, to support attendance tracking and learning management.

An automated daily database backup mechanism is in place to prevent data loss.

\-Security:

The user's password must be at least 8 characters long and include both lowercase and uppercase letters.

The system must include security features to prevent common security vulnerabilities such as SQL Injection and XSS.

\-Usability:

The interface is designed to be user-friendly for students, instructors, and administrators, minimizing the risk of operational errors.

design an interface that is compatible with web browsers on both desktop and mobile.

**Organizational Requirements**:

\-Application Platform**:** Web-based application accessible through modern web browsers.

\- Frontend Technologies: HTML5, CSS3, JavaScript.

\- Backend Programming Language: Java or Python.

\- Database Management System: MySQL.

**External Requirements**:

\- Legislative Requirements:

The system must comply with university regulations regarding student data privacy and academic records protection.

Software Architecture

To ensure high maintainability, scalability, and separation of concerns, the Student Attendance Management System adopts a standard 3-Tier Layered Architecture. This architectural pattern separates applications into three logical tiers, allowing developers to modify or add specific layers independently without affecting the entire system.

**Presentation Layer (Client Tier)**:

\- Role**:** Acts as the user interface and the entry point of the application. It handles user interactions and renders data visually.

\- Implementation: Built using a Responsive Web Design approach (HTML5, CSS3, JavaScript) accessible via modern web browsers on both desktops and mobile devices for Students, Lecturers, and Administrators.

**Business Logic Layer** (Application Tier):

\- Role: Acts as the "brain" of the system, handling core business logic, verifying attendance timestamps, authenticating PIN codes, and managing user permissions (RBAC).

\- Implementation: Written in Python or Java, it receives requests from the front-end interface before interacting with the database.

**Data Access Layer** (Database Tier):

\- Role: Manages long-term data storage and retrieval and executes query commands against the database.

\- Implementation: Use MySQL to store entity tables such as Users, Courses, Course Sections, Attendance Sessions, and Attendance Details.