#ifndef SECURITY_BRIDGE_H
#define SECURITY_BRIDGE_H

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include "sqlite3.h"

class SecurityBridge {
private:
    std::map<std::string, std::string> activeOTPs; 

public:
    // OTP System
    void requestOTP(std::string phone);
    bool verifyOTP(std::string phone, std::string enteredOTP);
    
    // Authorization
    bool validateParentKey(sqlite3* DB, int studentID, std::string parentKey);
    
    // Automated Audit
    void logSecurityEvent(sqlite3* DB, int studentID, std::string action);
};

#endif