#ifndef LEAVE_H
#define LEAVE_H

extern "C" {
    #include "../sqlite3.h" 
}
#include <string>
#include <iostream>

using namespace std;

class Leave {
public:
    void createLeaveTable(sqlite3* DB);
    void parentPortal(sqlite3* DB);
    void wardenPortal(sqlite3* DB);
    void vaniMandirPortal(sqlite3* DB);
    void hodPortal(sqlite3* DB);
    void printGatePass(sqlite3* DB, int studentID);
    void studentQuickView(sqlite3* DB); 
    void checkGlobalStatus(sqlite3* DB, int studentID);
};

#endif