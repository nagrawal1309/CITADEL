# CITADEL
A high-performance, modular CITADEL (College Management  System) developed in C++ and SQLite3. Features multi-role authentication for Admins, Faculty, and Students, integrated security bridges, and a 3-tier automated leave approval workflow.
#Key Features
#Admin Dashboard
. User Directory: Full CRUD (Create, Read, Update, Delete) operations for Student and Staff registration.
. Attendance Management: [NEW] Admin has the authority to mark, view, and modify attendance records to ensure data integrity.
. Fee Management: Assign, update, and track student tuition and hostel fee status.
. Campus Communication: Create/Delete official notices and manage scheduled university events.
. Security Control: Generate and retrieve unique Parent Security Keys for external access.
#Student & Staff Portals
. Attendance Tracking: Staff can record daily attendance, and students can monitor their attendance percentage.
. Smart Leave Workflow: Submit online leave applications with a tiered approval system involving Parents, Wardens, and HODs.
. Digital Profile: Access personal academic records and fee balance status instantly.
#Parent Portal
. Secure Access: Login via a system-generated unique security key linked to the student.
. Real-time Monitoring: Track student attendance and verify pending fee dues.
#Tech Stack
. Language: C++ (Object-Oriented Programming).
. Database: SQLite3 (Relational Database).
. Tools: G++ Compiler, VS Code.
#How to Run
Prerequisites
. G++ Compiler (MinGW-w64 for Windows).
. SQLite3 library files (sqlite3.h, sqlite3.c or sqlite3.o) must be in the root folder.
#Compilation Command
. Bash command
- g++ main.cpp Admin/*.cpp Student/*.cpp integrated_staff/*.cpp Event/*.cpp Fee/*.cpp attendance/*.cpp leave/*.cpp parent/*.cpp sqlite3.c -o banasthali_iris -I. -fpermissive
- ./banasthali_iris
