#include <iostream>

#include "bootstrap.h"
#include "update.h"
#include <memory>

int main() {
    std::vector<float> weights, minRange, maxRange;
    float intersection;
    DBManager *dbManager;
    //DBManager *dbManager;
    /*dbManager = std::make_shared<DBManager>(new DBManager(
            "/Users/francisco/Documents/IST/Working-dir/ModelUpdate/db.sqlite"));*/
    dbManager = new DBManager("/Users/francisco/Documents/IST/Working-dir/ModelUpdate/db.sqlite");

    std::cout << "Init DB:" << std::endl;
    dbManager->initDb();

    weights = {2, 3};
    intersection = 2;
    minRange = {1, 1};
    maxRange = {100, 100};

    // Bootstrapping
    ImpactFunction *impact = new ImpactFunction(0.8, 3, intersection, weights, minRange, maxRange);

    generateSynthetic(*impact, dbManager);

    delete(dbManager);
    delete(impact);

    // UpdateModel
    updateFunctions();

    return 0;
}