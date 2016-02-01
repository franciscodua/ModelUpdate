//
// Created by Francisco Duarte on 27/01/16.
//

#include "ImpactFunction.h"

float ImpactFunction::computeOutput(std::vector<float> variables) {
    float result = 0;
    int size;

    size = (int) variables.size();

    for (int i = 0; i < size; i++) {
        result += variables[i] * getWeight(i);
    }

    return result;
}

float ImpactFunction::computeError(Sample sample) {
    float expectedOutcome;
    float realOutcome;
    std::vector<float> vars = sample.getVariables();

    expectedOutcome = computeOutput(vars);

    realOutcome = sample.getNewRspTime();

    return std::abs(expectedOutcome - realOutcome);
}
