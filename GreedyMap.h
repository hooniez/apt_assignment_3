//
// Created by Myeonghoon Sun on 29/5/2022.
//

#ifndef ASSIGNMENT3_GREEDYMAP_H
#define ASSIGNMENT3_GREEDYMAP_H

#include <unordered_map>
#include <map>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "types.h"

typedef std::map<char, size_t> letterToScoreMapType;
typedef std::shared_ptr<letterToScoreMapType> letterToScorePtr;

typedef std::unordered_map<std::string, letterToScorePtr> greedyMapType;
typedef std::shared_ptr<greedyMapType> mapPtr;

class GreedyMap {
public:
    GreedyMap();
    mapPtr forward;
    mapPtr backward;

    void readFileToMap(const std::string&, mapPtr&);
};

typedef std::shared_ptr<GreedyMap> GreedyMapPtr;


#endif //ASSIGNMENT3_GREEDYMAP_H
