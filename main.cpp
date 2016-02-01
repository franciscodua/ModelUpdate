#include <iostream>

#include "bootstrap.h"

using namespace std;

int main() {
    std::vector<float> weights, minRange, maxRange;
    DBManager *dbManager;
    dbManager = new DBManager("/Users/francisco/Documents/Working-dir/ModelUpdate/db.sqlite");

    cout << "Init DB:" << endl;
    dbManager->initDb();

    weights = {2, 3};
    minRange = {1, 1};
    maxRange = {100, 100};

    ImpactFunction *impact = new ImpactFunction(0.8, 3, weights, minRange, maxRange);

    generateSynthetic(*impact, dbManager);

    delete(dbManager);
    delete(impact);

    return 0;
}