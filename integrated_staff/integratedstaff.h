#ifndef INTEGRATEDSTAFF_H
#define INTEGRATEDSTAFF_H

extern "C" {
#include "sqlite3.h"
}

#include <string>

using namespace std;


class IntegratedStaff {
public:
    
    void registerStaff(sqlite3* DB);

    
    void viewStaff(sqlite3* DB); // 

    /**
     * Authenticates a staff member and redirects to their specific dashboard.
     * @return true if credentials match, false otherwise.
     */
    bool login(sqlite3* DB); 
};

#endif