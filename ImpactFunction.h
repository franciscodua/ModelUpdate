//
// Created by Francisco Duarte on 27/01/16.
//

#ifndef MODELUPDATE_IMPACTFUNCTION_H
#define MODELUPDATE_IMPACTFUNCTION_H


class ImpactFunction {
private:
    float _percentage;
    int _nDimensions;
    float *_weights;
    float *_minRange;
    float *_maxRange;

public:
    ImpactFunction(float percentage, int nDimensions, float *weights, float *minRange, float *maxRange);

    float getPercentage() const {
        return _percentage;
    }

    int getNDimensions() const {
        return _nDimensions;
    }

    float *getWeights() const {
        return _weights;
    }

    float getWeight(int dimension) {
        return this->_weights[dimension];
    }

    float getMinRange(int dimension) {
        return this->_minRange[dimension];
    }

    float getMaxRange(int dimension) {
        return this->_maxRange[dimension];
    }

    float computeOutput(float *variables);
};


#endif //MODELUPDATE_IMPACTFUNCTION_H
