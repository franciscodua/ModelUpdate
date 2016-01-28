//
// Created by Francisco Duarte on 27/01/16.
//

#include <iosfwd>
#include <string>
#include <sqlite3.h>
#include <iostream>
#include "bootstrap.h"
#include <cmath>

#define N_SAMPLES 100

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
            "RspTime REAL    NOT NULL,"
            "Resources REAL  NOT NULL,"
            "NewRspTime REAL NOT NULL,"
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
            "weightRspTime REAL NOT NULL,"
            "weightResources REAL NOT NULL,"
            "minRangeRspTime REAL NOT NULL,"
            "maxRangeRspTime REAL NOT NULL,"
            "minRangeResources REAL NOT NULL,"
            "maxRangeResources REAL NOT NULL)";

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
    float weightRspTime, weightResources;
    float minRangeRspTime, maxRangeRspTime;
    float minRangeResources, maxRangeResources;

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
    sqlite3_bind_double(stmt, 1, weightRspTime);
    sqlite3_bind_double(stmt, 2, weightResources);

    minRangeRspTime = impact->getMinRange(0);
    maxRangeRspTime = impact->getMaxRange(0);
    sqlite3_bind_double(stmt, 3, minRangeRspTime);
    sqlite3_bind_double(stmt, 4, maxRangeRspTime);

    minRangeResources = impact->getMinRange(1);
    maxRangeResources = impact->getMaxRange(1);
    sqlite3_bind_double(stmt, 5, minRangeResources);
    sqlite3_bind_double(stmt, 6, maxRangeResources);

    rCode = sqlite3_step(stmt);

    if (rCode != SQLITE_DONE) {
        fprintf(stderr, "Could not execute statement (Insert): %s\n", sqlite3_errmsg(db));
        return -1;
    }

    sqlite3_finalize(stmt);

    // TODO: Alternative would be to store rowid or select max
    return (int) sqlite3_last_insert_rowid(db);
}

void add_sample(float rspTime, float resources, float newRspTime, int impactId, sqlite3 *db) {
    std::string sqlInsert;
    sqlite3_stmt *stmt;
    int rCode;

    sqlInsert = "INSERT INTO Samples VALUES"
            "(?, ?, ?, ?);";

    rCode = sqlite3_prepare(db, sqlInsert.c_str(), -1, &stmt, 0);

    if (rCode != SQLITE_OK) {
        fprintf(stderr, "Could not prepare statement: %s\n", sqlite3_errmsg(db));
        return;
    }

    sqlite3_bind_double(stmt, 1, rspTime);
    sqlite3_bind_double(stmt, 2, resources);
    sqlite3_bind_double(stmt, 3, newRspTime);
    sqlite3_bind_int(stmt, 4, impactId);

    rCode = sqlite3_step(stmt);

    if (rCode != SQLITE_DONE) {
        fprintf(stderr, "Could not execute statement (Insert): %s\n", sqlite3_errmsg(db));
        return;
    }

    std::cout << "Sample generated: (" << rspTime << ", " << resources << ", " << newRspTime
    << ", " << impactId << ")" << std::endl;

    sqlite3_finalize(stmt);

    return;
}

void generate_samples(ImpactFunction *impact, int impactId, sqlite3 *db) {
    int nDimensions;
    int samples, samplesPerRow;
    float samplingIntervalX, samplingIntervalY;
    std::vector<float> vars (2);
    //int *minRanges, *maxRanges;
    std::vector<int> minRanges(2);
    std::vector<int> maxRanges(2);
    int rest;
    int samplingLastCol;

    nDimensions = impact->getNDimensions();
    //vars = new float[nDimensions-1];
    //minRanges = new int [nDimensions-1];
    //maxRanges = new int [nDimensions-1];
    samples = (int) (impact->getPercentage() * N_SAMPLES);

    for (int i = 0; i < nDimensions-1; i++) {
        minRanges[i] = (int) impact->getMinRange(i);
        maxRanges[i] = (int) impact->getMaxRange(i);
    }

    samplingIntervalX = (float) ((maxRanges[0] - minRanges[0]) / floor(sqrt(samples)));
    samplingIntervalY = (float) ((maxRanges[1] - minRanges[1]) / floor(sqrt(samples)));

    samplesPerRow = floor(sqrt(samples));
    rest = samples - (samplesPerRow*samplesPerRow);

    for (int x = 0; x < samplesPerRow; x++) {
        for (int y = 0; y < samplesPerRow; y++) {
            vars[0] = minRanges[0] + (x * samplingIntervalX) + (samplingIntervalX / 2);
            vars[1] = minRanges[1] + (y * samplingIntervalY) + (samplingIntervalX / 2);
            add_sample(vars[0], vars[1], impact->computeOutput(vars), impactId, db);
        }
    }

    samplingLastCol = (maxRanges[1] - minRanges[1]) / rest;
    vars[0] = samplesPerRow * samplingIntervalX;
    for (int i = 0; i < rest; i++) {
        vars[1] = minRanges[0] + (i * samplingLastCol) + (samplingLastCol / 2);

        add_sample(vars[0], vars[1], impact->computeOutput(vars), impactId, db);
    }

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

    generate_samples(impact, impactID, db);
}