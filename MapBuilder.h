//
// Created by Myeonghoon Sun on 20/5/2022.
//

#ifndef ASSIGNMENT3_MAPBUILDER_H
#define ASSIGNMENT3_MAPBUILDER_H

#include <map>
#include <unordered_map>
#include <vector>
#include <string>
#include <numeric>
#include "types.h"
#include <fstream>
#include <iostream>
#include <algorithm>



class MapBuilder {
public:
    virtual void convertToMap() = 0;
    virtual void saveMap() = 0;
};


#endif //ASSIGNMENT3_MAPBUILDER_H
