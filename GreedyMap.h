//
// Created by Myeonghoon Sun on 29/5/2022.
//

#ifndef ASSIGNMENT3_GREEDYMAP_H
#define ASSIGNMENT3_GREEDYMAP_H

/*
 * GreedyMap was initially preprocessed from the provided dictionary such
 * that each
 */

#include <unordered_map>
#include <map>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "types.h"

typedef std::set<char> extendableLettersType;
typedef std::shared_ptr<extendableLettersType> extendableLettersPtr;

typedef std::unordered_map<std::string, extendableLettersPtr> greedyMapType;
typedef std::shared_ptr<greedyMapType> mapPtr;

class GreedyMap {
public:
    GreedyMap();
    mapPtr forward;
    mapPtr backward;

    static void readFileToMap(const std::string&, mapPtr&);
};

typedef std::shared_ptr<GreedyMap> GreedyMapPtr;


#endif //ASSIGNMENT3_GREEDYMAP_H
