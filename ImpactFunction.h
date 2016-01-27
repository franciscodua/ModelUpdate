//
// Created by Francisco Duarte on 27/01/16.
//

#ifndef MODELUPDATE_IMPACTFUNCTION_H
#define MODELUPDATE_IMPACTFUNCTION_H


class ImpactFunction {
private:
    int percentage;
    int nDimensions;
    int *weights;
    int *minRange;
    int *maxRange;

public:
    ImpactFunction(int percentage, int nDimensions, int *weights, int *minRange, int *maxRange);

    int getPercentage() const {
        return percentage;
    }

    int getNDimensions() const {
        return nDimensions;
    }

    int *getWeights() const {
        return weights;
    }

    int getWeight(int dimension) {
        return this->weights[dimension];
    }

    int getMinRange(int dimension) {
        return this->minRange[dimension];
    }

    int getMaxRange(int dimension) {
        return this->maxRange[dimension];
    }

    int computeOutput(int *variables);
};


#endif //MODELUPDATE_IMPACTFUNCTION_H
