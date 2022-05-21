//
// Created by Myeonghoon Sun on 20/5/2022.
//

#include <chrono>

#include "WordBuilder.h"
WordBuilder::WordBuilder(const std::string &forwardSearchMapFileName,
                         const std::string &backwardSearchMapFileName,
                         bool canGiveHints): canGiveHints(canGiveHints), isPlaying(false)
//                         :Player() {
{
    std::cout << "Setting Up AI (it takes an average of 10 seconds)" << std::endl;
    initialiseMaps();
    std::cout << "Set-up Completed" << std::endl;
}

WordBuilder::WordBuilder(const std::string &forwardSearchMapFileName,
                         const std::string &backwardSearchMapFileName,
                         const std::string &name,
                         bool canGiveHints): Player(name), canGiveHints(canGiveHints), isPlaying(true) {
//    std::cout << "Setting Up AI (it takes an average of 10 seconds)" << std::endl;
//    auto start = std::chrono::high_resolution_clock::now();
    initialiseMaps();
//    auto stop = std::chrono::high_resolution_clock::now();
//    std::cout << "Set-up Completed" << std::endl;
//    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop -start);
//    std::cout << duration.count() / 1000000 << " seconds taken" << std::endl;
}

void WordBuilder::readFileToMap(const std::string &fileName, const aiMapPtr& mainMap) {

    std::string line;
    std::string mainKey;
    char subKey;
    float score;

    std::ifstream in(fileName);
    // Read a file as long as there is a line to read
    while (std::getline(in, line)) {
        // Use istringstream to work with individual words on a line
        std::istringstream weightedProbabilities(line);
        weightedProbabilities >> mainKey;
        letterToScorePtr subMap = std::make_shared<letterToScoreMapType>();
        // Build a subMap first
        while (weightedProbabilities >> subKey >> score) {
            subMap->insert(std::make_pair(subKey, score));
        }
        // Build a mainMap
        mainMap->insert(std::make_pair(mainKey, subMap));
    }
//    std::cout << (*(*mainMap)["A"])['C'] << std::endl;

    in.close();
}

void WordBuilder::initialiseMaps() {
    forwardMap = std::make_shared<aiMapType>();
    forwardMap->reserve(NUM_ELEMENTS_IN_AI_MAP);
    readFileToMap("forwardAiMap", forwardMap);
    backwardMap = std::make_shared<aiMapType>();
    backwardMap->reserve(NUM_ELEMENTS_IN_AI_MAP);
    readFileToMap("backwardAiMap", backwardMap);
}