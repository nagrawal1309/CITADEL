#ifndef STUDENT_H
#define STUDENT_H

#include <string>
#include "../sqlite3.h"

class Student {
private:
    bool isStrongPassword(std::string pass); 

    std::string generateParentKey(); 

public:
    void registerStudent(sqlite3* DB);
    
    void loginStudent(sqlite3* DB);
};

#endif