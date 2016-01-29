//
// Created by Francisco Duarte on 29/01/16.
//

#include "DBManager.h"

DBManager::DBManager(std::string fileName) {
    int rCode;
    _fileName = fileName;

    rCode = sqlite3_open(_fileName.c_str(), &_db);

    if(rCode != SQLITE_OK) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(_db);
    }
}

DBManager::~DBManager() {
    sqlite3_close(_db);
}
