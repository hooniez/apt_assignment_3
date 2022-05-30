//
// Created by Myeonghoon Sun on 29/5/2022.
//

#include "GreedyMap.h"

GreedyMap::GreedyMap() {
    std::cout <<
    "Setting Up (it takes an average of 10 seconds)" << std::endl;
    forward = std::make_shared<greedyMapType>();
    forward->reserve(NUM_ELEMENTS_IN_GREEDY_MAP);
    readFileToMap("forwardMap", forward);

    backward = std::make_shared<greedyMapType>();
    backward->reserve(NUM_ELEMENTS_IN_GREEDY_MAP);
    readFileToMap("backwardMap", backward);
}

void GreedyMap::readFileToMap(const std::string &fileName, mapPtr& map) {
    std::string line;
    std::string key;
    char letter;

    std::ifstream in(fileName);
    // Read a file as long as there is a line to read
    while (std::getline(in, line)) {
        // Use istringstream to work with individual completeWords on a line
        std::istringstream keyLetterCombo(line);
        keyLetterCombo >> key;
        extendableLettersPtr letters = std::make_shared<extendableLettersType>();
        // Build a letters first
        while (keyLetterCombo >> letter) {
            letters->insert(letter);
        }
        // Build a map
        map->insert(std::make_pair(key, letters));
    }

    in.close();
}


// The code below is a previous implementation. Please disregard it.

// To measure time it takes to construct maps, use the code below.
// #include <chrono>
//    auto start = std::chrono::high_resolution_clock::now();
//    auto stop = std::chrono::high_resolution_clock::now();
//    std::cout << "Set-up Completed" << std::endl;
//    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop -start);
//    std::cout << duration.count() / 1000000 << " seconds taken" << std::endl;

//void GreedyMap::readFileToMap(const std::string &fileName, mapPtr& mainMap) {
//    std::string line;
//    std::string mainKey;
//    char subKey;
//    float score;
//
//    std::ifstream in(fileName);
//    // Read a file as long as there is a line to read
//    while (std::getline(in, line)) {
//        // Use istringstream to work with individual completeWords on a line
//        std::istringstream weightedProbabilities(line);
//        weightedProbabilities >> mainKey;
//        extendableLettersPtr subMap = std::make_shared<extendableLettersType>();
//        // Build a subMap first
//        while (weightedProbabilities >> subKey >> score) {
//            subMap->insert(std::make_pair(subKey, score));
//        }
//        // Build a mainMap
//        mainMap->insert(std::make_pair(mainKey, subMap));
//    }
//
//    in.close();
//}