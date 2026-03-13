#ifndef NOTICE_H
#define NOTICE_H

#include "sqlite3.h"
#include <iostream>
#include <string>

using namespace std;

class Notice {
public:
    
    void postNotice(sqlite3* DB);   
    void deleteNotice(sqlite3* DB); 

    
    void viewAllNotices(sqlite3* DB);
};

#endif