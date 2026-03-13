#include "Student.h"
#include <iostream>
#include <string>
#include <cctype> 
#include <ctime>   
#include <cstdlib> 
#include "../sqlite3.h"
#include "StudentDashboard.h"

using namespace std;

string Student::generateParentKey() {
    const string charset = "ABCDEFGHJKLMNPQRSTUVWXYZ23456789"; 
    string key = "";
    for (int i = 0; i < 6; i++) {
        key += charset[rand() % charset.length()];
    }
    return key;
}

bool Student::isStrongPassword(string pass) {
    if (pass.length() < 8) return false;
    bool hasUpper = false, hasLower = false, hasDigit = false, hasSpecial = false;
    string specialChars = "!@#$%^&*()-_=+[]{}|;:,.<>?";

    for (char ch : pass) {
        if (isupper(ch)) hasUpper = true;
        else if (islower(ch)) hasLower = true;
        else if (isdigit(ch)) hasDigit = true;
        else if (specialChars.find(ch) != string::npos) hasSpecial = true;
    }
    return hasUpper && hasLower && hasDigit && hasSpecial;
}

void Student::registerStudent(sqlite3* DB) {
    string name, email, password, course;
    srand(static_cast<unsigned int>(time(0))); 

    cout << "\n--- Student Registration (Admin Controlled) ---\n";
    cout << "Enter Name: "; 
    cin.ignore(); 
    getline(cin, name); 
    
    cout << "Enter Email: "; 
    cin >> email;

    do {
        cout << "Set Password (Min 8 chars, 1 Upper, 1 Lower, 1 Digit, 1 Special): ";
        cin >> password;
        if (!isStrongPassword(password)) {
            cout << "\n[!] Password too weak! A-Z, a-z, 0-9, and @#$% are required.\n";
        }
    } while (!isStrongPassword(password));

    cout << "Enter Course: "; 
    cin >> course;

    string autoParentKey = generateParentKey();

    sqlite3_stmt* stmt;
    string sql = "INSERT INTO STUDENT (NAME, EMAIL, PASSWORD, COURSE, PARENT_KEY) VALUES (?, ?, ?, ?, ?);";
    
    if (sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, email.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, password.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 4, course.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 5, autoParentKey.c_str(), -1, SQLITE_STATIC);

        if (sqlite3_step(stmt) == SQLITE_DONE) {
            long long lastID = sqlite3_last_insert_rowid(DB); 
            cout << "\n[✔] Registration Successful!" << endl;
            cout << "STUDENT ID      : " << lastID << endl;
            cout << "-------------------------------------------" << endl;
            cout << "ALLOTTED PARENT KEY: " << autoParentKey << endl;
            cout << "-------------------------------------------" << endl;
            cout << "[!] ADMIN: Please provide this key to the parent only." << endl;
        } else {
            cout << "\n[!] Error: Email already exists or Database constraint violation." << endl;
        }
    }
    sqlite3_finalize(stmt);
}

void Student::loginStudent(sqlite3* DB) {
    string email, password;
    cout << "\n--- Student Login ---\n";
    cout << "Enter Email: "; cin >> email;
    cout << "Enter Password: "; cin >> password;

    sqlite3_stmt* stmt;
    string sql = "SELECT ID FROM STUDENT WHERE EMAIL = ? AND PASSWORD = ?;";
    
    if (sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, email.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, password.c_str(), -1, SQLITE_STATIC);

        if (sqlite3_step(stmt) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0); 
            cout << "\nLogin Successful! Welcome ID: " << id << endl;
            sqlite3_finalize(stmt); 
            StudentDashboard dashboard(id); 
            dashboard.showMenu(DB);
        } else {
            cout << "\n[!] Invalid Credentials!" << endl;
            sqlite3_finalize(stmt);
        }
    }
}