#include <iostream>

#include "bootstrap.h"

using namespace std;

int main() {
    std::vector<float> weights, minRange, maxRange;

    cout << "Init DB:" << endl;
    init_db("/Users/francisco/Documents/Working-dir/ModelUpdate/db.sqlite");

    /*float weights[] = {2, 3};
    float minRange[] = {1, 1};
    float maxRange[] = {100, 100};*/

    weights = {2, 3};
    minRange = {1, 1};
    maxRange = {100, 100};

    ImpactFunction *impact = new ImpactFunction(0.8, 3, weights, minRange, maxRange);

    generate_synthetic(impact, "/Users/francisco/Documents/Working-dir/ModelUpdate/db.sqlite");


    delete(impact);

    return 0;
}