//
// Created by Myeonghoon Sun on 22/5/2022.
//

#include "GreedyMapBuilder.h"

GreedyMapBuilder::GreedyMapBuilder(bool forward,
                                   const std::string& inFile,
                                   const std::string& outFile):
                                   greedyMap(),
                                   forward(forward),
                                   inFile(inFile),
                                   outFile(outFile) {}

void GreedyMapBuilder::build() {
    convertToMap();
    saveMap();
}

void GreedyMapBuilder::convertToMap() {
    // Read in wordsInQueue from the file
    std::ifstream in(inFile);
    std::string word;
    while (std::getline(in, word)) {
        // If backward, process word backwards
        if (!forward)
            std::reverse(word.begin(), word.end());
        // Execute the process described in Step 1 in the header file
        // numLetters letters to process in the inner loop
        for (size_t numLetters = 1; numLetters < word.size() + 1; ++numLetters) {
            // Step to the next letter(s)
            for (size_t idx = 0; idx < word.size(); ++idx) {
                size_t lastIdxToProcess = word.size() - numLetters;
                std::string mainKey;
                char subKey;
                if (idx <= lastIdxToProcess) {
                    // If the current letter(s) is not in sortedWordMap as a key
                    mainKey = word.substr(idx, numLetters);
                    if (!forward) {
                        std::reverse(mainKey.begin(), mainKey.end());
                    }

                    // If there is the next letter(s) to process
                    if (idx < lastIdxToProcess) {
                        subKey = word[idx + numLetters];
                    } else if (idx == lastIdxToProcess) { // If there is no next letter(s) to process
                        subKey = '|';
                    }

//                    // If greedyMap does not contain mainKey already
//                    if (!greedyMap->count(mainKey)) {
//                        (*greedyMap)[mainKey];
//                    }

                    // If the next letter (subKey) is not associated with mainKey
                    if (greedyMap[mainKey].count(subKey) == 0) {
                        // Associate the next letter (subKey) with mainKey and give it the value 1
                        greedyMap[mainKey][subKey] = 1;
                    } else {
                        // Increment the value associated with subKey by 1
                        greedyMap[mainKey][subKey] += 1;
                    }
                }
            }
        }
    }

//    // Execute the process described in Step 2 in the header file
//    for (auto beg = greedyMap->cbegin(), end = greedyMap->cend(); beg != end; ++beg) {
//        std::string mainKey = beg->first;
//        auto subBeg = beg->second->cbegin();
//        auto subEnd = beg->second->cend();
//
//        // Sum the number of frequencies of each letter associated with mainKey
//        float total_frequencies = std::accumulate(subBeg, subEnd, 0,
//                                                  [](const float prev, auto el)
//                                                  {return prev + el.second;});
//
//        // Iterate letters found in the inner map and calculate the probability of its frequencies
//        while (subBeg != subEnd) {
//            char subKey = subBeg->first;
//            float frequency = subBeg->second;
//            (*(*greedyMap)[mainKey])[subKey] = letterScoreMap.at(subKey) * (frequency / total_frequencies);
//            ++subBeg;
//        }
//    }
    in.close();
}

void GreedyMapBuilder::saveMap() {
    std::ofstream out;
    out.open(outFile);

    for (auto beg = greedyMap.begin(), end = greedyMap.end(); beg != end; ++beg) {
        out << beg->first << " ";
        for (auto el: beg->second) {
            out << el.first << " " << el.second << " ";
        }
        out << std::endl;
    }
    out.close();
}
