#include "integratedstaffdashboard.h"
#include "../attendance/attendance.h" 
#include "../leave/leave.h" 
#include <iostream>
#include <string>
#include <iomanip>

using namespace std;

IntegratedStaffDashboard::IntegratedStaffDashboard() {} 

void IntegratedStaffDashboard::showMenu(sqlite3* DB, int staffID, string role) {
    int choice;
    Leave lm; 
    
    do {
        cout << "\n=====================================";
        cout << "\n    INTEGRATED STAFF PORTAL - " << role;
        cout << "\n=====================================";

       
        if (role == "WARDEN") {
            cout << "\n1. View Hostel Students";
            cout << "\n2. Approve Leave (Step 2: Warden Sign-off)";
            cout << "\n3. View In-Campus Student List";
        } 
        else if (role == "VANI_MANDIR") {
            cout << "\n1. View All Students";
            cout << "\n2. Leave Audit & Issuance (Step 3 & 5)";
        }
        else if (role == "HOD") {
            cout << "\n1. View Department Students";
            cout << "\n2. Mark Student Attendance";
            cout << "\n3. Academic Leave Verification (Step 4)";
        }

        cout << "\n4. Logout";
        cout << "\n-------------------------------------";
        cout << "\nEnter choice: ";
        
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(100, '\n');
            continue;
        }

        switch(choice) {
            case 1: {
                viewAllStudents(DB); 
                break;
            }
            case 2: {
                if (role == "WARDEN") {
                    lm.wardenPortal(DB); 
                } 
                else if (role == "VANI_MANDIR") {
                    lm.vaniMandirPortal(DB); 
                }
                else if (role == "HOD") {
                    Attendance att;
                    att.markAttendance(DB); 
                }
                break;
            }
            case 3: {
                if (role == "WARDEN") {
                    cout << "\n--- Feature: Students Currently In-Campus ---\n";
                } 
                else if (role == "HOD") {
                    lm.hodPortal(DB); 
                }
                break;
            }
            case 4:
                cout << "Logging out of Integrated Staff Portal...\n";
                break;
            default:
                cout << "Invalid choice! Please try again.\n";
        } 
    } while (choice != 4); 
}

void IntegratedStaffDashboard::viewAllStudents(sqlite3* DB) {
    cout << "\n--- Student Records ---\n";
    cout << left << setw(8) << "ID" << " | " << setw(20) << "Name" << " | " << "Course" << endl;
    cout << "--------------------------------------------\n";
    
    const char* sql = "SELECT ID, NAME, COURSE FROM STUDENT;";
    
    auto callback = [](void* data, int argc, char** argv, char** colName) -> int {
        cout << left << setw(8) << (argv[0] ? argv[0] : "N/A") << " | " 
             << setw(20) << (argv[1] ? argv[1] : "N/A") << " | " 
             << (argv[2] ? argv[2] : "N/A") << endl;
        return 0;
    };

    if (sqlite3_exec(DB, sql, callback, 0, NULL) != SQLITE_OK) {
        cout << "Database Error: Unable to fetch records.\n";
    }
}