//
// Created by Myeonghoon Sun on 19/5/2022.
//

#ifndef ASSIGNMENT3_AIMAP_H
#define ASSIGNMENT3_AIMAP_H
/*
 * Two maps are used to make the best decision from a given tile
 * or given continuous tiles without a gap.
 *
 * Each of the tiles placeable is searched by its letter in both
 * forwardAiMap and backwardAiMap, by which one with the highest value
 * (the probability of a letter among valid letters * the score of a letter)
 * can be found.
 *
 */

#include <map>
#include <unordered_map>
#include <vector>
#include <string>
#include <numeric>
#include "types.h"
#include <fstream>
#include <iostream>
#include <algorithm>
typedef std::unordered_map<char, float> letterToScoreMapType;
typedef std::shared_ptr<letterToScoreMapType> letterToScorePtr;
typedef std::unordered_map<std::string, letterToScorePtr> aiMapType;
typedef std::shared_ptr<aiMapType> aiMapPtr;


class AiMap {
public:
    AiMap();
    aiMapPtr aiMap;
};


#endif //ASSIGNMENT3_AIMAP_H
