#include "Fee.h"
#include <iostream>
#include <string>

using namespace std;

void Fee::assignFee(sqlite3* DB) {
    int id;
    double tuition, hostel, other;
    string date, remarks;

    cout << "\n--- Admin: Assign Fee Breakup ---\n";
    cout << "Enter Student ID: "; cin >> id;
    cout << "Tuition Fee: "; cin >> tuition;
    cout << "Hostel Fee: "; cin >> hostel;
    cout << "Other Charges (Events/Library): "; cin >> other;
    cout << "Due Date (YYYY-MM-DD): "; cin >> date;
    cout << "Remarks (e.g. Sem 1 Fees): "; cin.ignore(); getline(cin, remarks);

    double total = tuition + hostel + other;

    
    string sql = "INSERT OR REPLACE INTO FEE (STUDENT_ID, TUITION, HOSTEL, OTHER, TOTAL_FEE, DUE_FEE, DUE_DATE, REMARKS) "
                 "VALUES (" + to_string(id) + ", " + to_string(tuition) + ", " + to_string(hostel) + ", " 
                 + to_string(other) + ", " + to_string(total) + ", " + to_string(total) + ", '" + date + "', '" + remarks + "');";
    
    if (sqlite3_exec(DB, sql.c_str(), NULL, 0, NULL) == SQLITE_OK) {
        cout << "Fee Assigned Successfully! Total: " << total << endl;
    } else {
        cout << "Error: " << sqlite3_errmsg(DB) << endl;
    }
}

void Fee::viewReceipt(sqlite3* DB, int studentID) {
    string sql = "SELECT S.NAME, S.COURSE, F.TUITION, F.HOSTEL, F.OTHER, F.TOTAL_FEE, F.DUE_FEE, F.DUE_DATE, F.REMARKS "
                 "FROM FEE F LEFT JOIN STUDENT S ON S.ID = F.STUDENT_ID "
                 "WHERE F.STUDENT_ID = " + to_string(studentID) + ";";

    auto callback = [](void* data, int argc, char** argv, char** colName) -> int {
        cout << "\n====================================";
        cout << "\n        OFFICIAL FEE RECEIPT         ";
        cout << "\n====================================";
        cout << "\nStudent Name : " << (argv[0] ? argv[0] : "N/A");
        cout << "\nCourse       : " << (argv[1] ? argv[1] : "N/A");
        cout << "\nDescription  : " << (argv[8] ? argv[8] : "N/A");
        cout << "\n------------------------------------";
        cout << "\n1. Tuition Fee : " << (argv[2] ? argv[2] : "0.0");
        cout << "\n2. Hostel Fee  : " << (argv[3] ? argv[3] : "0.0");
        cout << "\n3. Other Fees  : " << (argv[4] ? argv[4] : "0.0");
        cout << "\n------------------------------------";
        cout << "\nTOTAL AMOUNT   : " << (argv[5] ? argv[5] : "0.0");
        cout << "\nAMOUNT DUE     : " << (argv[6] ? argv[6] : "0.0");
        cout << "\nLAST DATE      : " << (argv[7] ? argv[7] : "N/A");
        cout << "\n------------------------------------";

        if (argv[6] && stod(argv[6]) > 0) {
            cout << "\n[!] NOTIFICATION: Your fee is PENDING.";
            cout << "\nPlease pay before " << (argv[7] ? argv[7] : "due date") << " to avoid fine.";
        } else {
            cout << "\n[✔] Status: Fully Paid.";
        }
        cout << "\n====================================\n";
        return 0;
    };

    if (sqlite3_exec(DB, sql.c_str(), callback, 0, NULL) != SQLITE_OK) {
        cerr << "SQL Error: " << sqlite3_errmsg(DB) << endl;
    }
}