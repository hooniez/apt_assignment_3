//
// Created by Myeonghoon Sun on 20/5/2022.
//

#include "AiMapBuilder.h"

AiMapBuilder::AiMapBuilder(): forwardAiMap(std::make_shared<aiMapBuildType>()),
                              backwardAiMap(std::make_shared<aiMapBuildType>()) {
    readWords("words");
    buildAiMaps();
    saveAiMapToFile("forwardAiMap", forwardAiMap);
    saveAiMapToFile("backwardAiMap", backwardAiMap);
}

// Read words in and store them as they are in forwardWords and backwards in backwardWords
void AiMapBuilder::readWords(const std::string& fileName) {
    std::ifstream in(fileName);
    // Read in words from the file
    std::string word;
    while (std::getline(in, word)) {
        forwardWords.push_back(word);
        std::reverse(word.begin(), word.end());
        backWardWords.push_back(word);
    }
    in.close();
}

// Build a map to be used to find the best letter after a certain letter(s)
void AiMapBuilder::buildForwardAiMap() {
    forwardAiMap = mapLettersToFrequencies(forwardWords);
    backwardAiMap = mapLettersToFrequencies(backWardWords);
}

// Build a map to be used to find the best letter before a certain letter(s)
void AiMapBuilder::buildBackwardAiMap() {
    forwardAiMap = convertFrequenciesToProbabilities(forwardAiMap);
    backwardAiMap = convertFrequenciesToProbabilities(backwardAiMap);
}

void AiMapBuilder::buildAiMaps() {
    buildForwardAiMap();
    buildBackwardAiMap();
}

aiMapBuildPtr AiMapBuilder::mapLettersToFrequencies(std::vector<std::string>& words) {
    aiMapBuildPtr aiMap = std::make_shared<aiMapBuildType>();
    for (const auto& word : words) {
        // numLetters letters to process in the inner loop
        for (size_t numLetters = 1; numLetters < word.size() + 1; ++numLetters) {
            // Step to the next letter(s)
            for (size_t idx = 0; idx < word.size(); ++idx) {
                size_t lastIdxToProcess = word.size() - numLetters;
                std::string mainKey;
                char subKey;
                if (idx <= lastIdxToProcess) {
                    // If the current letter(s) is not in aiMap as a key
                    mainKey = word.substr(idx, numLetters);

                    // If there is the next letter(s) to process
                    if (idx < lastIdxToProcess) {
                        subKey = word[idx + numLetters];
                    } else if (idx == lastIdxToProcess) { // If there is no next letter(s) to process
                        subKey = '|';
                    }

                    // If the next letter (subKey) is not associated with mainKey
                    if ((*aiMap)[mainKey].count(subKey) == 0) {
                        // Associate the next letter (subKey) with mainKey and give it the value 1
                        (*aiMap)[mainKey][subKey] = 1;
                    } else {
                        // Increment the value associated with subKey by 1
                        (*aiMap)[mainKey][subKey] += 1;
                    }
                }
            }
        }
    }
    return aiMap;
}

aiMapBuildPtr AiMapBuilder::convertFrequenciesToProbabilities(aiMapBuildPtr aiMap) {
    aiMapBuildPtr convertedAiMap = std::make_shared<aiMapBuildType>();
    for (auto beg = aiMap->cbegin(), end = aiMap->cend(); beg != end; ++beg) {
        std::string mainKey = beg->first;
        auto subBeg = beg->second.cbegin();
        auto subEnd = beg->second.cend();

        // Sum the number of frequencies of each letter associated with mainKey
        float total_frequencies = std::accumulate(subBeg, subEnd, 0,
                                                  [](const float prev, auto el)
                                                  {return prev + el.second;});

        // Iterate letters found in the inner map and calculate the probability of its frequencies
        while (subBeg != subEnd) {
            char subKey = subBeg->first;
            float frequency = subBeg->second;
            (*convertedAiMap)[mainKey][subKey] = letterScoreMap.at(subKey) * (frequency / total_frequencies);
            ++subBeg;
        }
    }
    return convertedAiMap;
}

void AiMapBuilder::saveAiMapToFile(const std::string& fileName, aiMapBuildPtr aiMap) {
    std::ofstream out;
    out.open(fileName);

    for (auto beg = aiMap->begin(), end = aiMap->end(); beg != end; ++beg) {
        out << beg->first << " ";
        for (auto el: beg->second) {
            out << el.first << " " << el.second << " ";
        }
        out << std::endl;
    }
    out.close();
}