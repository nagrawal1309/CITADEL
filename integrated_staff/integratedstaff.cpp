#include "integratedstaff.h"
#include "integratedstaffdashboard.h"
#include <iostream>  
#include <string>
#include <iomanip> 

using namespace std;


void IntegratedStaff::registerStaff(sqlite3* DB) { 
    string name, dept, pass, role;
    int roleChoice;

    cout << "\n--- Citadel IRIS: Staff Registration ---\n";
    cout << "Enter Name: "; 
    cin.ignore(); 
    getline(cin, name); 
    cout << "Enter Department/Hostel: "; 
    getline(cin, dept);

    cout << "Select Role:\n1. Warden\n2. HOD/Teaching\n3. Vani Mandir (Audit & Gatepass)\nChoice: ";
    cin >> roleChoice;
    
    if (roleChoice == 1) role = "WARDEN";
    else if (roleChoice == 3) role = "VANI_MANDIR";
    else role = "HOD";

    cout << "Set Password: "; 
    cin >> pass;

    string sql = "INSERT INTO STAFF (NAME, DEPARTMENT, PASSWORD, ROLE) VALUES (?, ?, ?, ?);";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, dept.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 3, pass.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 4, role.c_str(), -1, SQLITE_TRANSIENT);

        if (sqlite3_step(stmt) == SQLITE_DONE) {
            cout << "[✔] Staff Registered Successfully as " << role << "!" << endl;
        } else {
            cout << "[!] Registration Failed: " << sqlite3_errmsg(DB) << endl;
        }
        sqlite3_finalize(stmt);
    }
}
void IntegratedStaff::viewStaff(sqlite3* DB) {
    string sql = "SELECT ID, NAME, DEPARTMENT, ROLE FROM STAFF;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, NULL) == SQLITE_OK) {
        cout << "\n------------------ CITADEL FACULTY DIRECTORY ------------------\n";
        cout << left << setw(5) << "ID" << setw(20) << "NAME" << setw(15) << "DEPT" << "ROLE" << endl;
        cout << "---------------------------------------------------------------\n";

        bool found = false;
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            found = true;
            cout << left << setw(5) << sqlite3_column_int(stmt, 0)
                 << setw(20) << (const char*)sqlite3_column_text(stmt, 1)
                 << setw(15) << (const char*)sqlite3_column_text(stmt, 2)
                 << (const char*)sqlite3_column_text(stmt, 3) << endl;
        }

        if (!found) cout << "[!] No staff members registered in the system." << endl;
        cout << "---------------------------------------------------------------\n";
        sqlite3_finalize(stmt);
    }
}


bool IntegratedStaff::login(sqlite3* DB) {
    string name, pass;
    cout << "\n--- Staff Portal Login ---\n";
    
    cin.ignore(); 
    cout << "Enter Name: ";
    getline(cin, name); 
    
    cout << "Enter Password: ";
    cin >> pass; 

    string sql = "SELECT ID, ROLE FROM STAFF WHERE NAME = ? AND PASSWORD = ?;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, pass.c_str(), -1, SQLITE_TRANSIENT);

        if (sqlite3_step(stmt) == SQLITE_ROW) {
            int staffID = sqlite3_column_int(stmt, 0);
            const char* rolePtr = (const char*)sqlite3_column_text(stmt, 1);
            string role = rolePtr ? rolePtr : "STAFF";

            cout << "[✔] Login Successful! Welcome " << role << " " << name << "." << endl;
            
            sqlite3_finalize(stmt); 
            
            IntegratedStaffDashboard sd;
            sd.showMenu(DB, staffID, role); 
            return true; 
        } else {
            cout << "[!] Invalid Staff Credentials!" << endl;
            sqlite3_finalize(stmt);
        }
    }
    return false;
}