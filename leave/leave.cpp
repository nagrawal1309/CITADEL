#include "leave.h"
#include <iostream>
#include <string>
#include <iomanip>
using namespace std;

void Leave::createLeaveTable(sqlite3* DB) {
    string sql = "CREATE TABLE IF NOT EXISTS LEAVE_APPS ("
                 "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
                 "STUDENT_ID INTEGER NOT NULL, "
                 "REASON TEXT NOT NULL, "
                 "START_DATE TEXT NOT NULL, "
                 "PARENT_CONSENT INTEGER DEFAULT 0, "
                 "WARDEN_APPROVED INTEGER DEFAULT 0, "
                 "VANI_MANDIR_AUDIT INTEGER DEFAULT 0, "
                 "HOD_APPROVED INTEGER DEFAULT 0, "
                 "STATUS TEXT DEFAULT 'Awaiting Parent Consent');";
    sqlite3_exec(DB, sql.c_str(), NULL, 0, NULL);
}

void Leave::parentPortal(sqlite3* DB) {
    int sID;
    string pKey, reason, startDate;
    cout << "\n--- STEP 1: PARENT LEAVE INITIATION ---" << endl;
    cout << "Enter Student ID: "; cin >> sID;
    cout << "Enter Parent Security Key: "; cin >> pKey;

    string sql = "SELECT NAME FROM STUDENT WHERE ID = ? AND PARENT_KEY = ?;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, sID);
        sqlite3_bind_text(stmt, 2, pKey.c_str(), -1, SQLITE_STATIC);

        if (sqlite3_step(stmt) == SQLITE_ROW) {
            cout << "[✔] Student Verified: " << (const char*)sqlite3_column_text(stmt, 0) << endl;
            cout << "Reason: "; cin.ignore(); getline(cin, reason);
            cout << "Date (YYYY-MM-DD): "; cin >> startDate;

            string ins = "INSERT INTO LEAVE_APPS (STUDENT_ID, REASON, START_DATE, PARENT_CONSENT, STATUS) VALUES (?, ?, ?, 1, 'Awaiting Warden');";
            sqlite3_stmt* insStmt;
            if (sqlite3_prepare_v2(DB, ins.c_str(), -1, &insStmt, NULL) == SQLITE_OK) {
                sqlite3_bind_int(insStmt, 1, sID);
                sqlite3_bind_text(insStmt, 2, reason.c_str(), -1, SQLITE_STATIC);
                sqlite3_bind_text(insStmt, 3, startDate.c_str(), -1, SQLITE_STATIC);
                if (sqlite3_step(insStmt) == SQLITE_DONE) 
                    cout << "[✔] Initiated. App ID: " << sqlite3_last_insert_rowid(DB) << endl;
                sqlite3_finalize(insStmt);
            }
        } else cout << "[!] Invalid credentials." << endl;
        sqlite3_finalize(stmt);
    }
}

void Leave::wardenPortal(sqlite3* DB) {
    int appID;
    cout << "\n--- STEP 2: WARDEN APPROVAL ---" << endl;
    cout << "Enter App ID: "; cin >> appID;
    string sql = "UPDATE LEAVE_APPS SET WARDEN_APPROVED = 1, STATUS = 'Awaiting Audit' WHERE ID = ? AND PARENT_CONSENT = 1;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, appID);
        if (sqlite3_step(stmt) == SQLITE_DONE && sqlite3_changes(DB) > 0) cout << "[✔] Warden Approved." << endl;
        else cout << "[!] Failed. Ensure Parent initiated Step 1." << endl;
        sqlite3_finalize(stmt);
    }
}

void Leave::vaniMandirPortal(sqlite3* DB) {
    int appID;
    cout << "\n--- STEP 3: VANI MANDIR AUDIT ---" << endl;
    cout << "Enter App ID: "; cin >> appID;
    string sql = "UPDATE LEAVE_APPS SET VANI_MANDIR_AUDIT = 1, STATUS = 'Awaiting HOD' WHERE ID = ? AND WARDEN_APPROVED = 1;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, appID);
        if (sqlite3_step(stmt) == SQLITE_DONE && sqlite3_changes(DB) > 0) cout << "[✔] Audit Success." << endl;
        else cout << "[!] Failed. Warden clearance required." << endl;
        sqlite3_finalize(stmt);
    }
}

void Leave::hodPortal(sqlite3* DB) {
    int appID;
    cout << "\n--- STEP 4: HOD FINAL SIGN-OFF ---" << endl;
    cout << "Enter App ID: "; cin >> appID;
    string sql = "UPDATE LEAVE_APPS SET HOD_APPROVED = 1, STATUS = 'APPROVED_EXIT_READY' WHERE ID = ? AND VANI_MANDIR_AUDIT = 1;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, appID);
        if (sqlite3_step(stmt) == SQLITE_DONE && sqlite3_changes(DB) > 0) cout << "[✔] HOD Signed. Gate-Pass LIVE." << endl;
        else cout << "[!] Denied. Audit incomplete." << endl;
        sqlite3_finalize(stmt);
    }
}

void Leave::printGatePass(sqlite3* DB, int studentID) {
string sql = "SELECT STATUS, REASON, START_DATE FROM LEAVE_APPS WHERE STUDENT_ID = ? AND HOD_APPROVED = 1 ORDER BY ID DESC LIMIT 1;";
sqlite3_stmt* stmt;
if (sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, NULL) == SQLITE_OK) {
sqlite3_bind_int(stmt, 1, studentID);
 if (sqlite3_step(stmt) == SQLITE_ROW) {
     cout << "\n*** BANASTHALI GATE-PASS ***" << endl;
      cout << "ID: " << studentID << " | STATUS: AUTHORIZED" << endl;
 cout << "REASON: " << (const char*)sqlite3_column_text(stmt, 1) << endl;
  cout << "DATE: " << (const char*)sqlite3_column_text(stmt, 2) << endl;
 } else {
 cout << "[!] No approved pass." << endl;
 checkGlobalStatus(DB, studentID);
 }
 sqlite3_finalize(stmt);
}
}

void Leave::studentQuickView(sqlite3* DB) {
    int id;
    cout << "\nQuick Check - Enter Student ID: "; cin >> id;
    printGatePass(DB, id);
}

void Leave::checkGlobalStatus(sqlite3* DB, int studentID) {
    string sql = "SELECT STATUS, PARENT_CONSENT, WARDEN_APPROVED, VANI_MANDIR_AUDIT, HOD_APPROVED FROM LEAVE_APPS WHERE STUDENT_ID = ? ORDER BY ID DESC LIMIT 1;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, studentID);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            cout << "1. Parent: " << (sqlite3_column_int(stmt, 1) ? "DONE" : "PENDING") << endl;
            cout << "2. Warden: " << (sqlite3_column_int(stmt, 2) ? "DONE" : "PENDING") << endl;
            cout << "3. Audit:  " << (sqlite3_column_int(stmt, 3) ? "DONE" : "PENDING") << endl;
            cout << "4. HOD:    " << (sqlite3_column_int(stmt, 4) ? "DONE" : "PENDING") << endl;
        }
        sqlite3_finalize(stmt);
    }
}