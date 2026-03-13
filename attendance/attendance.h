#ifndef ATTENDANCE_H
#define ATTENDANCE_H
#include "sqlite3.h"
#include <string>

class Attendance {
public:
    void markAttendance(sqlite3* DB);
    void viewMyAttendance(sqlite3* DB, int sID);
    void viewMonthlyAttendance(sqlite3* DB, int sID, std::string monthYear);
};
#endif