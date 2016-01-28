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

ImpactFunction::ImpactFunction(float percentage, int nDimensions, float *weights, float *minRange, float *maxRange) {
    this->_percentage = percentage;
    this->_nDimensions = nDimensions;
    this->_weights = copyArray(weights, nDimensions - 1);
    this->_minRange = copyArray(minRange, nDimensions - 1);
    this->_maxRange = copyArray(maxRange, nDimensions - 1);
}

float ImpactFunction::computeOutput(float *variables) {
    float result = 0;
    int size;

    size = sizeof(variables)/sizeof(variables[0]);

    for (int i = 0; i < size; i++) {
        result += variables[i] * getWeight(i);
    }

    return result;
}
