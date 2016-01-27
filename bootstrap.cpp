//
// Created by Francisco Duarte on 27/01/16.
//

#include <iosfwd>
#include <string>
#include <sqlite3.h>
#include <iostream>
#include "bootstrap.h"


int init_db (std::string file) {
    sqlite3 *db;
    int rCode;
    const char* sqlCreateSamples;
    const char* sqlCreateImpact;
    char *errMsg;

    rCode = sqlite3_open(file.c_str(), &db);

    if(rCode != SQLITE_OK) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
    }

    sqlCreateSamples =    "CREATE TABLE Samples ("
            "RspTime INT    NOT NULL,"
            "Resources INT  NOT NULL,"
            "NewRspTime INT NOT NULL,"
            "Impact INT     NOT NULL,"
            "FOREIGN KEY(Impact) REFERENCES Impact_Functions(ImpactId));";

    rCode = sqlite3_exec(db, sqlCreateSamples, NULL, NULL, &errMsg);

    if (rCode != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", errMsg);
        sqlite3_free(errMsg);
        return 1;
    }
    std::cout << "SQL: Samples Created" << std::endl;

    sqlCreateImpact = "CREATE TABLE Impact_Functions("
            "ImpactId INT PRIMARY KEY  NOT NULL,"
            "weightRspTime INT NOT NULL,"
            "weightResources INT NOT NULL,"
            "rangeRspTime INT NOT NULL,"
            "rangeResources INT NOT NULL)";

    rCode = sqlite3_exec(db, sqlCreateImpact, NULL, NULL, &errMsg);

    if (rCode != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", errMsg);
        sqlite3_free(errMsg);
        return 1;
    }

    std::cout << "SQL: Impact_Functions Created" << std::endl;

    sqlite3_close(db);
    return 0;
}
