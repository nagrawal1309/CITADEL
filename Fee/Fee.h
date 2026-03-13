#ifndef FEE_H
#define FEE_H

#include "sqlite3.h"
#include <string>

class Fee {
public:
    void assignFee(sqlite3* DB);
    
    void viewReceipt(sqlite3* DB, int studentID);
};

#endif