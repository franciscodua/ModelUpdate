//
// Created by Francisco Duarte on 27/01/16.
//

#include <iosfwd>
#include <string>
#include <sqlite3.h>
#include <iostream>
#include "bootstrap.h"

int drop_tables(sqlite3 *db) {
    int rCode;
    const char *sqlDropSamples, *sqlDropImpact;
    char *errMsg;

    sqlDropSamples = "DROP TABLE IF EXISTS Samples;";
    sqlDropImpact = "DROP TABLE IF EXISTS Impact_Functions";

    rCode = sqlite3_exec(db, sqlDropSamples, NULL, NULL, &errMsg);

    if (rCode != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", errMsg);
        sqlite3_free(errMsg);
        return 1;
    }
    std::cout << "SQL: Samples Dropped" << std::endl;

    rCode = sqlite3_exec(db, sqlDropImpact, NULL, NULL, &errMsg);

    if (rCode != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", errMsg);
        sqlite3_free(errMsg);
        return 1;
    }
    std::cout << "SQL: Impact_Functions Dropped" << std::endl;

    return 0;
}

int create_tables(sqlite3 *db) {
    int rCode;
    const char* sqlCreateSamples;
    const char* sqlCreateImpact;
    char *errMsg;

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
            "ImpactId INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,"
            "weightRspTime INT NOT NULL,"
            "weightResources INT NOT NULL,"
            "minRangeRspTime INT NOT NULL,"
            "maxRangeRspTime INT NOT NULL,"
            "minRangeResources INT NOT NULL,"
            "maxRangeResources INT NOT NULL)";

    rCode = sqlite3_exec(db, sqlCreateImpact, NULL, NULL, &errMsg);

    if (rCode != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", errMsg);
        sqlite3_free(errMsg);
        return 1;
    }

    std::cout << "SQL: Impact_Functions Created" << std::endl;

    return 0;
}

int add_impact_function(ImpactFunction *impact, sqlite3 *db) {
    std::string sqlInsert;
    sqlite3_stmt *stmt;
    int rCode;
    int weightRspTime, weightResources;
    int minRangeRspTime, maxRangeRspTime;
    int minRangeResources, maxRangeResources;

    sqlInsert = "INSERT INTO Impact_Functions"
            "(weightRspTime, weightResources, minRangeRspTime, maxRangeRspTime, minRangeResources, maxRangeResources)"
            "VALUES (?, ?, ?, ?, ?, ?);";

    rCode = sqlite3_prepare(db, sqlInsert.c_str(), -1, &stmt, 0);

    if (rCode != SQLITE_OK) {
        fprintf(stderr, "Could not prepare statement: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    weightRspTime = impact->getWeight(0);
    weightResources = impact->getWeight(1);
    sqlite3_bind_int(stmt, 1, weightRspTime);
    sqlite3_bind_int(stmt, 2, weightResources);

    minRangeRspTime = impact->getMinRange(0);
    maxRangeRspTime = impact->getMaxRange(0);
    sqlite3_bind_int(stmt, 3, minRangeRspTime);
    sqlite3_bind_int(stmt, 4, maxRangeRspTime);

    minRangeResources = impact->getMinRange(1);
    maxRangeResources = impact->getMaxRange(1);
    sqlite3_bind_int(stmt, 5, minRangeResources);
    sqlite3_bind_int(stmt, 6, maxRangeResources);

    rCode = sqlite3_step(stmt);

    if (rCode != SQLITE_DONE) {
        fprintf(stderr, "Could not execute statement (Insert): %s\n", sqlite3_errmsg(db));
        return -1;
    }

    sqlite3_finalize(stmt);

    // TODO: Alternative would be to store rowid or select max
    return (int) sqlite3_last_insert_rowid(db);
}

void add_sample(int rspTime, int resources, int newRspTime, int impactId, sqlite3 *db) {
    std::string sqlInsert;
    sqlite3_stmt *stmt;
    int rCode;

    sqlInsert = "INSERT INTO Samples"
            "(?, ?, ?, ?);";

    rCode = sqlite3_prepare(db, sqlInsert.c_str(), -1, &stmt, 0);

    if (rCode != SQLITE_OK) {
        fprintf(stderr, "Could not prepare statement: %s\n", sqlite3_errmsg(db));
        return;
    }

    sqlite3_bind_int(stmt, 1, rspTime);
    sqlite3_bind_int(stmt, 2, resources);
    sqlite3_bind_int(stmt, 3, newRspTime);
    sqlite3_bind_int(stmt, 4, impactId);

    rCode = sqlite3_step(stmt);

    if (rCode != SQLITE_DONE) {
        fprintf(stderr, "Could not execute statement (Insert): %s\n", sqlite3_errmsg(db));
        return;
    }

    sqlite3_finalize(stmt);

    return;
}

int init_db (std::string file) {
    sqlite3 *db;
    int rCode;

    rCode = sqlite3_open(file.c_str(), &db);

    if(rCode != SQLITE_OK) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
    }

    if (drop_tables(db)) {
        return 1;
    }

    if (create_tables(db)) {
        return 1;
    }

    sqlite3_close(db);
    return 0;
}

void generate_synthetic (ImpactFunction *impact, std::string file) {
    sqlite3 *db;
    int rCode;
    int impactID;

    rCode = sqlite3_open(file.c_str(), &db);

    if(rCode != SQLITE_OK) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
    }

    impactID = add_impact_function(impact, db);


}