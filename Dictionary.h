//
// Created by Myeonghoon Sun on 21/5/2022.
//

#include <unordered_set>
#include <memory>
#include "types.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#ifndef ASSIGNMENT3_DICTIONARY_H
#define ASSIGNMENT3_DICTIONARY_H

/*
 * Dictionary is initialised when --dictionary, --ai, or --hint option is
 * enabled. Its contents are stored in a set since the dictionary is going to
 * be used only for membership checking; the time complexity of search using
 * set is O(1).
 *
 * The unordered_set allows for specifying the number of buckets to store
 * words, thereby reducing the rehashing.
 */

typedef std::unordered_set<std::string> dictType;
typedef std::shared_ptr<dictType> dictPtr;

class Dictionary {
public:
    Dictionary(const std::string& dictFileName);
    bool isInDict(std::vector<std::string>& words);
    bool isInDict(std::string& word);
private:
    dictPtr dict;

};

typedef std::shared_ptr<Dictionary> DictionaryPtr;

#endif //ASSIGNMENT3_DICTIONARY_H
