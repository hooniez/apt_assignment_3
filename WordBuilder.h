//
// Created by Myeonghoon Sun on 20/5/2022.
//

#ifndef ASSIGNMENT3_WORDBUILDER_H
#define ASSIGNMENT3_WORDBUILDER_H

#include <unordered_set>
#include <memory>
#include "types.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

/*
 * WordBuilder
 */
typedef std::unordered_set<std::string> dictType;
typedef std::shared_ptr<dictType> dictPtr;

class WordBuilder {
public:
    WordBuilder(const std::string& dictFileName);
    bool isInDict(std::vector<std::string>& words);
private:
    dictPtr dict;
};
typedef std::shared_ptr<WordBuilder> WordBuilderPtr;

#endif //ASSIGNMENT3_WORDBUILDER_H
