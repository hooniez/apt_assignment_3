//
// Created by Myeonghoon Sun on 20/5/2022.
//

#include "WordBuilder.h"
WordBuilder::WordBuilder(const std::string& dictFileName): dict(std::make_shared<dictType>()) {
    // Make sure dict can hold the total number of words in the provided file
    // so that no rehash slows down the loading process
    dict->reserve(TOTAL_WORDS_IN_DIC);

    // Read words in the file into dict
    std::string word;
    std::ifstream in(dictFileName);
    while (std::getline(in, word)) {
        dict->insert(word);
    }
    in.close();
}

bool WordBuilder::isInDict(std::vector<std::string>& words) {
    bool isAllInDict = true;
    for (auto it = words.cbegin();
         it != words.cend() && isAllInDict; ++it) {
        if (dict->count(*it) == 0)
            isAllInDict = false;
    }
    return isAllInDict;
}