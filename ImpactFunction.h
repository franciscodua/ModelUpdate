//
// Created by Francisco Duarte on 27/01/16.
//

#ifndef MODELUPDATE_IMPACTFUNCTION_H
#define MODELUPDATE_IMPACTFUNCTION_H


#include <vector>

class ImpactFunction {
/*
 * Class represents impact function.
 * It has the function's properties and computes the output based on the input
 */
private:
    float _percentage;
    int _nDimensions;
    std::vector<float> _weights;
    std::vector<float> _minRange;
    std::vector<float> _maxRange;

public:
    ImpactFunction(float percentage, int nDimensions, std::vector<float> weights,
                   std::vector<float> minRange, std::vector<float> maxRange);

    float getPercentage() const {
        return _percentage;
    }

    int getNDimensions() const {
        return _nDimensions;
    }

    float getWeight(int dimension) {
        return this->_weights.at(dimension);
    }

    float getMinRange(int dimension) {
        return this->_minRange.at(dimension);
    }

    float getMaxRange(int dimension) {
        return this->_maxRange.at(dimension);
    }

    float computeOutput(std::vector<float> variables);
};


#endif //MODELUPDATE_IMPACTFUNCTION_H
