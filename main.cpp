#include <iostream>
#include <string>
#include <vector>
#include "sqlite3.h"
#include "Student/Student.h"
#include "Student/StudentDashboard.h"
#include "Admin/Admin.h"
#include "Admin/AdminDashboard.h" 
#include "integrated_staff/integratedstaff.h"           
#include "integrated_staff/integratedstaffdashboard.h"  
#include "Event/Event.h"
#include "Event/Notice.h"
#include "attendance/attendance.h"
#include "leave/leave.h"   
#include "securitybridge/securitybridge.h"
#include "parent/parent.h"
#include "parent/parentdashboard.h"

using namespace std;

void initializeDatabase(sqlite3* DB) {
    char* errorMessage = nullptr;

    
    string studentTable = "CREATE TABLE IF NOT EXISTS STUDENT ("
                          "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
                          "NAME TEXT, EMAIL TEXT UNIQUE, PASSWORD TEXT, "
                          "PARENT_KEY TEXT, COURSE TEXT);";

    
    string parentAccountTable = "CREATE TABLE IF NOT EXISTS PARENT_ACCOUNTS ("
                                "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
                                "STUDENT_ID INTEGER UNIQUE, "
                                "PARENT_PHONE TEXT UNIQUE, "
                                "FOREIGN KEY(STUDENT_ID) REFERENCES STUDENT(ID));";

    
    string staffTable = "CREATE TABLE IF NOT EXISTS STAFF ("
                        "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
                        "NAME TEXT NOT NULL, DEPARTMENT TEXT, "
                        "PASSWORD TEXT NOT NULL, ROLE TEXT NOT NULL);";

   
    string leaveTable = "CREATE TABLE IF NOT EXISTS LEAVE_APPS ("
                        "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
                        "STUDENT_ID INTEGER, "
                        "REASON TEXT, "
                        "START_DATE TEXT, "
                        "PARENT_CONSENT INTEGER DEFAULT 0, "      
                        "WARDEN_APPROVED INTEGER DEFAULT 0, "     
                        "VANI_MANDIR_AUDIT INTEGER DEFAULT 0, "   
                        "HOD_APPROVED INTEGER DEFAULT 0, "       
                        "STATUS TEXT DEFAULT 'Awaiting Parent Consent', "
                        "FOREIGN KEY(STUDENT_ID) REFERENCES STUDENT(ID));";

   
    string feeTable = "CREATE TABLE IF NOT EXISTS FEE (STUDENT_ID INTEGER PRIMARY KEY, TUITION REAL, HOSTEL REAL, OTHER REAL, TOTAL_FEE REAL, DUE_FEE REAL, DUE_DATE TEXT, REMARKS TEXT);";
    string attendanceTable = "CREATE TABLE IF NOT EXISTS ATTENDANCE (ID INTEGER PRIMARY KEY AUTOINCREMENT, STUDENT_ID INTEGER, SUBJECT TEXT, STATUS TEXT, DATE TEXT DEFAULT (DATE('now')));";
    string adminTable = "CREATE TABLE IF NOT EXISTS ADMIN (ID INTEGER PRIMARY KEY AUTOINCREMENT, USERNAME TEXT UNIQUE, PASSWORD TEXT NOT NULL, ROLE TEXT NOT NULL);";
    string eventTable = "CREATE TABLE IF NOT EXISTS EVENT (ID INTEGER PRIMARY KEY AUTOINCREMENT, EVENT_NAME TEXT, DATE TEXT, LOCATION TEXT, SEATS INTEGER);";
    string studentEventTable = "CREATE TABLE IF NOT EXISTS STUDENT_EVENT (STUDENT_ID INTEGER, EVENT_ID INTEGER, PRIMARY KEY(STUDENT_ID, EVENT_ID));";
    string noticeTable = "CREATE TABLE IF NOT EXISTS NOTICE (ID INTEGER PRIMARY KEY AUTOINCREMENT, TITLE TEXT, CONTENT TEXT, DATE TEXT);";

    vector<string> tables = {
        studentTable, parentAccountTable, staffTable, leaveTable, 
        adminTable, eventTable, studentEventTable, noticeTable, 
        feeTable, attendanceTable
    };

    for (const string& sql : tables) {
        sqlite3_exec(DB, sql.c_str(), NULL, 0, &errorMessage);
    }

    // Seed Admin
    string seedAdmin = "INSERT OR IGNORE INTO ADMIN (USERNAME, PASSWORD, ROLE) VALUES ('admin', 'admin123', 'SUPER_ADMIN');";
    sqlite3_exec(DB, seedAdmin.c_str(), NULL, 0, NULL);
    
    cout << " Citadel Database Synchronized." << endl;
}

int main() {
    sqlite3* DB;
    if (sqlite3_open("iris.db", &DB) != SQLITE_OK) {
        cerr << "Critical Error: Could not open iris.db!" << endl;
        return 1;
    }

    initializeDatabase(DB);

    int choice;
    bool running = true;

    while (running) {
        cout << "\n=====================================" << endl;
        cout << "        CITADEL  SYSTEM          " << endl;
        cout << "=====================================" << endl;
        cout << "1. Student: Registration" << endl;
        cout << "2. Student: Login" << endl;
        cout << "3. Admin: Registration" << endl;
        cout << "4. Admin: Login" << endl; 
        cout << "5. Staff: Registration" << endl;
        cout << "6. Staff: Portal (Warden/HOD/Audit)" << endl; 
        cout << "7. Parent: Portal (OTP Login)" << endl; 
        cout << "8. Student: Gate-Pass Tracker" << endl;
        cout << "9. Exit" << endl;             
        cout << "-------------------------------------" << endl;
        cout << "Enter Choice: ";
        
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(100, '\n');
            continue;
        }

        switch (choice) {
            case 1: { Student s; s.registerStudent(DB); break; }
            case 2: { Student s; s.loginStudent(DB); break; }
            case 3: { Admin a; a.registerAdmin(DB); break; }
            case 4: { 
                Admin a;
                string user, pass;
                cout << "Username: "; cin.ignore(); getline(cin, user);
                cout << "Password: "; cin >> pass;
                if (a.login(DB, user, pass)) { AdminDashboard ad; ad.showMenu(DB); }
                break;
            }           
            case 5: { IntegratedStaff st; st.registerStaff(DB); break; }
            case 6: { IntegratedStaff st; st.login(DB); break; }
            case 7: { 
                Parent p;
                int pChoice;
                cout << "\n1. Registration (New) | 2. Login (Existing): "; cin >> pChoice;
                if(pChoice == 1) p.registerParent(DB);
                else p.loginParent(DB);
                break; 
            }
            case 8: {
                Leave lm;
                lm.studentQuickView(DB); 
                break;
            }
            case 9:
                cout << "Exiting system..." << endl;
                running = false;
                break;
            default:
                cout << "[!] Select 1-9." << endl;
        }
    }
    
    sqlite3_close(DB);
    return 0;
}