#include "Notice.h"

void Notice::postNotice(sqlite3* DB) {
    string title, content;
    cout << "\n--- Create New Notice ---\n";
    cout << "Enter Notice Title: ";
    cin.ignore(); 
    getline(cin, title); 
    cout << "Enter Content: ";
    getline(cin, content);

    string sql = "INSERT INTO NOTICE (TITLE, CONTENT, DATE) VALUES (?, ?, DATE('now'));";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, title.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, content.c_str(), -1, SQLITE_STATIC);
        
        if (sqlite3_step(stmt) == SQLITE_DONE) {
            cout << "Notice Published Successfully!\n";
        } else {
            cout << "Error publishing notice!\n";
        }
    }
    sqlite3_finalize(stmt);
}


void Notice::viewAllNotices(sqlite3* DB) {
    cout << "\n========== NOTICE BOARD ==========\n";
    string sql = "SELECT ID, TITLE, CONTENT, DATE FROM NOTICE ORDER BY ID DESC;";
    
    auto callback = [](void* data, int argc, char** argv, char** azColName) -> int {
        cout << "Notice ID: " << (argv[0] ? argv[0] : "N/A") << " | Date: " << (argv[3] ? argv[3] : "N/A") << endl;
        cout << "TITLE: " << (argv[1] ? argv[1] : "No Title") << endl;
        cout << "MESSAGE: " << (argv[2] ? argv[2] : "No Content") << endl;
        cout << "----------------------------------\n";
        return 0;
    };

    char* errMsg = 0;
    if (sqlite3_exec(DB, sql.c_str(), callback, 0, &errMsg) != SQLITE_OK) {
        cout << "Error reading notices: " << errMsg << endl;
        sqlite3_free(errMsg);
    }
}


void Notice::deleteNotice(sqlite3* DB) {
    int id;
    viewAllNotices(DB); 
    cout << "Enter Notice ID to Delete: ";
    cin >> id;

    string sql = "DELETE FROM NOTICE WHERE ID = " + to_string(id) + ";";
    if (sqlite3_exec(DB, sql.c_str(), NULL, 0, NULL) == SQLITE_OK) {
        cout << "Notice deleted successfully!\n";
    } else {
        cout << "Error deleting notice: " << sqlite3_errmsg(DB) << endl;
    }
}