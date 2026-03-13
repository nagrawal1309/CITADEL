#ifndef ADMIN_H
#define ADMIN_H

#include <string>
#include <vector>
#include "sqlite3.h"

class Admin {
private:
    static bool isStrongPassword(const std::string& pass);

public:
    Admin() = default;
    virtual ~Admin() = default;

    void registerAdmin(sqlite3* DB);
    bool login(sqlite3* DB, const std::string& user, const std::string& pass);

    void displayProfile();
    void viewAllStudents(sqlite3* DB);
    void viewAllFaculty(sqlite3* DB);
    void manageAttendance(sqlite3* DB);
};

#endif