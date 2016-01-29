//
// Created by Francisco Duarte on 27/01/16.
//

#include <iosfwd>
#include <string>
#include <iostream>
#include "bootstrap.h"
#include <cmath>

#define N_SAMPLES 100

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
    samples = (int) (impact.getPercentage() * N_SAMPLES);

    for (int i = 0; i < nDimensions-1; i++) {
        minRanges[i] = (int) impact.getMinRange(i);
        maxRanges[i] = (int) impact.getMaxRange(i);
    }

    samplingIntervalX = (float) ((maxRanges[0] - minRanges[0]) / ceil(sqrt(samples)));
    samplingIntervalY = (float) ((maxRanges[1] - minRanges[1]) / ceil(sqrt(samples)));

    samplesPerRow = (int) floor(sqrt(samples));
    rest = samples - (samplesPerRow*samplesPerRow);

    sample = new Sample(0, 0, 0, impactId);

    for (int x = 0; x < samplesPerRow; x++) {
        for (int y = 0; y < samplesPerRow; y++) {
            vars[0] = minRanges[0] + (x * samplingIntervalX) + (samplingIntervalX / 2);
            vars[1] = minRanges[1] + (y * samplingIntervalY) + (samplingIntervalX / 2);

            sample->set_rspTime(vars[0]);
            sample->set_resources(vars[1]);
            sample->set_newRspTime(impact.computeOutput(vars));

            dbManager->add_sample(*sample);
        }
    }

    restRow =(int) floor(rest / 2);
    restCol = rest - restRow;

    samplingLastRow = (maxRanges[0] - minRanges[0]) / restRow;
    samplingLastCol = (maxRanges[1] - minRanges[1]) / restCol;

    vars[0] = samplesPerRow * samplingIntervalX;
    sample->set_rspTime(vars[0]);
    for (int i = 0; i < restRow; i++) {
        vars[1] = minRanges[1] + (i * samplingLastRow) + (samplingLastRow / 2);
        sample->set_resources(vars[1]);
        sample->set_newRspTime(impact.computeOutput(vars));

        dbManager->add_sample(*sample);
    }

    vars[1] = samplesPerRow * samplingIntervalY;
    sample->set_resources(vars[1]);
    for (int i = 0; i < restRow; i++) {
        vars[0] = minRanges[0] + (i * samplingLastCol) + (samplingLastCol / 2);
        sample->set_rspTime(vars[0]);
        sample->set_newRspTime(impact.computeOutput(vars));

        dbManager->add_sample(*sample);
    }
    delete(sample);
}


void generate_synthetic (ImpactFunction impact, DBManager *dbManager) {
    int impactId;

    impactId = dbManager->add_impact_function(impact);

    generate_samples(impact, impactId, dbManager);
}