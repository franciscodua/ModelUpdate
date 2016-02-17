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
        fprintf(stderr, "Constructor: Can't open database: %s\n", sqlite3_errmsg(_db));
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
            "Synthetic INT  NOT NULL,"
            "FOREIGN KEY(Impact) REFERENCES Impact_Functions(ImpactId));";

    rCode = sqlite3_exec(_db, sqlCreateSamples, NULL, NULL, &errMsg);

    if (rCode != SQLITE_OK) {
        fprintf(stderr, "createSamples: SQL error: %s\n", errMsg);
        sqlite3_free(errMsg);
        return DBMANAGER_ERR;
    }
    std::cout << "SQL: Samples Created" << std::endl;

    sqlCreateImpact = "CREATE TABLE Impact_Functions("
            "ImpactId INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,"
            "probability REAL NOT NULL,"
            "weightRspTime REAL NOT NULL,"
            "weightResources REAL NOT NULL,"
            "intersection REAL NOT NULL,"
            "minRangeRspTime REAL NOT NULL,"
            "maxRangeRspTime REAL NOT NULL,"
            "minRangeResources REAL NOT NULL,"
            "maxRangeResources REAL NOT NULL);";

    rCode = sqlite3_exec(_db, sqlCreateImpact, NULL, NULL, &errMsg);

    if (rCode != SQLITE_OK) {
        fprintf(stderr, "createImpact: SQL error: %s\n", errMsg);
        sqlite3_free(errMsg);
        return DBMANAGER_ERR;
    }

    // No Impact Function
    sqlInsertNoFunc = "INSERT INTO Impact_Functions("
            "probability, weightRspTime, weightResources, intersection,"
            "minRangeRspTime, maxRangeRspTime, minRangeResources,"
            "maxRangeResources)"
            "VALUES (0, 0, 0, 0, 0, 0, 0, 0);";

    rCode = sqlite3_exec(_db, sqlInsertNoFunc, NULL, NULL, &errMsg);

    if (rCode != SQLITE_OK) {
        fprintf(stderr, "insertNoFunc: SQL error: %s\n", errMsg);
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
        fprintf(stderr, "dropSamples: SQL error: %s\n", errMsg);
        sqlite3_free(errMsg);
        return DBMANAGER_ERR;
    }
    std::cout << "SQL: Samples Dropped" << std::endl;

    rCode = sqlite3_exec(_db, sqlDropImpact, NULL, NULL, &errMsg);

    if (rCode != SQLITE_OK) {
        fprintf(stderr, "dropImpact: SQL error: %s\n", errMsg);
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
    float probability;
    float weightRspTime, weightResources;
    float intersection;
    float minRangeRspTime, maxRangeRspTime;
    float minRangeResources, maxRangeResources;

    sqlInsert = "INSERT INTO Impact_Functions"
            "(probability, weightRspTime, weightResources, intersection,"
            "minRangeRspTime, maxRangeRspTime, minRangeResources,"
            "maxRangeResources)"
            "VALUES (?, ?, ?, ?, ?, ?, ?, ?);";

    rCode = sqlite3_prepare(_db, sqlInsert.c_str(), -1, &stmt, 0);

    if (rCode != SQLITE_OK) {
        fprintf(stderr, "addImpactFunction: Could not prepare statement: %s\n", sqlite3_errmsg(_db));
        return DBMANAGER_ERR;
    }

    // this could be squashed
    probability = impact.getProbability();
    weightRspTime = impact.getWeight(0);
    weightResources = impact.getWeight(1);
    intersection = impact.getIntersection();
    minRangeRspTime = impact.getMinRange(0);
    maxRangeRspTime = impact.getMaxRange(0);
    minRangeResources = impact.getMinRange(1);
    maxRangeResources = impact.getMaxRange(1);

    sqlite3_bind_double(stmt, 1, probability);
    sqlite3_bind_double(stmt, 2, weightRspTime);
    sqlite3_bind_double(stmt, 3, weightResources);
    sqlite3_bind_double(stmt, 4, intersection);
    sqlite3_bind_double(stmt, 5, minRangeRspTime);
    sqlite3_bind_double(stmt, 6, maxRangeRspTime);
    sqlite3_bind_double(stmt, 7, minRangeResources);
    sqlite3_bind_double(stmt, 8, maxRangeResources);

    rCode = sqlite3_step(stmt);

    if (rCode != SQLITE_DONE) {
        fprintf(stderr, "addImpactFunction: Could not execute statement (Insert): %s\n", sqlite3_errmsg(_db));
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
    int synthetic;

    sqlInsert = "INSERT INTO Samples"
            "(RspTime, Resources, NewRspTime, Impact, Synthetic)"
            "VALUES (?, ?, ?, ?, ?);";

    rCode = sqlite3_prepare(_db, sqlInsert.c_str(), -1, &stmt, 0);

    if (rCode != SQLITE_OK) {
        fprintf(stderr, "addSample: Could not prepare statement: %s\n", sqlite3_errmsg(_db));
        return DBMANAGER_ERR;
    }

    // this could be squashed but is used in print
    rspTime = sample.getRspTime();
    resources = sample.getResources();
    newRspTime = sample.getNewRspTime();
    impactId = sample.getImpactId();
    synthetic = (sample.getSynthetic())? 1 : 0;

    sqlite3_bind_double(stmt, 1, rspTime);
    sqlite3_bind_double(stmt, 2, resources);
    sqlite3_bind_double(stmt, 3, newRspTime);
    sqlite3_bind_int(stmt, 4, impactId);
    sqlite3_bind_int(stmt, 5, synthetic);

    rCode = sqlite3_step(stmt);

    if (rCode != SQLITE_DONE) {
        fprintf(stderr, "addSample: Could not execute statement (Insert): %s\n", sqlite3_errmsg(_db));
        return DBMANAGER_ERR;
    }

    //std::cout << "Sample generated: (" << rspTime << ", " << resources << ", " << newRspTime
    //<< ", " << impactId << ")" << std::endl;

    sqlite3_finalize(stmt);

    return DBMANAGER_OK;
}

std::vector<ImpactFunction> DBManager::getImpactFunctions() {
    std::vector<ImpactFunction> result;
    const char *sqlSelect;
    sqlite3_stmt *stmt;
    int rCode;


    // impact variables
    int impactId;
    float probability;
    float intersection;
    std::vector<float> *weights;
    std::vector<float> *minRange;
    std::vector<float> *maxRange;

    sqlSelect = "SELECT * FROM Impact_Functions WHERE ImpactId > 1;";

    result = std::vector<ImpactFunction>();
    weights = new std::vector<float>(2);
    minRange = new std::vector<float>(2);
    maxRange = new std::vector<float>(2);

    sqlite3_prepare(_db, sqlSelect, -1, &stmt, 0);

    while(true) {
        rCode = sqlite3_step(stmt);
        if (rCode == SQLITE_ROW) {
            impactId = sqlite3_column_int(stmt, 0);
            probability = (float) sqlite3_column_double(stmt, 1);
            weights->at(0) = (float) sqlite3_column_double(stmt, 2);
            weights->at(1) = (float) sqlite3_column_double(stmt, 3);
            intersection = (float) sqlite3_column_double(stmt, 4);
            minRange->at(0) = (float) sqlite3_column_double(stmt, 5);
            maxRange->at(0) = (float) sqlite3_column_double(stmt, 6);
            minRange->at(1) = (float) sqlite3_column_double(stmt, 7);
            maxRange->at(1) = (float) sqlite3_column_double(stmt, 8);

            // 3 is the number of dimensions
            result.insert(result.end(), ImpactFunction(impactId, probability, 3,
                                                       intersection, *weights, *minRange, *maxRange));
        }
        else if (rCode == SQLITE_DONE) {
            break;
        }

        else {
            fprintf(stderr, "getImpactFunctions: Could not step stmt: %s\n", sqlite3_errmsg(_db));
            result = std::vector<ImpactFunction>();
            break;
        }
    }

    sqlite3_finalize(stmt);

    delete(weights);
    delete(minRange);
    delete(maxRange);

    return result;
}

std::vector<int> DBManager::getImpactIds() {
    const char *sqlSelect;
    std::vector<int> ids;
    sqlite3_stmt *stmt;
    int rCode;
    int id;

    sqlSelect = "SELECT ImpactId FROM Impact_Functions WHERE ImpactId > 1;";
    ids = std::vector<int>();

    sqlite3_prepare(_db, sqlSelect, -1, &stmt, 0);

    while(true) {
        rCode = sqlite3_step(stmt);
        if (rCode == SQLITE_ROW) {
            id = sqlite3_column_int(stmt, 0);
            ids.insert(ids.end(), id);
        }

        else if (rCode == SQLITE_DONE) {
            break;
        }

        else {
            fprintf(stderr, "getImpactIds: Could not step stmt: %s\n", sqlite3_errmsg(_db));
            ids = std::vector<int>();
            break;
        }
    }

    sqlite3_finalize(stmt);

    return ids;
}

std::vector<Sample> DBManager::getSamples(int impactId) {
    std::string sqlSelect;
    std::vector<Sample> samples;
    sqlite3_stmt *stmt;
    int rCode;
    Sample *sample;

    sqlSelect = "SELECT RspTime, Resources, NewRspTime FROM Samples WHERE Impact =" +
            std::to_string(impactId) + " ;";

    samples = std::vector<Sample>();

    // Being synthetic or not does not affect result
    sample = new Sample(0, 0, 0, false);

    sqlite3_prepare(_db, sqlSelect.c_str(), -1, &stmt, 0);

    while(true) {
        rCode = sqlite3_step(stmt);
        if (rCode == SQLITE_ROW) {
            sample->setRspTime(sqlite3_column_int(stmt, 0));
            sample->setResources(sqlite3_column_int(stmt, 1));
            sample->setNewRspTime(sqlite3_column_int(stmt, 2));

            samples.insert(samples.end(), *sample);
        }

        else if (rCode == SQLITE_DONE) {
            break;
        }

        else {
            fprintf(stderr, "getSamples: Could not step stmt: %s\n", sqlite3_errmsg(_db));
            samples = std::vector<Sample>();
            break;
        }
    }

    delete(sample);
    sqlite3_finalize(stmt);

    return samples;
}
