#ifndef PARENTDASHBOARD_H
#define PARENTDASHBOARD_H

#include "../sqlite3.h"
#include "../attendance/attendance.h"
#include "../Fee/Fee.h"  
#include <string>

class ParentDashboard {
public:
    /**
     * @brief Main interface for parents. Displays Ward Summary (Fees/Attendance) 
     * automatically upon entry and provides the navigation menu.
     */
    void showMenu(sqlite3* DB, int studentID);
    
    /**
     * @brief Automatically fetches and displays high-level Fee Status and 
     * overall Attendance % from the database for the parent.
     */
    void displayWardSummary(sqlite3* DB, int studentID);

    /**
     * @brief Parent initiates the leave request directly. 
     * Sets PARENT_CONSENT = 1 and status to 'Awaiting Warden'.
     */
    void initiateLeaveRequest(sqlite3* DB, int studentID);

    /**
     * @brief Tracks the 4-step sequential approval and displays 
     * the Digital Gate-Pass once HOD approval is granted.
     */
    void viewLeaveStatus(sqlite3* DB, int studentID);

private:
    // Internal instances to access the shared logic from other modules
    Attendance attendanceModule;
    Fee feeModule;
};

#endif