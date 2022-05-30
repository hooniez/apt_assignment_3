//
// Created by Myeonghoon Sun on 21/5/2022.
//

#include "Dictionary.h"
Dictionary::Dictionary(const std::string& dictFileName):
dict(std::make_shared<dictType>()) {
    // Make sure dict can hold the total number of words in the provided file
    // so that rehash does not slow down the loading process
    dict->reserve(TOTAL_WORDS_IN_DIC);

    // Read completeWords in the file into dict
    std::string word;
    std::ifstream in(dictFileName);
    while (std::getline(in, word)) {
        dict->insert(word);
    }
    in.close();
}

bool Dictionary::isInDict(std::vector<std::string>& words) {
    bool isAllInDict = false;
    for (auto it = words.cbegin();
         it != words.cend(); ++it) {
        if (dict->count(*it))
            isAllInDict = true;
        else
            isAllInDict = false;
    }
    return isAllInDict;
}

bool Dictionary::isInDict(std::string& word) {
    bool isInDict = false;
    if (dict->count(word))
        isInDict = true;
    else
        isInDict = false;

    return isInDict;
}