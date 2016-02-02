//
// Created by Francisco Duarte on 29/01/16.
//

#ifndef MODELUPDATE_SAMPLE_H
#define MODELUPDATE_SAMPLE_H


#include <vector>

class Sample {
    /*
     * Represents samples details. Container.
     */
private:
    int _sampleId;
    float _rspTime;
    float _resources;
    float _newRspTime;
    int _impactId;
    bool _synthetic;

public:

    Sample(float _rspTime, float _resources, float _newRspTime, int _impactId, bool synthetic) : _rspTime(_rspTime),
                                                                                 _resources(_resources),
                                                                                 _newRspTime(_newRspTime),
                                                                                 _impactId(_impactId),
                                                                                 _synthetic(synthetic){ }


    Sample(int _sampleId, float _rspTime, float _resources, float _newRspTime, int _impactId, bool synthetic) :
                                                                                                _sampleId(_sampleId),
                                                                                                _rspTime(_rspTime),
                                                                                                _resources(_resources),
                                                                                                _newRspTime(
                                                                                                        _newRspTime),
                                                                                                _impactId(_impactId),
                                                                                                _synthetic(synthetic){ }

    Sample(float _rspTime, float _resources, float _newRspTime, bool synthetic) : _rspTime(_rspTime),
                                                                                  _resources(_resources),
                                                                                  _newRspTime(_newRspTime),
                                                                                  _synthetic(synthetic){ }


    int getSampleId() const {
        return _sampleId;
    }

    float getRspTime() const {
        return _rspTime;
    }

    float getResources() const {
        return _resources;
    }

    float getNewRspTime() const {
        return _newRspTime;
    }

    int getImpactId() const {
        return _impactId;
    }

    bool getSynthetic() const {
        return _synthetic;
    }

    void set_impactId(int _impactId) {
        Sample::_impactId = _impactId;
    }

    void setRspTime(float _rspTime) {
        Sample::_rspTime = _rspTime;
    }

    void setResources(float _resources) {
        Sample::_resources = _resources;
    }

    void setNewRspTime(float _newRspTime) {
        Sample::_newRspTime = _newRspTime;
    }

    std::vector<float> getVariables();

};


#endif //MODELUPDATE_SAMPLE_H
