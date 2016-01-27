//
// Created by Francisco Duarte on 27/01/16.
//

#ifndef MODELUPDATE_BOOTSTRAP_H
#define MODELUPDATE_BOOTSTRAP_H

#include <string>
#include "ImpactFunction.h"

int init_db(std::string file);
void generate_synthetic (ImpactFunction *impact, std::string file);

#endif //MODELUPDATE_BOOTSTRAP_H
