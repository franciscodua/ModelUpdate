//
// Created by Francisco Duarte on 29/01/16.
//

#ifndef MODELUPDATE_DBMANAGER_H
#define MODELUPDATE_DBMANAGER_H

#include<string>
#include <sqlite3.h>
#include "ImpactFunction.h"
#include "Sample.h"

class DBManager {
private:
    std::string _fileName;
    sqlite3 *_db;

public:
    DBManager(std::string fileName);
    ~DBManager();

    int init_db();

    int create_tables();
    int drop_tables();

    int add_impact_function(ImpactFunction impact);

    int add_sample(Sample sample);
};


#endif //MODELUPDATE_DBMANAGER_H
