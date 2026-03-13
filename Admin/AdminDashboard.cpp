#include "AdminDashboard.h"
#include "Admin.h" 
#include "Event/Event.h"  
#include "Event/Notice.h"
#include "integrated_staff/integratedstaff.h" 
#include <iostream>
#include <string>
#include <iomanip>

using namespace std;


void AdminDashboard::showMenu(sqlite3* DB) {
    int choice;
    Event ev;  
    Notice nt;  
    IntegratedStaff is; 

    do {
        cout << "\n======================================";
        cout << "\n          ADMIN PANEL";
        cout << "\n======================================";
        cout << "\n 1. View All Students";
        cout << "\n 2. Delete Student Record";
        cout << "\n 3. View Faculty/Staff Directory";
        cout << "\n 4. Delete Faculty Member";
        cout << "\n 5. Add New Campus Event";
        cout << "\n 6. View Registered Events";
        cout << "\n 7. Post Official Notice";
        cout << "\n 8. Delete Old Notice";
        cout << "\n 9. Manage Student Fees";
        cout << "\n 10. Manage Attendance";
        cout << "\n 11. View Parent Security Key";
        cout << "\n 12. Logout From System";
        cout << "\n======================================";
        cout << "\nEnter Choice (1-12): ";
        
        if (!(cin >> choice)) {
            cout << "[!] Invalid input. Please enter a number.\n";
            cin.clear();
            cin.ignore(1000, '\n');
            continue;
        }

        switch (choice) {
            case 1:  viewStudents(DB); break;
            case 2:  deleteStudent(DB); break;
            case 3:  is.viewStaff(DB); break; 
            case 4:  deleteFaculty(DB); break;
            case 5:  ev.addEvent(DB); break;
            case 6:  ev.viewAllEvents(DB); break;
            case 7:  nt.postNotice(DB); break;
            case 8:  nt.deleteNotice(DB); break;
            case 9:  assignFee(DB); break;
            case 10: manageAttendance(DB); break;
            case 11: viewParentKey(DB); break; 
            case 12: cout << "\n[*] Logging out safely from Admin Panel...\n"; break;
            default: cout << "[!] Invalid option selection!\n";
        }
    } while (choice != 12);
}


void AdminDashboard::viewParentKey(sqlite3* DB) {
    int sID;
    cout << "\n--- PARENT KEY RETRIEVAL ---" << endl;
    cout << "Enter Student ID: "; 
    cin >> sID;

    string sql = "SELECT NAME, PARENT_KEY FROM STUDENT WHERE ID = ?;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, sID);
        
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            string name = (const char*)sqlite3_column_text(stmt, 0);
            const char* key = (const char*)sqlite3_column_text(stmt, 1);
            
            cout << "\n-------------------------------------------" << endl;
            cout << " STUDENT NAME : " << name << endl;
            cout << " PARENT KEY   : " << (key ? key : "NOT GENERATED") << endl;
            cout << "-------------------------------------------" << endl;
            cout << "[!] Provide this key to the parent for portal registration." << endl;
        } else {
            cout << "\n[!] Student ID " << sID << " not found in database." << endl;
        }
    }
    sqlite3_finalize(stmt);
}


void AdminDashboard::manageAttendance(sqlite3* DB) {
    Admin adm;
    adm.manageAttendance(DB);
}


void AdminDashboard::assignFee(sqlite3* DB) {
    int id;
    double tuition, hostel, other;
    string date, remarks;

    cout << "\n--- ASSIGN STUDENT FEE ---" << endl;
    viewStudents(DB);
    
    cout << "\nEnter Student ID: "; cin >> id;
    cout << "Tuition Fee: "; cin >> tuition;
    cout << "Hostel Fee: "; cin >> hostel;
    cout << "Other Charges: "; cin >> other;
    cout << "Due Date (YYYY-MM-DD): "; cin >> date;
    cout << "Remarks: "; cin.ignore(); getline(cin, remarks);

    double total = tuition + hostel + other;

    const char* sql = "INSERT OR REPLACE INTO FEE (STUDENT_ID, TUITION, HOSTEL, OTHER, TOTAL_FEE, DUE_FEE, DUE_DATE, REMARKS) "
                      "VALUES (?, ?, ?, ?, ?, ?, ?, ?);";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(DB, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, id);
        sqlite3_bind_double(stmt, 2, tuition);
        sqlite3_bind_double(stmt, 3, hostel);
        sqlite3_bind_double(stmt, 4, other);
        sqlite3_bind_double(stmt, 5, total);
        sqlite3_bind_double(stmt, 6, total); 
        sqlite3_bind_text(stmt, 7, date.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 8, remarks.c_str(), -1, SQLITE_TRANSIENT);

        if (sqlite3_step(stmt) == SQLITE_DONE) {
            cout << "\n[+] Fee Assigned Successfully! Total: " << fixed << setprecision(2) << total << endl;
        }
    }
    sqlite3_finalize(stmt);
}


void AdminDashboard::viewStudents(sqlite3* DB) {
    cout << "\n--- Current Student List ---" << endl;
    const char* sql = "SELECT ID, NAME, COURSE FROM STUDENT;";
    
    auto callback = [](void*, int argc, char** argv, char** col) -> int {
        cout << "ID: " << left << setw(4) << (argv[0] ? argv[0] : "N/A") 
             << " | Name: " << setw(15) << (argv[1] ? argv[1] : "N/A")
             << " | Course: " << (argv[2] ? argv[2] : "N/A") << endl;
        return 0;
    };

    sqlite3_exec(DB, sql, callback, 0, nullptr);
}


void AdminDashboard::deleteStudent(sqlite3* DB) {
    int id;
    cout << "\n[Caution] Enter Student ID to delete: "; cin >> id;

    const char* queries[] = {
        "DELETE FROM STUDENT_EVENT WHERE STUDENT_ID = ?;",
        "DELETE FROM FEE WHERE STUDENT_ID = ?;",
        "DELETE FROM STUDENT WHERE ID = ?;"
    };

    for (const char* sql : queries) {
        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(DB, sql, -1, &stmt, nullptr) == SQLITE_OK) {
            sqlite3_bind_int(stmt, 1, id);
            sqlite3_step(stmt);
        }
        sqlite3_finalize(stmt);
    }
    cout << "[*] Student and all linked records removed successfully from Citadel.\n";
}


void AdminDashboard::deleteFaculty(sqlite3* DB) {
    int id;
    cout << "\nEnter Staff/Faculty ID to delete: "; cin >> id;

    const char* sql = "DELETE FROM STAFF WHERE ID = ?;"; 
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(DB, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, id);
        if (sqlite3_step(stmt) == SQLITE_DONE) {
            if (sqlite3_changes(DB) > 0)
                cout << "[*] Staff member removed from Citadel records.\n";
            else
                cout << "[!] No staff record found with ID " << id << ".\n";
        }
    }
    sqlite3_finalize(stmt);
}