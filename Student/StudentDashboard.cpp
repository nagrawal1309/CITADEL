#include "StudentDashboard.h"
#include "../sqlite3.h"           
#include "Event/Event.h"   
#include "Event/Notice.h"  
#include "../attendance/attendance.h" 
#include <iostream>
#include <string>

using namespace std;

StudentDashboard::StudentDashboard(int id) : studentID(id) {}

void StudentDashboard::showMenu(sqlite3* DB) {
    int choice;
    Event ev;   
    Notice nt;  

    do {
        cout << "\n===== STUDENT DASHBOARD (ID: " << studentID << ") =====";
        cout << "\n1. View Events";
        cout << "\n2. Register for Event";
        cout << "\n3. View Notice Board";
        cout << "\n4. View Attendance Records"; 
        cout << "\n5. View Fee Receipt"; 
        cout << "\n6. Logout";
        cout << "\nEnter Choice: ";
        
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(100, '\n');
            continue;
        }

        switch (choice) {
            case 1: ev.viewAllEvents(DB); break;
            case 2: registerEvent(DB); break;
            case 3: nt.viewAllNotices(DB); break;
            case 4: viewAttendance(DB); break; 
            case 5: viewFeeReceipt(DB); break;
            case 6: cout << "Logging out...\n"; break;
            default: cout << "Invalid choice!\n";
        }
    } while (choice != 6);
}

void StudentDashboard::viewAttendance(sqlite3* DB) {
    Attendance att; 
    int subChoice;

    cout << "\n--- Attendance Records ---";
    cout << "\n1. View Daily History (Raw Logs)";
    cout << "\n2. View Subject-Wise Monthly Percentage";
    cout << "\nEnter Choice: ";
    cin >> subChoice;

    if (subChoice == 1) {
        att.viewMyAttendance(DB, this->studentID);
    } else if (subChoice == 2) {
        string mYear;
        cout << "Enter Month (YYYY-MM, e.g., 2026-02): ";
        cin >> mYear;
        att.viewMonthlyAttendance(DB, this->studentID, mYear);
    }

    cout << "\nPress Enter to return...";
    cin.ignore();
    cin.get();
}

void StudentDashboard::viewFeeReceipt(sqlite3* DB) {
    string sql = "SELECT S.NAME, F.TOTAL_FEE, F.DUE_FEE, F.DUE_DATE "
                 "FROM STUDENT S JOIN FEE F ON S.ID = F.STUDENT_ID "
                 "WHERE S.ID = " + to_string(studentID) + ";";

    bool dataFound = false;
    auto callback = [](void* data, int argc, char** argv, char** colName) -> int {
        *((bool*)data) = true;
        cout << "\n--- Fee Receipt ---";
        cout << "\nStudent Name : " << (argv[0] ? argv[0] : "N/A");
        cout << "\nTotal Fee    : " << (argv[1] ? argv[1] : "0.0");
        cout << "\nDue Amount   : " << (argv[2] ? argv[2] : "0.0");
        cout << "\nDue Date     : " << (argv[3] ? argv[3] : "N/A") << endl;
        return 0;
    };
    
    sqlite3_exec(DB, sql.c_str(), callback, &dataFound, NULL);
    
    if (!dataFound) {
        cout << "\n[!] Fee details not found for ID " << studentID << ". Please contact Admin.\n";
    }
}

void StudentDashboard::registerEvent(sqlite3* DB) {
    int eventID;
    cout << "Enter Event ID to Register: "; 
    cin >> eventID;
    
    string sql = "INSERT INTO STUDENT_EVENT (STUDENT_ID, EVENT_ID) VALUES (" + 
                 to_string(studentID) + "," + to_string(eventID) + ");";
    
    if (sqlite3_exec(DB, sql.c_str(), NULL, 0, NULL) == SQLITE_OK) {
        cout << "Success: Registered for Event ID " << eventID << "!\n";
    } else {
        cout << "Error: You are already registered for this event or ID is invalid.\n";
    }
}