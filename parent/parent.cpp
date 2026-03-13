#include "parent.h"
#include "../leave/leave.h"
#include "../attendance/attendance.h"
#include "../Fee/fee.h"
#include "../securitybridge/securitybridge.h"
#include "parentdashboard.h"
#include <iostream>
#include <string>
#include <iomanip>

using namespace std;

void Parent::createParentTable(sqlite3* DB) {
    string sql = "CREATE TABLE IF NOT EXISTS PARENT_ACCOUNTS ("
                 "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
                 "STUDENT_ID INTEGER UNIQUE, "
                 "PARENT_PHONE TEXT UNIQUE, "
                 "FOREIGN KEY(STUDENT_ID) REFERENCES STUDENT(ID));";
    sqlite3_exec(DB, sql.c_str(), NULL, 0, NULL);
}

void Parent::registerParent(sqlite3* DB) {
    int sID;
    string pKey, phone;

    cout << "\n--- PARENT PORTAL REGISTRATION ---" << endl;
    cout << "Enter Student ID: "; cin >> sID;
    cout << "Enter Parent Security Key (from Admin): "; cin >> pKey;

    string verifySql = "SELECT NAME FROM STUDENT WHERE ID = ? AND PARENT_KEY = ?;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(DB, verifySql.c_str(), -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, sID);
        sqlite3_bind_text(stmt, 2, pKey.c_str(), -1, SQLITE_STATIC);

        if (sqlite3_step(stmt) == SQLITE_ROW) {
            cout << "[✔] Student Verified: " << (const char*)sqlite3_column_text(stmt, 0) << endl;
            cout << "Enter Mobile Number for OTP: "; cin >> phone;

            string insertSql = "INSERT INTO PARENT_ACCOUNTS (STUDENT_ID, PARENT_PHONE) VALUES (?, ?);";
            sqlite3_stmt* insStmt;
            if (sqlite3_prepare_v2(DB, insertSql.c_str(), -1, &insStmt, NULL) == SQLITE_OK) {
                sqlite3_bind_int(insStmt, 1, sID);
                sqlite3_bind_text(insStmt, 2, phone.c_str(), -1, SQLITE_STATIC);

                if (sqlite3_exec(DB, sqlite3_expanded_sql(insStmt), NULL, 0, NULL) == SQLITE_OK) {
                    cout << "[✔] Registration Complete! You can now login via OTP." << endl;
                } else {
                    cout << "[!] Error: Account already exists for this Student ID." << endl;
                }
            }
            sqlite3_finalize(insStmt);
        } else {
            cout << "[!] Invalid Student ID or Security Key." << endl;
        }
    }
    sqlite3_finalize(stmt);
}

int Parent::loginParent(sqlite3* DB) {
    string phone, inputOTP;
    SecurityBridge bridge; 
    
    cout << "\n--- PARENT PORTAL: LOGIN ---" << endl;
    cout << "Enter Registered Mobile Number: "; cin >> phone;

    string sql = "SELECT STUDENT_ID FROM PARENT_ACCOUNTS WHERE PARENT_PHONE = ?;";
    sqlite3_stmt* stmt;
    int sID = -1;

    if (sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, phone.c_str(), -1, SQLITE_STATIC);

        if (sqlite3_step(stmt) == SQLITE_ROW) {
            sID = sqlite3_column_int(stmt, 0);
            bridge.requestOTP(phone);
            cout << "Enter 4-digit OTP: "; cin >> inputOTP;

            if (bridge.verifyOTP(phone, inputOTP)) {
                cout << "[✔] Login Successful." << endl;
                showDashboard(DB, sID, phone); 
            } else {
                cout << "[!] Incorrect OTP. Access Denied." << endl;
                sID = -1;
            }
        } else {
            cout << "[!] Mobile number not found. Please Register first." << endl;
        }
    }
    sqlite3_finalize(stmt);
    return sID;
}

void Parent::showDashboard(sqlite3* DB, int studentID, string phone) {
    int choice;
    Leave lv;
    Attendance att; 
    Fee fe;        

    do {
        cout << "\n==========================================" << endl;
        cout << "    PARENT DASHBOARD | STUDENT ID: " << studentID << endl;
        cout << "==========================================" << endl;
        
        sqlite3_stmt* stmt;
        string feeSum = "SELECT DUE_FEE FROM FEE WHERE STUDENT_ID = " + to_string(studentID) + ";";
        if (sqlite3_prepare_v2(DB, feeSum.c_str(), -1, &stmt, NULL) == SQLITE_OK) {
            if (sqlite3_step(stmt) == SQLITE_ROW) {
                double due = sqlite3_column_double(stmt, 0);
                cout << " FEE STATUS    : " << (due > 0 ? "[!] PENDING DUES" : "[✔] PAID") << endl;
            }
        }
        sqlite3_finalize(stmt);

        string leaveSql = "SELECT STATUS FROM LEAVE_APPS WHERE STUDENT_ID = " + to_string(studentID) + " ORDER BY ID DESC LIMIT 1;";
        if (sqlite3_prepare_v2(DB, leaveSql.c_str(), -1, &stmt, NULL) == SQLITE_OK) {
            if (sqlite3_step(stmt) == SQLITE_ROW) 
                cout << " LEAVE STATUS  : " << (const char*)sqlite3_column_text(stmt, 0) << endl;
        }
        sqlite3_finalize(stmt);
        
        cout << "------------------------------------------" << endl;
        cout << "1. View Attendance Report" << endl;
        cout << "2. View Fee Receipt" << endl;
        cout << "3. INITIATE LEAVE (Step 1: Give Parent Consent)" << endl;
        cout << "4. View Gate-Pass" << endl;
        cout << "5. Logout" << endl;
        cout << "\nEnter Choice: "; cin >> choice;

        switch (choice) {
            case 1: {
                string month;
                cout << "Enter Month-Year (YYYY-MM): "; cin >> month;
                att.viewMonthlyAttendance(DB, studentID, month);
                break;
            }
            case 2:
                fe.viewReceipt(DB, studentID);
                break;
            case 3: {
                string reason, date;
                cout << "Reason for Leave: "; cin.ignore(); getline(cin, reason);
                cout << "Departure Date (YYYY-MM-DD): "; cin >> date;
                
                string sql = "INSERT INTO LEAVE_APPS (STUDENT_ID, REASON, START_DATE, STATUS, PARENT_CONSENT) "
                             "VALUES (?, ?, ?, 'Awaiting Warden', 1);";
                sqlite3_stmt* insStmt;
                if(sqlite3_prepare_v2(DB, sql.c_str(), -1, &insStmt, NULL) == SQLITE_OK) {
                    sqlite3_bind_int(insStmt, 1, studentID);
                    sqlite3_bind_text(insStmt, 2, reason.c_str(), -1, SQLITE_STATIC);
                    sqlite3_bind_text(insStmt, 3, date.c_str(), -1, SQLITE_STATIC);

                    if(sqlite3_step(insStmt) == SQLITE_DONE) {
                        long long appID = sqlite3_last_insert_rowid(DB);
                        cout << "\n[✔] Consent Granted! APPLICATION ID: " << appID << endl;
                        cout << "[!] Give this ID to the Warden for Step 2 Approval." << endl;
                    }
                }
                sqlite3_finalize(insStmt);
                break;
            }
            case 4:
                lv.printGatePass(DB, studentID); 
                break;
            case 5:
                cout << "Logging out..." << endl;
                break;
            default:
                cout << "[!] Invalid Selection." << endl;
        }
    } while (choice != 5);
}