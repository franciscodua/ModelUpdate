//
// Created by Francisco Duarte on 01/02/16.
//

#ifndef MODELUPDATE_UPDATE_H
#define MODELUPDATE_UPDATE_H

#include "DBManager.h"

//TODO: define more meaningful threshold
#define ERROR_THRESHOLD 100

int assingFunctionToSample(Sample sample, DBManager *dbManager);

// uses gsl lib
void updateFunctions(DBManager *dbManager);

// uses R script
void updateFunctions();

#endif //MODELUPDATE_UPDATE_H
