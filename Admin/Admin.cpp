#include "Admin.h"
#include "../attendance/attendance.h"
#include "../sqlite3.h"
#include <iostream>
#include <string>
#include <cctype>
#include <vector>

using namespace std;

bool Admin::isStrongPassword(const string& pass) {
    if (pass.length() < 8) return false;

    bool hasUpper = false, hasLower = false, hasDigit = false, hasSpecial = false;
    const string specialChars = "!@#$%^&*()-_=+[]{}|;:,.<>?";

    for (char ch : pass) {
        if (isupper(ch)) hasUpper = true;
        else if (islower(ch)) hasLower = true;
        else if (isdigit(ch)) hasDigit = true;
        else if (specialChars.find(ch) != string::npos) hasSpecial = true;
    }

    return hasUpper && hasLower && hasDigit && hasSpecial;
}

void Admin::registerAdmin(sqlite3* DB) {
    string username, password, role;

    cout << "\n======================================";
    cout << "\n     STAFF / ADMIN REGISTRATION";
    cout << "\n======================================\n";
    
    cout << "Full Name: ";
    cin.ignore(); 
    getline(cin, username); 


    while (true) {
        cout << "Set Password (Min 8 chars, 1 Upper, 1 Lower, 1 Digit, 1 Special): ";
        cin >> password;
        if (isStrongPassword(password)) break;
        
        cout << "\n Security standards not met. Try again.\n";
        cout << "[Hint: Include A-Z, a-z, 0-9, and symbols]\n\n";
    }

    cout << "Enter Role (Admin/Faculty/VaniMandir): ";
    cin >> role;

    sqlite3_stmt* stmt;
    const char* sql = "INSERT INTO ADMIN (USERNAME, PASSWORD, ROLE) VALUES (?, ?, ?);";

    if (sqlite3_prepare_v2(DB, sql, -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, password.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, role.c_str(), -1, SQLITE_STATIC);

        if (sqlite3_step(stmt) == SQLITE_DONE) {
            cout << "\n>>> " << role << " Account Created Successfully! <<<\n";
        } else {
            cout << "\n[!] Registration Failed: Username might already exist.\n";
        }
    } else {
        cerr << "\n[!] SQL Error: " << sqlite3_errmsg(DB) << endl;
    }
    sqlite3_finalize(stmt);
}


bool Admin::login(sqlite3* DB, const string& user, const string& pass) {
    sqlite3_stmt* stmt;
    const char* sql = "SELECT ROLE FROM ADMIN WHERE USERNAME = ? AND PASSWORD = ?;";
    bool success = false;

    if (sqlite3_prepare_v2(DB, sql, -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, user.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, pass.c_str(), -1, SQLITE_STATIC);

        if (sqlite3_step(stmt) == SQLITE_ROW) {
            const char* roleText = (const char*)sqlite3_column_text(stmt, 0);
            cout << "\n[+] Login Successful! Welcome, " << (roleText ? roleText : "User") << ".\n";
            success = true;
        } else {
            cout << "\n[!] Error: Invalid Username or Password.\n";
        }
    } else {
        cerr << "\n[!] SQL Error: " << sqlite3_errmsg(DB) << endl;
    }

    sqlite3_finalize(stmt);
    return success;
}

void Admin::displayProfile() {
    cout << "\n--- ADMIN CONTROL PANEL ---\n";
}


int dbCallback(void* data, int argc, char** argv, char** colName) {
    for (int i = 0; i < argc; i++) {
        cout << colName[i] << ": " << (argv[i] ? argv[i] : "NULL") << "\t";
    }
    cout << endl;
    return 0;
}

void Admin::viewAllStudents(sqlite3* DB) {
    cout << "\n--- REGISTERED STUDENTS ---\n";
    string sql = "SELECT ID, NAME, EMAIL, COURSE FROM STUDENT;";
    if (sqlite3_exec(DB, sql.c_str(), dbCallback, NULL, NULL) != SQLITE_OK) {
        cerr << "Query Error: " << sqlite3_errmsg(DB) << endl;
    }
}

void Admin::viewAllFaculty(sqlite3* DB) {
    cout << "\n--- REGISTERED FACULTY ---\n";
    string sql = "SELECT ID, NAME, DEPARTMENT FROM FACULTY;";
    if (sqlite3_exec(DB, sql.c_str(), dbCallback, NULL, NULL) != SQLITE_OK) {
        cerr << "Query Error: " << sqlite3_errmsg(DB) << endl;
    }
}


void Admin::manageAttendance(sqlite3* DB) {
    Attendance att;
    int choice, sID;
    string mYear;

    cout << "\n--- ATTENDANCE MANAGEMENT (ADMIN OVERRIDE) ---";
    cout << "\n1. Mark New Attendance Record";
    cout << "\n2. View Student History (Daily)";
    cout << "\n3. View Student Percentage (Monthly)";
    cout << "\n0. Return to Main Menu";
    cout << "\nEnter Choice: ";
    cin >> choice;

    switch (choice) {
        case 1: 
            att.markAttendance(DB); 
            break;
        case 2:
            cout << "Enter Student ID: "; cin >> sID;
            att.viewMyAttendance(DB, sID); 
            break;
        case 3:
            cout << "Enter Student ID: "; cin >> sID;
            cout << "Enter Month (YYYY-MM): "; cin >> mYear;
            att.viewMonthlyAttendance(DB, sID, mYear); 
            break;
        case 0:
            return;
        default: 
            cout << "\n[!] Invalid Option.\n"; 
            break;
    }

    cout << "\nPress Enter to continue...";
    cin.ignore(1000, '\n'); 
    cin.get();
}
