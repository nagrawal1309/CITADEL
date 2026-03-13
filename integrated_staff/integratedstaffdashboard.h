#ifndef INTEGRATEDSTAFFDASHBOARD_H
#define INTEGRATEDSTAFFDASHBOARD_H

extern "C" {
    #include "../sqlite3.h" 
}

#include <string>

using namespace std;

class IntegratedStaffDashboard {
public:
    // Constructor
    IntegratedStaffDashboard(); 
    
    /**
     * Main portal entry point.
     * UPDATED: Ensure these parameters match exactly what is called in 
     * IntegratedStaff::login to fix the 'too few arguments' error.
     */
    void showMenu(sqlite3* DB, int staffID, string role); 
    
    void viewAllStudents(sqlite3* DB); 

    
    void wardenCallVerify(sqlite3* DB);
    void vaniMandirAudit(sqlite3* DB);
    void hodSignOff(sqlite3* DB);

private:
    void viewPendingRequests(sqlite3* DB, string role);
};

#endif