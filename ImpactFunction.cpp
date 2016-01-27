//
// Created by Francisco Duarte on 27/01/16.
//

#include "ImpactFunction.h"

int *copyArray(int *array, int size) {
    int *res = new int[size];

    for (int i = 0; i < size; i++) {
        res[i] = array[i];
    }
    return res;
}

ImpactFunction::ImpactFunction(int percentage, int nDimensions, int *weights, int *minRange, int *maxRange) {
    this->percentage = percentage;
    this->nDimensions = nDimensions;
    this->weights = copyArray(weights, nDimensions - 1);
    this->minRange = copyArray(minRange, nDimensions - 1);
    this->maxRange = copyArray(maxRange, nDimensions - 1);
}

int ImpactFunction::computeOutput(int *variables) {
    int result = 0;
    int size;

    size = sizeof(variables)/sizeof(variables[0]);

    for (int i = 0; i < size; i++) {
        result += variables[i] * getWeight(i);
    }

    return result;
}
