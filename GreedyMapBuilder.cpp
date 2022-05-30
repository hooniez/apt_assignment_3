//
// Created by Myeonghoon Sun on 22/5/2022.
//

#include "GreedyMapBuilder.h"

#include <utility>

GreedyMapBuilder::GreedyMapBuilder(bool forward,
                                   std::string  inFile,
                                   std::string  outFile):
                                   greedyMap(),
                                   forward(forward),
                                   inFile(std::move(inFile)),
                                   outFile(std::move(outFile)) {}

void GreedyMapBuilder::build() {
    convertToMap();
    saveMap();
}

void GreedyMapBuilder::convertToMap() {
    // Read in wordsInQueue from the file
    std::ifstream in(inFile);
    std::string word;
    while (std::getline(in, word)) {
        // If backward, process wordBeingBuilt backwards
        if (!forward)
            std::reverse(word.begin(), word.end());
        // Execute the process described in Step 1 in the header file
        // numLetters letters to process in the inner loop
        for (size_t numLetters = 1; numLetters < word.size() + 1; ++numLetters)
        {
            // Step to the next letter(s)
            for (size_t idx = 0; idx < word.size(); ++idx) {
                size_t lastIdxToProcess = word.size() - numLetters;
                std::string mainKey;
                char letter;
                if (idx <= lastIdxToProcess) {
                    // If the current letter(s) is not in sortedWordMap as a key
                    mainKey = word.substr(idx, numLetters);
                    if (!forward) {
                        std::reverse(mainKey.begin(), mainKey.end());
                    }

                    // If there is the next letter(s) to process
                    if (idx < lastIdxToProcess) {
                        letter = word[idx + numLetters];
                    } else if (idx == lastIdxToProcess) {
                        // If there is no next letter(s) to process
                        letter = '|';
                    }

                    // If the next letter is not associated with mainKey
                    if (greedyMap[mainKey].count(letter) == 0)
                        // Associate the next letter (letter) with mainKey
                        greedyMap[mainKey].insert(letter);
                }
            }
        }
    }

    in.close();
}

void GreedyMapBuilder::saveMap() {
    std::ofstream out;
    out.open(outFile);

    for (auto & beg : greedyMap) {
        out << beg.first << " ";
        for (auto el: beg.second) {
            out << el << " ";
        }
        out << std::endl;
    }
    out.close();
}

/*
 * The code below is the previous implementation. Please disregard it.
 */

//void GreedyMapBuilder::convertToMap() {
//    // Read in wordsInQueue from the file
//    std::ifstream in(inFile);
//    std::string word;
//    while (std::getline(in, word)) {
//        // If backward, process wordBeingBuilt backwards
//        if (!forward)
//            std::reverse(word.begin(), word.end());
//        // Execute the process described in Step 1 in the header file
//        // numLetters letters to process in the inner loop
//        for (size_t numLetters = 1; numLetters < word.size() + 1; ++numLetters) {
//            // Step to the next letter(s)
//            for (size_t idx = 0; idx < word.size(); ++idx) {
//                size_t lastIdxToProcess = word.size() - numLetters;
//                std::string mainKey;
//                char subKey;
//                if (idx <= lastIdxToProcess) {
//                    // If the current letter(s) is not in sortedWordMap as a key
//                    mainKey = word.substr(idx, numLetters);
//                    if (!forward) {
//                        std::reverse(mainKey.begin(), mainKey.end());
//                    }
//
//                    // If there is the next letter(s) to process
//                    if (idx < lastIdxToProcess) {
//                        subKey = word[idx + numLetters];
//                    } else if (idx == lastIdxToProcess) { // If there is no next letter(s) to process
//                        subKey = '|';
//                    }
//
//                    // If the next letter (subKey) is not associated with mainKey
//                    if (greedyMap[mainKey].count(subKey) == 0) {
//                        // Associate the next letter (subKey) with mainKey and give it the value 1
//                        greedyMap[mainKey][subKey] = 1;
//                    } else {
//                        // Increment the value associated with subKey by 1
//                        greedyMap[mainKey][subKey] += 1;
//                    }
//                }
//            }
//        }
//    }

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
//    in.close();
//}
