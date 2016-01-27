#include <iostream>

#include "bootstrap.h"

using namespace std;

int main() {
    cout << "Init DB:" << endl;
    init_db("/Users/francisco/Documents/Working-dir/ModelUpdate/db.sqlite");

    int weights[] = {2, 3};
    int minRange[] = {1, 2};
    int maxRange[] = {100, 200};


    ImpactFunction *impact = new ImpactFunction(0.8, 3, weights, minRange, maxRange);

    generate_synthetic(impact, "/Users/francisco/Documents/Working-dir/ModelUpdate/db.sqlite");

    return 0;
}