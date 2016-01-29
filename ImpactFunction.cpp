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
