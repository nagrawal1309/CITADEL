#ifndef STUDENTDASHBOARD_H
#define STUDENTDASHBOARD_H

#include <iostream>
#include <string>
#include "sqlite3.h"
#include "../attendance/attendance.h" 

using namespace std;

class StudentDashboard {
private:
    int studentID; 

public:
    StudentDashboard(int id); 

    void showMenu(sqlite3* DB);

    void viewAttendance(sqlite3* DB);

    void viewFeeReceipt(sqlite3* DB); 

    void registerEvent(sqlite3* DB);
};

#endif