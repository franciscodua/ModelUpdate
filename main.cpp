#include <iostream>

#include "bootstrap.h"

using namespace std;

int main() {
    cout << "Init DB:" << endl;
    init_db("/Users/francisco/Documents/Working-dir/ModelUpdate/db.sqlite");
    return 0;
}