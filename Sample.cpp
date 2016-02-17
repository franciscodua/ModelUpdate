//
// Created by Francisco Duarte on 29/01/16.
//

#include "Sample.h"

std::vector<float> Sample::getVariables() {
    std::vector<float> variables(2);
    variables[0] = getRspTime();
    variables[1] = getResources();
    return variables;
}