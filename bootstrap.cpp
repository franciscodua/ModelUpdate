//
// Created by Francisco Duarte on 27/01/16.
//

#include <iosfwd>
#include <string>
#include <iostream>
#include "bootstrap.h"

#define N_SAMPLES 100

/*
 * Generates samples from impact function and inserts them in database.
 */
void generate_samples(ImpactFunction impact, int impactId, DBManager *dbManager) {
    int nDimensions;
    int samples, samplesPerRow;
    float samplingIntervalX, samplingIntervalY;
    std::vector<float> vars (2);
    std::vector<int> minRanges(2);
    std::vector<int> maxRanges(2);
    int rest, restCol, restRow;
    int samplingLastCol, samplingLastRow;
    Sample *sample;

    nDimensions = impact.getNDimensions();
    // number of samples to be generated. Probability times constant
    samples = (int) (impact.getProbability() * N_SAMPLES);

    for (int i = 0; i < nDimensions-1; i++) {
        minRanges[i] = (int) impact.getMinRange(i);
        maxRanges[i] = (int) impact.getMaxRange(i);
    }

    // sampling intervals: delta range divided by number of points per row
    // assuming squared space
    samplingIntervalX = (float) ((maxRanges[0] - minRanges[0]) / ceil(sqrt(samples)));
    samplingIntervalY = (float) ((maxRanges[1] - minRanges[1]) / ceil(sqrt(samples)));

    samplesPerRow = (int) floor(sqrt(samples));
    rest = samples - (samplesPerRow*samplesPerRow);

    sample = new Sample(0, 0, 0, impactId, true);

    // generates samples. floor * floor <= samples
    for (int x = 0; x < samplesPerRow; x++) {
        for (int y = 0; y < samplesPerRow; y++) {
            vars[0] = minRanges[0] + (x * samplingIntervalX) + (samplingIntervalX / 2);
            vars[1] = minRanges[1] + (y * samplingIntervalY) + (samplingIntervalX / 2);

            sample->setRspTime(vars[0]);
            sample->setResources(vars[1]);
            sample->setNewRspTime(impact.computeOutput(vars));

            dbManager->addSample(*sample);
        }
    }

    // remaining samples are added to the last column and last row (half each)
    restRow =(int) floor(rest / 2);
    restCol = rest - restRow;

    samplingLastRow = (maxRanges[0] - minRanges[0]) / restRow;
    samplingLastCol = (maxRanges[1] - minRanges[1]) / restCol;

    vars[0] = samplesPerRow * samplingIntervalX;
    sample->setRspTime(vars[0]);
    for (int i = 0; i < restRow; i++) {
        vars[1] = minRanges[1] + (i * samplingLastRow) + (samplingLastRow / 2);
        sample->setResources(vars[1]);
        sample->setNewRspTime(impact.computeOutput(vars));

        dbManager->addSample(*sample);
    }

    vars[1] = samplesPerRow * samplingIntervalY;
    sample->setResources(vars[1]);
    for (int i = 0; i < restRow; i++) {
        vars[0] = minRanges[0] + (i * samplingLastCol) + (samplingLastCol / 2);
        sample->setRspTime(vars[0]);
        sample->setNewRspTime(impact.computeOutput(vars));

        dbManager->addSample(*sample);
    }
    delete(sample);
}


void generateSynthetic(ImpactFunction impact, DBManager *dbManager) {
    int impactId;
    // adds function to table
    impactId = dbManager->addImpactFunction(impact);

    generate_samples(impact, impactId, dbManager);
}