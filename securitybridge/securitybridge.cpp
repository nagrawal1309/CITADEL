#include "SecurityBridge.h"
#include <ctime>
#include <iomanip>

using namespace std;

void SecurityBridge::requestOTP(string phone) {
    srand(time(0));
    string otp = to_string(rand() % 9000 + 1000);
    activeOTPs[phone] = otp;

    cout << "\n[SMS GATEWAY] Message sent to: " << phone << endl;
    cout << "------------------------------------------" << endl;
    cout << " IRIS OTP: " << otp << " (Valid for 5 mins) " << endl;
    cout << "------------------------------------------" << endl;
}

bool SecurityBridge::verifyOTP(string phone, string enteredOTP) {
    return (activeOTPs.count(phone) && activeOTPs[phone] == enteredOTP);
}

bool SecurityBridge::validateParentKey(sqlite3* DB, int studentID, string parentKey) {
    sqlite3_stmt* stmt;
    string sql = "SELECT NAME FROM STUDENT WHERE ID = ? AND PARENT_KEY = ?;";
    bool isValid = false;

    if (sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, studentID);
        sqlite3_bind_text(stmt, 2, parentKey.c_str(), -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) == SQLITE_ROW) isValid = true;
    }
    sqlite3_finalize(stmt);
    return isValid;
}