//
// Created by Francisco Duarte on 27/01/16.
//

#include "ImpactFunction.h"

float *copyArray(float *array, int size) {
    float *res = new float[size];

    for (int i = 0; i < size; i++) {
        res[i] = array[i];
    }
    return res;
}

ImpactFunction::ImpactFunction(float percentage, int nDimensions, std::vector<float> weights,
                               std::vector<float> minRange, std::vector<float> maxRange) {
    this->_percentage = percentage;
    this->_nDimensions = nDimensions;
    this->_weights = weights;
    this->_minRange = minRange;
    this->_maxRange = maxRange;
}

float ImpactFunction::computeOutput(std::vector<float> variables) {
    float result = 0;
    int size;

    size = (int) variables.size();

    for (int i = 0; i < size; i++) {
        result += variables[i] * getWeight(i);
    }

    return result;
}
