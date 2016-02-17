#include <iostream>

#include "bootstrap.h"
#include "update.h"

void test1(DBManager *dbManager) {
    std::vector<float> weights, minRange, maxRange;
    float intersection;

    weights = {2.5, 2.5};
    minRange = {1, 1};
    maxRange = {100, 100};
    intersection = 100;

    ImpactFunction *t1 = new ImpactFunction(0, 3, intersection, weights, minRange, maxRange);

    generateRealSamples(*t1, dbManager);

    weights[0] = 1.5;
    weights[1] = 1.5;
    intersection = 2;

    ImpactFunction *t2 = new ImpactFunction(0, 3, intersection, weights, minRange, maxRange);

    generateRealSamples(*t2, dbManager);

    delete(t1);
    delete(t2);
}

void test2(DBManager *dbManager) {
    std::vector<float> weights, minRange, maxRange;
    float intersection;

    weights = {1, 1};
    minRange = {1, 1};
    maxRange = {100, 100};
    intersection = 600;

    ImpactFunction *t1 = new ImpactFunction(0, 3, intersection, weights, minRange, maxRange);

    generateRealSamples(*t1, dbManager);

    delete(t1);
}

void test3(DBManager *dbManager) {
    std::vector<float> weights, minRange, maxRange;
    float intersection;

    weights = {2.5, 2.5};
    minRange = {1, 1};
    maxRange = {100, 100};
    intersection = 500;

    ImpactFunction *t1 = new ImpactFunction(0, 3, intersection, weights, minRange, maxRange);

    generateRealSamples(*t1, dbManager);

    intersection = 10000;

    ImpactFunction *t2 = new ImpactFunction(0, 3, intersection, weights, minRange, maxRange);

    generateRealSamples(*t2, dbManager);

    delete(t1);
    delete(t2);
}

void init(DBManager *dbManager) {
    std::vector<float> weights, minRange, maxRange;
    float intersection;

    std::cout << "Init DB:" << std::endl;

    dbManager->initDb();

    weights = {2, 3};
    intersection = 100;
    minRange = {1, 1};
    maxRange = {100, 100};

    // Bootstrapping
    ImpactFunction *impact = new ImpactFunction(0.8, 3, intersection, weights, minRange, maxRange);

    generateSynthetic(*impact, dbManager);


    delete(impact);

    weights[0] = 1;
    weights[1] = 1;
    intersection = 2;

    impact = new ImpactFunction(0.8, 3, intersection, weights, minRange, maxRange);

    generateSynthetic(*impact, dbManager);

    delete(impact);
}

int main() {
    DBManager *dbManager;

    dbManager = new DBManager("/Users/francisco/Documents/IST/Working-dir/ModelUpdate/db.sqlite");

    //init(dbManager);
    //test1(dbManager);
    test2(dbManager);
    //test3(dbManager);

    delete(dbManager);

    // UpdateModel
    updateFunctions();

    return 0;
}