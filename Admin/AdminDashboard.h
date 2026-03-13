#ifndef ADMINDASHBOARD_H
#define ADMINDASHBOARD_H

#include "../sqlite3.h"

class AdminDashboard {
public:
    void showMenu(sqlite3* DB);
    void viewStudents(sqlite3* DB);
    void deleteStudent(sqlite3* DB);
    void viewFaculty(sqlite3* DB);
    void deleteFaculty(sqlite3* DB);
    void assignFee(sqlite3* DB);
    void manageAttendance(sqlite3* DB);
    
    
    void viewParentKey(sqlite3* DB); 
};

#endif