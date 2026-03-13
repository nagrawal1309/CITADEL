#include "Event.h"
#include <iostream>
#include <string>

using namespace std;


void Event::addEvent(sqlite3* DB) {
    string name, loc, date;
    int seats;
    cout << "\n--- Create New Event ---\n";
    cout << "Enter Event Name: "; cin.ignore(); getline(cin, name);
    cout << "Enter Location: "; getline(cin, loc);
    cout << "Enter Date (YYYY-MM-DD): "; cin >> date;
    cout << "Enter Total Available Seats: "; cin >> seats;

    string sql = "INSERT INTO EVENT (EVENT_NAME, DATE, LOCATION, SEATS) VALUES (?, ?, ?, ?);";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, date.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, loc.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 4, seats);

        if (sqlite3_step(stmt) == SQLITE_DONE) {
            cout << "Event Created Successfully with " << seats << " seats!\n";
        }
    }
    
    sqlite3_finalize(stmt);
}

bool Event::hasSeats(sqlite3* DB, int eventID) {
    string sql = "SELECT SEATS FROM EVENT WHERE ID = " + to_string(eventID) + ";";
    sqlite3_stmt* stmt;
    int seatsLeft = 0;
    bool found = false;

    if (sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, NULL) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            seatsLeft = sqlite3_column_int(stmt, 0);
            found = true;
        }
    }
    sqlite3_finalize(stmt);
    return (found && seatsLeft > 0);
}

void Event::decrementSeat(sqlite3* DB, int eventID) {
    string updateSql = "UPDATE EVENT SET SEATS = SEATS - 1 WHERE ID = " + to_string(eventID) + ";";
    sqlite3_exec(DB, updateSql.c_str(), NULL, 0, NULL);
}

void Event::viewAllEvents(sqlite3* DB) {
    cout << "\n--- Available Events ---\n";
    string sql = "SELECT ID, EVENT_NAME, DATE, LOCATION, SEATS FROM EVENT;";
    
    auto callback = [](void* data, int argc, char** argv, char** colName) -> int {
        cout << "ID        : " << (argv[0] ? argv[0] : "N/A") << endl;
        cout << "Event Name: " << (argv[1] ? argv[1] : "N/A") << endl;
        cout << "Date      : " << (argv[2] ? argv[2] : "N/A") << endl;
        cout << "Location  : " << (argv[3] ? argv[3] : "N/A") << endl;
        cout << "Seats Left: " << (argv[4] ? argv[4] : "0") << endl;
        cout << "----------------------------------\n";
        return 0;
    };

    char* errMsg = 0;
    if (sqlite3_exec(DB, sql.c_str(), callback, 0, &errMsg) != SQLITE_OK) {
        cout << "Error: " << errMsg << endl;
        sqlite3_free(errMsg);
    }
}