//
// Created by Myeonghoon Sun on 22/5/2022.
//



#include "SortedMapBuilder.h"

SortedMapBuilder::SortedMapBuilder(const std::string& inFile,
                                   const std::string& outFile):
                                   sortedWordMap(std::make_shared<sortedMapType>()),
                                   inFile(inFile),
                                   outFile(outFile){
}

void SortedMapBuilder::build() {
    convertToMap();
    saveMap();
}

// Sort the letters of each wordBeingBuilt in the dictionary
// Use each of the sorted wordsInQueue as a key mapped to its original value
// (e.g. AELPP is a key mapped to APPLE)
void SortedMapBuilder::convertToMap() {
    std::ifstream in(inFile);
    // Read in wordsInQueue from the file
    std::string word;
    while (std::getline(in, word)) {
        std::string sortedString(word);
        std::sort(sortedString.begin(), sortedString.end());
        // If sortedString doesn't exist,
        if (sortedWordMap->count(sortedString) == 0) {
            originalWordsPtr originalStrings;
            // Create a set that will contain original strings as a value
            originalStrings->insert(word);
            // Map the sorted string to the set
            sortedWordMap->insert(std::make_pair(sortedString, originalStrings));
        } else {
            (*sortedWordMap)[sortedString]->insert(word);
        }
    }
    in.close();
}

void SortedMapBuilder::saveMap() {
    std::ofstream out;
    out.open(outFile);

    for (auto beg = sortedWordMap->begin(), end = sortedWordMap->end(); beg != end; ++beg) {
        out << beg->first << " ";
        for (auto el: *(beg->second)) {
            out << el << " ";
        }
        out << std::endl;
    }
    out.close();
}

