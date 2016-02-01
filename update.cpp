//
// Created by Francisco Duarte on 01/02/16.
//

#include "update.h"

int assingFunctionToSample(Sample sample, DBManager *dbManager) {
    int impactId;
    float error, minError;
    std::vector<ImpactFunction> impactFunctions;

    //NO IMPACT FUNCTION
    impactId = 1;
    minError = ERROR_THRESHOLD;

    impactFunctions = dbManager->getImpactFunctions();

    for (int i = 0; i < impactFunctions.size(); i++) {
        error = impactFunctions.at(i).computeError(sample);

        if (error < minError) {
            minError = error;
            impactId = impactFunctions.at(i).getImpactId();
        }
    }

    // set impactId that better represents sample
    sample.set_impactId(impactId);

    // adds sample to DB
    dbManager->addSample(sample);

    return impactId;
}
