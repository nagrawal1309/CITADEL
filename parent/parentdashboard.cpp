#include "../parent/parentdashboard.h"
#include "../leave/leave.h"
#include "../attendance/attendance.h"
#include "../Fee/Fee.h"
#include <iostream>
#include <iomanip>
#include <string>

using namespace std;

void ParentDashboard::showMenu(sqlite3* DB, int studentID) {
    int choice;
    Attendance att; 
    Fee fe;         

    do {
        cout << "\n==================== BANASTHALI PARENT PORTAL ====================" << endl;
        
        displayWardSummary(DB, studentID);

        cout << " 1. View Subject-wise Attendance (Consolidated Report)" << endl;
        cout << " 2. View Official Fee Receipt & Breakup" << endl;
        cout << " 3. Request New Leave for Ward (Step 1: Give Consent)" << endl;
        cout << " 4. Track Leave Status & View Digital Gate-Pass" << endl;
        cout << " 5. Logout" << endl;
        cout << "------------------------------------------------------------------" << endl;
        
        cout << "Select Option: ";
        if (!(cin >> choice)) { 
            cin.clear(); 
            cin.ignore(100, '\n'); 
            continue; 
        }

        switch (choice) {
            case 1: {
                string month;
                cout << "\nEnter Month-Year (YYYY-MM): "; 
                cin >> month;
                att.viewMonthlyAttendance(DB, studentID, month); 
                break;
            }
            case 2: 
                fe.viewReceipt(DB, studentID); 
                break;
            case 3: 
                initiateLeaveRequest(DB, studentID); 
                break;
            case 4: 
                viewLeaveStatus(DB, studentID); 
                break;
            case 5: 
                cout << "\n[i] Logging out of Parent Portal..." << endl; 
                break;
            default: 
                cout << "[!] Invalid choice. Please try again." << endl;
        }
    } while (choice != 5);
}

void ParentDashboard::displayWardSummary(sqlite3* DB, int studentID) {
    sqlite3_stmt* stmt;
    
    string attSql = "SELECT S.NAME, "
                    "(CAST(COUNT(CASE WHEN A.STATUS IN ('P','p','Present') THEN 1 END) AS FLOAT) / "
                    "NULLIF(COUNT(A.ID), 0)) * 100 "
                    "FROM STUDENT S LEFT JOIN ATTENDANCE A ON S.ID = A.STUDENT_ID "
                    "WHERE S.ID = ? GROUP BY S.ID;";

    if (sqlite3_prepare_v2(DB, attSql.c_str(), -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, studentID);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            const char* name = (const char*)sqlite3_column_text(stmt, 0);
            cout << " Ward Name  : " << (name ? name : "N/A") << endl;
            
            if (sqlite3_column_type(stmt, 1) != SQLITE_NULL) {
                double totalAtt = sqlite3_column_double(stmt, 1);
                cout << " Attendance : " << fixed << setprecision(2) << totalAtt << "%";
                if (totalAtt < 75.0) cout << " [!] ALERT: SHORTAGE"; 
                cout << endl;
            } else {
                cout << " Attendance : No records marked yet." << endl;
            }
        }
    }
    sqlite3_finalize(stmt);

    string feeSql = "SELECT DUE_FEE, DUE_DATE FROM FEE WHERE STUDENT_ID = ?;";
    if (sqlite3_prepare_v2(DB, feeSql.c_str(), -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, studentID);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            double due = sqlite3_column_double(stmt, 0);
            if (due > 0)
                cout << " Fee Status  : PENDING (Rs. " << due << ") | Deadline: " << sqlite3_column_text(stmt, 1) << endl;
            else
                cout << " Fee Status  : FULLY PAID [✔]" << endl;
        }
    }
    sqlite3_finalize(stmt);
    cout << "------------------------------------------------------------------" << endl;
}

void ParentDashboard::initiateLeaveRequest(sqlite3* DB, int studentID) {
    string reason, startDate;
    cout << "\n--- INITIATE NEW LEAVE REQUEST (PARENT CONSENT) ---" << endl;
    cout << "Reason for Leave: "; 
    cin.ignore(); 
    getline(cin, reason);
    cout << "Departure Date (YYYY-MM-DD): "; 
    cin >> startDate;

    string sql = "INSERT INTO LEAVE_APPS (STUDENT_ID, REASON, START_DATE, STATUS, PARENT_CONSENT) "
                 "VALUES (?, ?, ?, 'Awaiting Warden', 1);";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, studentID);
        sqlite3_bind_text(stmt, 2, reason.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, startDate.c_str(), -1, SQLITE_STATIC);

        if (sqlite3_step(stmt) == SQLITE_DONE) {
            long long appID = sqlite3_last_insert_rowid(DB);
            cout << "\n[✔] CONSENT GRANTED & REQUEST SUBMITTED!" << endl;
            cout << "------------------------------------------------" << endl;
            cout << " LEAVE APPLICATION ID : " << appID << endl;
            cout << "------------------------------------------------" << endl;
            cout << "[!] IMPORTANT: Give Application ID " << appID << " to the Warden." << endl;
        } else {
            cout << "[!] Error: Could not submit request." << endl;
        }
    }
    sqlite3_finalize(stmt);
}

void ParentDashboard::viewLeaveStatus(sqlite3* DB, int studentID) {
    string sql = "SELECT ID, STATUS, WARDEN_APPROVED, HOD_APPROVED FROM LEAVE_APPS WHERE STUDENT_ID = ? ORDER BY ID DESC LIMIT 1;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, studentID);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            int appID = sqlite3_column_int(stmt, 0);
            string status = (const char*)sqlite3_column_text(stmt, 1);
            int warden = sqlite3_column_int(stmt, 2);
            int hod = sqlite3_column_int(stmt, 3);

            cout << "\n--- LATEST LEAVE TRACKER (APP ID: " << appID << ") ---" << endl;
            cout << " Current Status   : " << status << endl;
            cout << " Warden Approval  : " << (warden == 1 ? "[✔] YES" : "[ ] PENDING") << endl;
            cout << " HOD/Final Sign   : " << (hod == 1 ? "[✔] YES" : "[ ] PENDING") << endl;

            if (hod == 1) { 
                cout << "\n***************************************" << endl;
                cout << "   BANASTHALI DIGITAL GATE-PASS ACTIVE " << endl;
                cout << "       AUTHORIZED FOR CAMPUS EXIT      " << endl;
                cout << "***************************************" << endl;
            }
        } else {
            cout << "\n[i] No leave requests found for this student." << endl;
        }
    }
    sqlite3_finalize(stmt);
}