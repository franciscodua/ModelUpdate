//
// Created by Francisco Duarte on 29/01/16.
//

#include "Sample.h"

std::vector<float> Sample::getVariables() {
    return std::vector<float>(getRspTime(), getResources());
}
