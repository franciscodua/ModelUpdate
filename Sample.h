//
// Created by Francisco Duarte on 29/01/16.
//

#ifndef MODELUPDATE_SAMPLE_H
#define MODELUPDATE_SAMPLE_H


class Sample {
private:
    float _rspTime;
    float _resources;
    float _newRspTime;
    int _impactId;

public:

    Sample(float _rspTime, float _resources, float _newRspTime, int _impactId) : _rspTime(_rspTime),
                                                                                 _resources(_resources),
                                                                                 _newRspTime(_newRspTime),
                                                                                 _impactId(_impactId) { }


    float get_rspTime() const {
        return _rspTime;
    }

    float get_resources() const {
        return _resources;
    }

    float get_newRspTime() const {
        return _newRspTime;
    }

    int get_impactId() const {
        return _impactId;
    }
};


#endif //MODELUPDATE_SAMPLE_H
