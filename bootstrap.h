//
// Created by Francisco Duarte on 27/01/16.
//

#ifndef MODELUPDATE_BOOTSTRAP_H
#define MODELUPDATE_BOOTSTRAP_H

#include <string>
#include "ImpactFunction.h"
#include "DBManager.h"
/*
 * Receives impact function inserts it in Impact_Functions, generates samples from it and adds them to database
 */
void generateSynthetic(ImpactFunction impact, DBManager *dbManager);

#endif //MODELUPDATE_BOOTSTRAP_H
