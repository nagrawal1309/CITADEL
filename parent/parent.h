#ifndef PARENT_H
#define PARENT_H
#include "sqlite3.h"
#include <string>

class Parent {
public:
    /**
     * @brief One-time registration: Links a Mobile Number to a Student ID
     * using the Admin-provided Secret Key.
     */
    void registerParent(sqlite3* DB);
    
    /**
     * @brief Login using Mobile Number + OTP (via SecurityBridge)
     * @return int The linked Student ID if successful, -1 if failed.
     */
    int loginParent(sqlite3* DB); 

    /**
     * @brief Parent Dashboard: View Attendance, Fees, and Initiate/Track Leave.
     * @param studentID The ID of the ward linked to this parent.
     * @param parentPhone The registered phone number of the parent.
     */
    void showDashboard(sqlite3* DB, int studentID, std::string parentPhone);

    /**
     * @brief Ensures the PARENT_ACCOUNTS table exists with Student ID and Phone.
     */
    void createParentTable(sqlite3* DB);
};

#endif