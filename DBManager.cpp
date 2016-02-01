//
// Created by Francisco Duarte on 29/01/16.
//

#include <iostream>
#include "DBManager.h"

#define DBMANAGER_OK 0
#define DBMANAGER_ERR -1

DBManager::DBManager(std::string fileName): _fileName(fileName) {
    int rCode;
    // opens connection to SQLite database
    rCode = sqlite3_open(_fileName.c_str(), &_db);

    if(rCode != SQLITE_OK) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(_db));
        sqlite3_close(_db);
    }
}

DBManager::~DBManager() {
    sqlite3_close(_db);
}

int DBManager::initDb() {
    // drop tables if exists
    if (dropTables()) {
        return DBMANAGER_ERR;
    }

    // create new tables Samples and Impact_Functions
    if (createTables()) {
        return DBMANAGER_ERR;
    }
    return DBMANAGER_OK;
}

int DBManager::createTables() {
    int rCode;
    const char* sqlCreateSamples;
    const char* sqlCreateImpact;
    const char* sqlInsertNoFunc;
    char *errMsg;

    sqlCreateSamples =    "CREATE TABLE Samples ("
            "SampleId INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,"
            "RspTime REAL    NOT NULL,"
            "Resources REAL  NOT NULL,"
            "NewRspTime REAL NOT NULL,"
            "Impact INT     NOT NULL,"
            "FOREIGN KEY(Impact) REFERENCES Impact_Functions(ImpactId));";

    rCode = sqlite3_exec(_db, sqlCreateSamples, NULL, NULL, &errMsg);

    if (rCode != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", errMsg);
        sqlite3_free(errMsg);
        return DBMANAGER_ERR;
    }
    std::cout << "SQL: Samples Created" << std::endl;

    sqlCreateImpact = "CREATE TABLE Impact_Functions("
            "ImpactId INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,"
            "weightRspTime REAL NOT NULL,"
            "weightResources REAL NOT NULL,"
            "minRangeRspTime REAL NOT NULL,"
            "maxRangeRspTime REAL NOT NULL,"
            "minRangeResources REAL NOT NULL,"
            "maxRangeResources REAL NOT NULL)";

    rCode = sqlite3_exec(_db, sqlCreateImpact, NULL, NULL, &errMsg);

    if (rCode != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", errMsg);
        sqlite3_free(errMsg);
        return DBMANAGER_ERR;
    }

    // No Impact Function
    sqlInsertNoFunc = "INSERT INTO Impact_Functions("
            "(weightRspTime, weightResources, minRangeRspTime, maxRangeRspTime, minRangeResources, maxRangeResources)"
            "VALUES (0, 0, 0, 0, 0, 0);";

    rCode = sqlite3_exec(_db, sqlInsertNoFunc, NULL, NULL, &errMsg);

    if (rCode != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", errMsg);
        sqlite3_free(errMsg);
        return DBMANAGER_ERR;
    }

    std::cout << "SQL: Impact_Functions Created" << std::endl;

    return DBMANAGER_OK;
}

int DBManager::dropTables() {
    int rCode;
    const char *sqlDropSamples, *sqlDropImpact;
    char *errMsg;

    sqlDropSamples = "DROP TABLE IF EXISTS Samples;";
    sqlDropImpact = "DROP TABLE IF EXISTS Impact_Functions";

    rCode = sqlite3_exec(_db, sqlDropSamples, NULL, NULL, &errMsg);

    if (rCode != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", errMsg);
        sqlite3_free(errMsg);
        return DBMANAGER_ERR;
    }
    std::cout << "SQL: Samples Dropped" << std::endl;

    rCode = sqlite3_exec(_db, sqlDropImpact, NULL, NULL, &errMsg);

    if (rCode != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", errMsg);
        sqlite3_free(errMsg);
        return DBMANAGER_ERR;
    }
    std::cout << "SQL: Impact_Functions Dropped" << std::endl;

    return DBMANAGER_OK;
}

int DBManager::addImpactFunction(ImpactFunction impact) {
    std::string sqlInsert;
    sqlite3_stmt *stmt;
    int rCode;
    float weightRspTime, weightResources;
    float minRangeRspTime, maxRangeRspTime;
    float minRangeResources, maxRangeResources;

    sqlInsert = "INSERT INTO Impact_Functions"
            "(weightRspTime, weightResources, minRangeRspTime, maxRangeRspTime, minRangeResources, maxRangeResources)"
            "VALUES (?, ?, ?, ?, ?, ?);";

    rCode = sqlite3_prepare(_db, sqlInsert.c_str(), -1, &stmt, 0);

    if (rCode != SQLITE_OK) {
        fprintf(stderr, "Could not prepare statement: %s\n", sqlite3_errmsg(_db));
        return DBMANAGER_ERR;
    }

    // this could be squashed
    weightRspTime = impact.getWeight(0);
    weightResources = impact.getWeight(1);
    minRangeRspTime = impact.getMinRange(0);
    maxRangeRspTime = impact.getMaxRange(0);
    minRangeResources = impact.getMinRange(1);
    maxRangeResources = impact.getMaxRange(1);

    sqlite3_bind_double(stmt, 1, weightRspTime);
    sqlite3_bind_double(stmt, 2, weightResources);
    sqlite3_bind_double(stmt, 3, minRangeRspTime);
    sqlite3_bind_double(stmt, 4, maxRangeRspTime);
    sqlite3_bind_double(stmt, 5, minRangeResources);
    sqlite3_bind_double(stmt, 6, maxRangeResources);

    rCode = sqlite3_step(stmt);

    if (rCode != SQLITE_DONE) {
        fprintf(stderr, "Could not execute statement (Insert): %s\n", sqlite3_errmsg(_db));
        return DBMANAGER_ERR;
    }

    sqlite3_finalize(stmt);

    // TODO: Alternative would be to store rowid or select max
    return (int) sqlite3_last_insert_rowid(_db);
}

int DBManager::addSample(Sample sample) {
    std::string sqlInsert;
    sqlite3_stmt *stmt;
    int rCode;
    float rspTime, resources, newRspTime;
    int impactId;

    sqlInsert = "INSERT INTO Samples"
            "(RspTime, Resources, NewRspTime, Impact)"
            "VALUES (?, ?, ?, ?);";

    rCode = sqlite3_prepare(_db, sqlInsert.c_str(), -1, &stmt, 0);

    if (rCode != SQLITE_OK) {
        fprintf(stderr, "Could not prepare statement: %s\n", sqlite3_errmsg(_db));
        return DBMANAGER_ERR;
    }

    // this could be squashed but is used in print
    rspTime = sample.getRspTime();
    resources = sample.getResources();
    newRspTime = sample.getNewRspTime();
    impactId = sample.getImpactId();

    sqlite3_bind_double(stmt, 1, rspTime);
    sqlite3_bind_double(stmt, 2, resources);
    sqlite3_bind_double(stmt, 3, newRspTime);
    sqlite3_bind_int(stmt, 4, impactId);

    rCode = sqlite3_step(stmt);

    if (rCode != SQLITE_DONE) {
        fprintf(stderr, "Could not execute statement (Insert): %s\n", sqlite3_errmsg(_db));
        return DBMANAGER_ERR;
    }

    std::cout << "Sample generated: (" << rspTime << ", " << resources << ", " << newRspTime
    << ", " << impactId << ")" << std::endl;

    sqlite3_finalize(stmt);

    return DBMANAGER_OK;
}

