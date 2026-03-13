#ifndef EVENT_H
#define EVENT_H

#include "sqlite3.h"
#include <iostream>
#include <string>

using namespace std;

class Event {
public:
    void addEvent(sqlite3* DB);       
    void updateEvent(sqlite3* DB);    
    void deleteEvent(sqlite3* DB);    
    void viewAllEvents(sqlite3* DB);  

    
    bool hasSeats(sqlite3* DB, int eventID);   
    void decrementSeat(sqlite3* DB, int eventID); 
};

#endif