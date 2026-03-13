#include "attendance.h"
#include <iostream>
#include <iomanip>
#include <string>

using namespace std;


void Attendance::viewMonthlyAttendance(sqlite3* DB, int sID, string monthYear) {
    
    

    string sql = "SELECT UPPER(TRIM(SUBJECT)), "
                 "SUM(CASE WHEN STATUS IN ('P', 'p', 'Present') THEN 1 ELSE 0 END) as PresentCount, "
                 "COUNT(STATUS) as TotalCount "
                 "FROM ATTENDANCE "
                 "WHERE STUDENT_ID = " + to_string(sID) + " AND DATE LIKE '" + monthYear + "%' "
                 "GROUP BY UPPER(TRIM(SUBJECT));";

    sqlite3_stmt* stmt;

    cout << "\n==========================================================" << endl;
    cout << "   CONSOLIDATED REPORT: " << monthYear << endl;
    cout << "   FOR STUDENT ID: " << sID << endl;
    cout << "==========================================================" << endl;
    cout << left << setw(22) << "SUBJECT" 
         << " | " << setw(8) << "TOTAL" 
         << " | " << setw(8) << "PRESENT" 
         << " | " << "PERCENTAGE" << endl;
    cout << "----------------------------------------------------------" << endl;

    if (sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, NULL) == SQLITE_OK) {
        bool hasData = false;
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            hasData = true;
            string subject = (const char*)sqlite3_column_text(stmt, 0);
            int present = sqlite3_column_int(stmt, 1);
            int total = sqlite3_column_int(stmt, 2);
            double percentage = (total > 0) ? (static_cast<double>(present) / total) * 100 : 0.0;

            cout << left << setw(22) << subject << " | " 
                 << setw(8) << total << " | " 
                 << setw(8) << present << " | " 
                 << fixed << setprecision(2) << percentage << "%";

            if (percentage < 75.0) {
                cout << " (SHORTAGE)";
            }
            cout << endl;
        }

        if (!hasData) {
            cout << "No records found for Student " << sID << " in " << monthYear << endl;
        }
    } else {
        cout << "Database Error: " << sqlite3_errmsg(DB) << endl;
    }
    sqlite3_finalize(stmt);
    cout << "----------------------------------------------------------" << endl;
}

void Attendance::viewMyAttendance(sqlite3* DB, int sID) {
    string sql = "SELECT SUBJECT, STATUS, DATE FROM ATTENDANCE WHERE STUDENT_ID = " + to_string(sID) + " ORDER BY DATE DESC;";
    
    cout << "\n--- DAILY ATTENDANCE HISTORY (STUDENT " << sID << ") ---" << endl;
    
    auto callback = [](void* data, int argc, char** argv, char** colName) -> int {
        cout << "Date: " << (argv[2] ? argv[2] : "N/A") 
             << " | Subject: " << (argv[0] ? argv[0] : "N/A") 
             << " | Status: " << (argv[1] ? argv[1] : "N/A") << endl;
        return 0;
    };
    sqlite3_exec(DB, sql.c_str(), callback, 0, NULL);
}

void Attendance::markAttendance(sqlite3* DB) {
    int sID;
    string subject, status;
    cout << "\nMarking Attendance for Today..." << endl;
    cout << "Student ID: "; cin >> sID;
    cout << "Subject: "; cin.ignore(); getline(cin, subject);
    cout << "Status (P/A): "; cin >> status;

    string sql = "INSERT INTO ATTENDANCE (STUDENT_ID, SUBJECT, STATUS) VALUES (?, ?, ?);";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, sID);
        sqlite3_bind_text(stmt, 2, subject.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, status.c_str(), -1, SQLITE_STATIC);
        sqlite3_step(stmt);
    }
    sqlite3_finalize(stmt);
    cout << "Successfully saved today's attendance." << endl;
}