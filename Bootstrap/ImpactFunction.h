//
// Created by Francisco Duarte on 27/01/16.
//

#ifndef MODELUPDATE_IMPACTFUNCTION_H
#define MODELUPDATE_IMPACTFUNCTION_H

#include <cmath>
#include <vector>
#include "Sample.h"

class ImpactFunction {
/*
 * Class represents impact function.
 * It has the function's properties and computes the output based on the input
 */
private:
    int _impactId;
    float _probability;
    int _nDimensions;
    float _intersection;
    std::vector<float> _weights;
    std::vector<float> _minRange;
    std::vector<float> _maxRange;

public:

    ImpactFunction(float _probability, int _nDimensions, float _intersection, const std::vector<float> _weights,
                   const std::vector<float> _minRange, const std::vector<float> _maxRange) : _probability(
            _probability), _nDimensions(_nDimensions), _intersection(_intersection), _weights(_weights),
                                                                                               _minRange(_minRange),
                                                                                               _maxRange(_maxRange) { }


    ImpactFunction(int _impactId, float _probability, int _nDimensions, float _intersection,
                   const std::vector<float> _weights, const std::vector<float> _minRange,
                   const std::vector<float> _maxRange) : _impactId(_impactId), _probability(_probability),
                                                          _nDimensions(_nDimensions), _intersection(_intersection),
                                                          _weights(_weights), _minRange(_minRange),
                                                          _maxRange(_maxRange) { }
    



    int getImpactId() const {
        return _impactId;
    }

    float getProbability() const {
        return _probability;
    }

    int getNDimensions() const {
        return _nDimensions;
    }

    float getIntersection() const {
        return _intersection;
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

    /*
     * Computes expected outcome of sample based on impact function.
     * Receives a vector of variables values.
     * Returns w1*v1 + w2*v2 + ... + wn*vn
     */
    float computeOutput(std::vector<float> variables);

    /*
     * Computes prediction error based on sample
     * returns | expected - real |
     */
    float computeError(Sample sample);
};


#endif //MODELUPDATE_IMPACTFUNCTION_H
