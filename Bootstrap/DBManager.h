//
// Created by Francisco Duarte on 29/01/16.
//

#ifndef MODELUPDATE_DBMANAGER_H
#define MODELUPDATE_DBMANAGER_H

#include<string>
#include <sqlite3.h>
#include "ImpactFunction.h"
#include "Sample.h"

/*
 * Creates and updates the database.
 * Opens connection to SQLite database in constructor,
 * closes in destructor.
 */

class DBManager {
private:
    std::string _fileName;
    sqlite3 *_db;

    int createTables();
    int dropTables();

public:
    DBManager(std::string fileName);
    ~DBManager();


    /*
     * If tables exist they are dropped
     * and new tables are created.
     * Samples and Impact_Functions.
     * Returns DBMANAGER_ERR if unsuccessful and DBMANAGER_OK otherwise
     */
    int initDb();

    /*
     * Adds impact function details to table Impact_Functions.
     *  _____________________________________________________________________________________________________________________________________
     *  |ImpactId|probability|weightRspTime|weightResources|intersection|minRangeRspTime|maxRangeRspTime|minRangeResources|maxRangeResources|
     *  \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
     *  | INT    | REAL      | REAL        | REAL          | REAL       | REAL          | REAL          | REAL            | REAL            |
     *
     * Receives impact which should have the details of an impact function
     * Returns DBMANAGER_ERR if unsuccessful and ImpactId otherwise
     */
    int addImpactFunction(ImpactFunction impact);

    /*
     * Adds sample details to table Samples.
     *
     *  _________________________________________________________________________________________
     *  |   SampleId    |   RspTime |   Resources   |   NewRspTime  |   Impact  |   Synthetic   |
     *  \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
     *  | INT           | REAL      | REAL          | REAL          | INT       | BOOL (INT)    |
     *
     * Receives sample which should have the details of a sample.
     * Returns DBMANAGER_ERR if unsuccessful and DBMANAGER_OK otherwise
     */
    int addSample(Sample sample);

    std::vector<ImpactFunction> getImpactFunctions();

    std::vector<int> getImpactIds();

    std::vector<Sample> getSamples(int impactId);
};


#endif //MODELUPDATE_DBMANAGER_H
