//
// Created by Myeonghoon Sun on 29/5/2022.
//

#include "GreedyMap.h"

GreedyMap::GreedyMap() {
    std::cout << "Setting Up AI to play against or give hints (it takes an average of 10 seconds)" << std::endl;
    forward = std::make_shared<greedyMapType>();
    forward->reserve(NUM_ELEMENTS_IN_GREEDY_MAP);
    readFileToMap("forwardMap", forward);

    backward = std::make_shared<greedyMapType>();
    backward->reserve(NUM_ELEMENTS_IN_GREEDY_MAP);
    readFileToMap("backWardMap", backward);
}

void GreedyMap::readFileToMap(const std::string &fileName, mapPtr& mainMap) {
    std::string line;
    std::string mainKey;
    char subKey;
    float score;

    std::ifstream in(fileName);
    // Read a file as long as there is a line to read
    while (std::getline(in, line)) {
        // Use istringstream to work with individual wordsInQueue on a line
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



// To measure time it takes to construct maps, use the code below.
// #include <chrono>
//    auto start = std::chrono::high_resolution_clock::now();
//    auto stop = std::chrono::high_resolution_clock::now();
//    std::cout << "Set-up Completed" << std::endl;
//    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop -start);
//    std::cout << duration.count() / 1000000 << " seconds taken" << std::endl;
