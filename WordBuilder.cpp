//
// Created by Myeonghoon Sun on 20/5/2022.
//

#include <chrono>

#include "WordBuilder.h"
WordBuilder::WordBuilder(const std::string &forwardSearchMapFileName,
                         const std::string &backwardSearchMapFileName,
                         const std::string& sortedMapFileName,
                         const DictionaryPtr& dictionary,
                         BoardPtr board,
                         bool canGiveHints):
        canGiveHints(canGiveHints),
        isPlaying(false),
        board(board),
        dictionary(dictionary),
        wordsInQueue(std::make_shared<std::priority_queue<Word, std::vector<Word>, CompareWord>>()),
        adjacentTiles(BOARD_LENGTH * BOARD_LENGTH, nullptr)
//                         :Player() {
{
    std::cout << "Setting Up AI (it takes an average of 10 seconds)" << std::endl;
    initialiseMaps();
    std::cout << "Set-up Completed" << std::endl;
}

WordBuilder::WordBuilder(const std::string &forwardSearchMapFileName,
                         const std::string &backwardSearchMapFileName,
                         const std::string& sortedMapFileName,
                         const DictionaryPtr& dictionary,
                         const std::string &name,
                         size_t score,
                         LinkedListPtr<TilePtr> hand,
                         BoardPtr board,
                         bool canGiveHints):
        Player(name, score, hand),
        canGiveHints(canGiveHints),
        isPlaying(true),
        board(board),
        dictionary(dictionary),
        wordsInQueue(std::make_shared<std::priority_queue<Word, std::vector<Word>, CompareWord>>()),
        adjacentTiles(BOARD_LENGTH * BOARD_LENGTH, nullptr) {
    std::cout << "Setting Up AI (it takes an average of 10 seconds)" << std::endl;
//    auto start = std::chrono::high_resolution_clock::now();
    initialiseMaps();
//    auto stop = std::chrono::high_resolution_clock::now();
//    std::cout << "Set-up Completed" << std::endl;
//    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop -start);
//    std::cout << duration.count() / 1000000 << " seconds taken" << std::endl;
}

WordBuilder::WordBuilder(const std::string& forwardSearchMapFileName,
            const std::string& backwardSearchMapFileName,
            const std::string& sortedMapFileName,
            const DictionaryPtr& dictionary,
            const std::string& name,
            BoardPtr board,
            bool canGiveHints): Player(name),
                                canGiveHints(canGiveHints),
                                isPlaying(true),
                                board(board),
                                dictionary(dictionary),
                                adjacentTiles(BOARD_LENGTH * BOARD_LENGTH, nullptr) {
    initialiseMaps();
}

void WordBuilder::readFileToMap(const std::string &fileName, greedyMapPtr& mainMap) {

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

void WordBuilder::readFileToMap(const std::string &fileName, sortedMapPtr& mainMap) {

    std::string line;
    std::string mainKey;
    std::string originalWord;

    std::ifstream in(fileName);
    // Read a file as long as there is a line to read
    while (std::getline(in, line)) {
        // Use istringstream to work with individual wordsInQueue on a line
        std::istringstream iss(line);
        iss >> mainKey;
        originalWordsPtr originalWords = std::make_shared<originalWordsType>();
        while (iss >> originalWord) {
            originalWords->insert(originalWord);
        }
        // Build a mainMap
        mainMap->insert(std::make_pair(mainKey, originalWords));
    }
    in.close();
}

void WordBuilder::initialiseMaps() {
    greedyForwardMap = std::make_shared<greedyMapType>();
    greedyForwardMap->reserve(NUM_ELEMENTS_IN_GREEDY_MAP);
    readFileToMap("forwardGreedyMap", greedyForwardMap);
    greedyBackwardMap = std::make_shared<greedyMapType>();
    greedyBackwardMap->reserve(NUM_ELEMENTS_IN_GREEDY_MAP);
    readFileToMap("backwardGreedyMap", greedyBackwardMap);
    sortedMap = std::make_shared<sortedMapType>();
    sortedMap->reserve(NUM_ELEMENTS_IN_SORTED_MAP);
    readFileToMap("sortedMap", sortedMap);
}

std::shared_ptr<std::map<std::string, char>> WordBuilder::getTheBestMove() {
    wordsInQueue = std::make_shared<std::priority_queue<Word, std::vector<Word>, CompareWord>>();
    updateSortedMappableAdjacentTile();
    beAwareOfTiles();
    prioritiseTiles();
    findWords();
    auto chosenTileIndices = wordsInQueue->top().tileIndices;
    // Convert chosenTileIndices into the format Board's pre-existing function understands
    return convert(chosenTileIndices);
}


std::shared_ptr<std::map<std::string, char>> WordBuilder::convert(std::map<int, char>& tileIndices) {
    std::shared_ptr<std::map<std::string, char>> ret = std::make_shared<std::map<std::string, char>>();
    int rowIdx;
    int colIdx;
    std::string gridLoc;
    char letter;
    for (auto tileIndex : tileIndices) {
        rowIdx = getCurrLine(tileIndex.first, HORIZONTAL);
        colIdx = getCurrLine(tileIndex.first, VERTICAL);
        gridLoc = convertIntToRowLetter(rowIdx) + std::to_string(colIdx);
        letter = tileIndex.second;
        ret->insert(std::make_pair(gridLoc, letter));
    }
    return ret;
}

// This method converts a 2D row index back to the alphabetical representation.
// Even though it is an inefficient, roundabout way to convert it only to be converted back again by board,
// it is considered a good solution as the conversion allows for
// the reuse of the pre-existing methods in other classes.
char WordBuilder::convertIntToRowLetter(int rowIdx) {
    char a = 'A';
    int baseInt = int(a);
    return rowIdx + baseInt;
}

void WordBuilder::findWords() {
    // Get all the lettersInHand from the hand
    std::string lettersInHand = getHand()->getLetters();
    std::string updatedLetters;

    while(!tilesToStartFrom.empty()) {
        auto at = tilesToStartFrom.top();
        Angle angles[] = {VERTICAL, HORIZONTAL};

        std::string verticalLetters;
        std::string horizontalLetters;
        std::map<WordPtr, std::tuple<bool, bool>> verticalValidLetters;
        std::map<WordPtr, std::tuple<bool, bool>> horizontalValidLetters;
        std::map<int, char> tileIndices;
        std::vector<std::string> startingWords;
        bool forwardWordExists;
        bool backwardWordExists;

        for (auto letter : lettersInHand) {
            startingWords.clear();
            tileIndices.clear();
            forwardWordExists = false;
            backwardWordExists = false;

            // Process the connected lettersInHand
            verticalLetters = std::string(1, letter);
            horizontalLetters = std::string(1, letter);
            if (!at->adjacentLetters[UPWARD].empty())
                verticalLetters = at->adjacentLetters[UPWARD] + verticalLetters;
            if (!at->adjacentLetters[DOWNWARD].empty())
                verticalLetters += at->adjacentLetters[DOWNWARD];
            if (!at->adjacentLetters[LEFTWARD].empty())
                horizontalLetters = at->adjacentLetters[LEFTWARD] + horizontalLetters;
            if (!at->adjacentLetters[RIGHTWARD].empty())
                horizontalLetters += at->adjacentLetters[RIGHTWARD];

            // Iterate VERTICAL and HORIZONTAL angles to see whether the AdjacentTile can go forwards or backwards or both.
            for (auto angle: angles) {
                if (angle == VERTICAL) {
                    if (dictionary->isInDict(horizontalLetters)) {
                        if ((*greedyForwardMap)[verticalLetters]) {
                            forwardWordExists = true;
                        }
                        if ((*greedyBackwardMap)[horizontalLetters]) {
                            backwardWordExists = true;
                        }
                    }
                } else { // angle == HORIZONTAL
                    if (dictionary->isInDict(verticalLetters)) {
                        if ((*greedyForwardMap)[horizontalLetters]) {
                            forwardWordExists = true;
                        }
                        if ((*greedyBackwardMap)[horizontalLetters]) {
                            backwardWordExists = true;
                        }
                    }
                }

                // If letters can be extended
                if (forwardWordExists || backwardWordExists) {
                    std::string lettersCopy(lettersInHand);
                    auto charPos = std::find(lettersCopy.begin(), lettersCopy.end(), letter);
                    lettersCopy.erase(charPos);
                    tileIndices.insert(std::make_pair(at->idx, letter));

                    std::tuple<bool, bool> wordAvailability = std::make_tuple(0, 0);
                    if (forwardWordExists)
                        std::get<FORWARDWORDEXISTS>(wordAvailability) = true;
                    if (backwardWordExists)
                        std::get<BACKWARDWORDEXISTS>(wordAvailability) = true;

                    if (angle == VERTICAL) {
                        verticalValidLetters.insert(
                                std::make_pair(std::make_shared<Word>(verticalLetters, lettersCopy, tileIndices),
                                               wordAvailability));
                    } else {
                        horizontalValidLetters.insert(
                                std::make_pair(std::make_shared<Word>(horizontalLetters, lettersCopy, tileIndices),
                                               wordAvailability));

                    }

                }
            }


        }






        // buildWord



//        if (at->misSolvableBySortedMap) {
//            // TODO process only one of the two AdjacentTiles for sortedMap (add only one in priority queue)
//            updatedLetters = letters;
//            // If the AdjacentTile is solvable by sortedMap, add the letter to letters
//            for (auto letter : at->adjacentLetters) {
//                updatedLetters += letter;
//            }
//            // Sort the combined letters
//            std::sort(updatedLetters.begin(), updatedLetters.end());
//
//            // Find whether the combined letters exist in sortedMap
//            if (sortedMap->count(updatedLetters)) {
//                std::cout << "Letters Enquired: " << updatedLetters << std::endl;
//                for (auto wordBeingBuilt: *(*sortedMap)[updatedLetters]) {
//                    std::cout << wordBeingBuilt << std::endl;
//                }
//            }
//        } else {
//            // Greedy Algo goes here
//        }




//        // Implement the greedy algorithm
//        // First find out in which direction the given AdjacentTile is connected to other letters
//        std::vector<int> directionsToSearch;
//        for (int idx = UPWARD; idx < TOTALDIRECTIONS; ++idx) {
//            // If the string stored at the index is not empty
//            if (!at->adjacentLetters[idx].empty())
//                directionsToSearch.push_back(idx);
//        }
//
//        // TODO decide on whether letters need to be turned into a set so each unique letter is checked
//        // First, given the AdjacentTile, try all the letters in the hand to see whether adjacent letters can be extended
//        std::string partialWord;
//        greedyMapPtr greedyMap;
//        // the number of combined letters, in a dictionary, with each of the extendible letters mapped to itself
//        std::map<int, char> extendibleLetters;
//
//
//        for (const auto &ch : letters) {
//            bool isExtendible = true;
//            for (const int dir : directionsToSearch) {
//                partialWord = at->adjacentLetters[dir];
//                // Decide on which map to use to look up whether a partial combination of letters exists
//                if (dir == UPWARD || dir == LEFTWARD) {
//                    // If the previous dir has not set isExtendible to false
//                    if (isExtendible) {
//                        greedyMap = greedyForwardMap;
//                        // TODO see if std::string::reserve is necessary
//
//                        // If partialWord key is not found in greedyMap
//                        if (!greedyMap->count(partialWord)) {
//                            // TODO see whether it is necessary to set isExtendible of AdjacentTile to false
//                            isExtendible = false;
//                        } else if (!(*greedyMap)[partialWord]->count(ch)){
//                            // If partialWord key is found but the given letter in the hand is not found associated with it
//                            isExtendible = false;
//                        }
//                    }
//                } else {
//                    // If the previous dir has not set isExtendible to false
//                    if (isExtendible) {
//                        greedyMap = greedyBackwardMap;
//                        // Prepend a letter
//                        // TODO: Check whether partialWord exists first
//                        if (!greedyMap->count(partialWord)) {
//                            isExtendible = false;
//                        } else if (!(*greedyMap)[partialWord]->count(ch)){
//                            // If partialWord key is found but the given letter in the hand is not found associated with it
//                            isExtendible = false;
//                        }
//                    }
//                }
//            }
//
//            // If the letter can extend all the connected wordsInQueue, store it and the number of wordsInQueue that can be made from the extension
//            if (isExtendible) {
//                extendibleLetters.insert(std::make_pair((*(*greedyMap)[partialWord])[ch], ch));
//            }
//        }

//      Find out the most backward index and forward index connected to the AdjacentIdx
//      and then build wordsInQueue
        int currIdx;
        int forwardDir;
        int backwardDir;
        int mostBackwardIdx;
        int mostForwardIdx;
        int currVerticalLine;
        int currHorizontalLine;
        std::string verticalWord;
        std::string horizontalWord;
        std::vector<std::string> tempWords;
        bool isForwardable = false;
        bool isBackwardable = false;

        if (!verticalValidLetters.empty()) {
            currIdx = at->idx;
            currVerticalLine = getCurrLine(currIdx, VERTICAL);
            mostBackwardIdx = currIdx;
            mostForwardIdx = currIdx;
            backwardDir = TOP;
            forwardDir = BOTTOM;

            // currIdx is technically occupied by one of the elements in verticalValidLetters
            // Therefore move backward to find the next empty cell
            mostBackwardIdx = currIdx + backwardDir;
            // As long as on the board and there are already placed tiles
            while (isPlacedTileOnTheSameLine(mostBackwardIdx, currVerticalLine, VERTICAL)) {
                mostBackwardIdx += backwardDir;
            }

            // Find out mostForwardIdx
            mostForwardIdx = currIdx + forwardDir;
            // As long as on the board and there are already placed tiles
            while (isPlacedTileOnTheSameLine(mostForwardIdx, currVerticalLine, VERTICAL)) {
                mostForwardIdx += forwardDir;
            }

//            // If mostBackwardIdx is the same as currIdx, go backwards again (taking into account the letter to be placed)
//            if (mostBackwardIdx == currIdx)
//                mostBackwardIdx += backwardDir;
//            // If mostForwardIdx is the same as currIdx, go forwards again (taking into account the letter to be placed)
//            if (mostForwardIdx == currIdx)
//                mostForwardIdx += forwardDir;

            if (isEmptyTileOnTheSameLine(mostBackwardIdx, currVerticalLine, VERTICAL)) {
                isBackwardable = true;
            } else {
                isBackwardable = false;
            }

            if (isEmptyTileOnTheSameLine(mostForwardIdx, currVerticalLine, VERTICAL)) {
                isForwardable = true;
            } else {
                isForwardable = false;
            }


            // Build words
            for (auto it = verticalValidLetters.begin(); it != verticalValidLetters.end(); ++it) {
                if (isForwardable && std::get<FORWARDWORDEXISTS>(it->second)) {
                    buildWordForwards(mostForwardIdx, mostBackwardIdx, currVerticalLine, VERTICAL,
                                      const_cast<WordPtr &>(it->first));
                }
                if (isBackwardable && std::get<BACKWARDWORDEXISTS>(it->second) {
                    buildWordBackwards(mostForwardIdx, mostBackwardIdx, currVerticalLine, VERTICAL, *it);
                }


            }
        }

        if (!horizontalValidLetters.empty()) {
            currIdx = at->idx;
            currHorizontalLine = getCurrLine(currIdx, HORIZONTAL);
            mostBackwardIdx = currIdx;
            mostForwardIdx = currIdx;
            backwardDir = LEFT;
            forwardDir = RIGHT;

            // Find out mostBackwardIdx first
            mostBackwardIdx = currIdx + backwardDir;
            // As long as on the board and there are already placed tiles
            while (isPlacedTileOnTheSameLine(mostBackwardIdx, currHorizontalLine, HORIZONTAL)) {
                mostBackwardIdx = currIdx + backwardDir;
            }

            // Find out mostForwardIdx
            mostForwardIdx = currIdx + forwardDir;
            // As long as on the board and there are already placed tiles
            while (isPlacedTileOnTheSameLine(mostBackwardIdx, currHorizontalLine, HORIZONTAL)) {
                mostForwardIdx = currIdx + forwardDir;
            }

            // If mostBackwardIdx is the same as currIdx, go backwards again (taking into account the letter to be placed)
            if (mostBackwardIdx == currIdx)
                mostBackwardIdx += backwardDir;
            // If mostForwardIdx is the same as currIdx, go forwards again (taking into account the letter to be placed)
            if (mostForwardIdx == currIdx)
                mostForwardIdx += forwardDir;

            if (isEmptyTileOnTheSameLine(mostBackwardIdx, currHorizontalLine, HORIZONTAL)) {
                isBackwardable = true;
            } else {
                isBackwardable = false;
            }

            if (isEmptyTileOnTheSameLine(mostForwardIdx, currHorizontalLine, HORIZONTAL)) {
                isForwardable = true;
            } else {
                isForwardable = false;
            }

            // Build words
            for (auto it = horizontalValidLetters.begin(); it != horizontalValidLetters.end(); ++it) {
                if (isForwardable)
                    buildWordForwards(mostForwardIdx, mostBackwardIdx, currHorizontalLine, HORIZONTAL, *it);
                if (isBackwardable)
                    buildWordBackwards(mostForwardIdx, mostBackwardIdx, currHorizontalLine, HORIZONTAL, *it);
            }
        }

        // Move over to the next adjacent tile
        tilesToStartFrom.pop();
    }





}

bool WordBuilder::wordCanStart(const std::string& lettersToSearch) {
    bool res = true;
    if (!greedyForwardMap->count(lettersToSearch))
        res = false;
    else if (!(*greedyForwardMap)[lettersToSearch]->count('|'))
        res = false;
    return res;
}

bool WordBuilder::wordCanEnd(const std::string& lettersToSearch) {
    bool res = true;
    if (!greedyBackwardMap->count(lettersToSearch))
        res = false;
    else if (!(*greedyBackwardMap)[lettersToSearch]->count('|'))
        res = false;
    return res;
}


//void WordBuilder::buildWord(
//                             int backwardIdx,
//                             int forwardIdx,
//                             int currLine,
//                             Angle angle,
//                             bool isForwardable,
//                             bool isBackwardable,
//                             Word& wordBeingBuilt) {
//
//    if (wordBeingBuilt.lettersInHand.empty() ||
//        (!isForwardable && !isBackwardable) ||
//        (!isOnBaseLine(backwardIdx, currLine, angle) &&
//        !isOnBaseLine(forwardIdx, currLine, angle))) {
//    } else {
//        // Recursive case
//        // 1. Compare the two indices with respect to the number of wordsInQueue that can be extended given all the tiles in the hand.
//        // 2. Choose the letter with the highest number of counts from either index
//        // 3. Add the letter to a string
//
//        // Set up the directions
//        int forwardDir;
//        int backwardDir;
//        if (angle == VERTICAL) {
//            backwardDir = TOP;
//            forwardDir = BOTTOM;
//        } else {
//            backwardDir = LEFT;
//            forwardDir = RIGHT;
//        }
//
//        if (dictionary->isInDict(wordBeingBuilt.wordBeingBuilt))
//            wordsInQueue->push(wordBeingBuilt);
//
//        if (isForwardable) {
//            // Check whether forwardIdx is on the same line as the original start index
//            if (isOnBaseLine(forwardIdx, currLine, angle)) {
//                bool letterFound = false;
//                for (const auto letter: wordBeingBuilt.lettersInHand) {
//                    // Check whether letter can be found in the map
//                    if ((*greedyForwardMap)[wordBeingBuilt.wordBeingBuilt]->count(letter)) {
//                        auto at = adjacentTiles[forwardIdx];
//                        // If on an AdjacentTile
//                        if (at != nullptr) {
//                            std::string verticalLetters;
//                            std::string horizontalLetters;
//                            if (angle == VERTICAL) {
//                                verticalLetters = wordBeingBuilt.wordBeingBuilt + letter;
//                                horizontalLetters = std::string(1, letter);
//                                // If going vertical, the forward direction is bottom, which means the adjacentTile can have at most three elements
//                                if (!at->adjacentLetters[DOWNWARD].empty())
//                                    verticalLetters += at->adjacentLetters[DOWNWARD];
//                                if (!at->adjacentLetters[LEFTWARD].empty())
//                                    horizontalLetters = at->adjacentLetters[LEFTWARD] + horizontalLetters;
//                                if (!at->adjacentLetters[RIGHTWARD].empty())
//                                    horizontalLetters += at->adjacentLetters[RIGHTWARD];
//                            } else {
//                                verticalLetters = std::string(1, letter);
//                                horizontalLetters = wordBeingBuilt.wordBeingBuilt + letter;
//                                // If going horizontal, the forward direction is right, which means the adjacentTile can have at most three elements
//                                if (!at->adjacentLetters[UPWARD].empty())
//                                    verticalLetters = at->adjacentLetters[UPWARD] + verticalLetters;
//                                if (!at->adjacentLetters[DOWNWARD].empty())
//                                    verticalLetters += at->adjacentLetters[DOWNWARD];
//                                if (!at->adjacentLetters[RIGHTWARD].empty())
//                                    horizontalLetters += at->adjacentLetters[RIGHTWARD];
//                            }
//
//                            if ((angle == VERTICAL && dictionary->isInDict(horizontalLetters)) ||
//                                (angle == HORIZONTAL && dictionary->isInDict(verticalLetters))) {
//                                Word word2(wordBeingBuilt);
//
//                                if (angle == VERTICAL)
//                                    word2.wordBeingBuilt = verticalLetters;
//                                else
//                                    word2.wordBeingBuilt = horizontalLetters;//       tempWords.push_back(verticalLetters);
//                                word2.tileIndices.insert(std::make_pair(forwardIdx, letter));
//                                // Erase the letter from the lettersInHand
//                                word2.erase(letter);
//                                // Traverse forwards until either an empty cell is encountered or out of the current line
//                                forwardIdx += forwardDir;
//                                while (isOnBaseLine(forwardIdx, currLine, angle) &&
//                                       board->hasPlacedTile(forwardIdx))
//                                    forwardIdx += forwardDir;
//
//                                buildWord(backwardIdx, forwardIdx, currLine, angle, isForwardable, isBackwardable,
//                                          word2);
//                                letterFound = true;
//                            }
//                        } else {
//                            Word word2(wordBeingBuilt);
//                            word2.wordBeingBuilt += letter;
//                            word2.tileIndices.insert(std::make_pair(forwardIdx, letter));
//                            // Erase the letter from the lettersInHand
//                            word2.erase(letter);
//                            // Traverse forwards until either an empty cell is encountered or out of the current line
//                            forwardIdx += forwardDir;
//                            while (isOnBaseLine(forwardIdx, currLine, angle) && board->hasPlacedTile(forwardIdx))
//                                forwardIdx += forwardDir;
//
//                            buildWord(backwardIdx, forwardIdx, currLine, angle, isForwardable, isBackwardable,
//                                      word2);
//                            letterFound = true;
//                        }
//                    }
//                }
//                if (!letterFound)
//                    isForwardable = false;
//            } else {
//                isForwardable = false;
//            }
//        }
//
//        if (isBackwardable) {
//            if (isOnBaseLine(backwardIdx, currLine, angle)) {
//                bool letterFound = false;
//                for (const auto letter: wordBeingBuilt.lettersInHand) {
//                // Check whether backwardIdx is on the same line as the original start index
//                    // Check whether letter can be found in the map
//                    if ((*greedyBackwardMap)[wordBeingBuilt.wordBeingBuilt]->count(letter)) {
//                        auto at = adjacentTiles[backwardIdx];
//                        // If on an AdjacentTile
//                        if (at != nullptr) {
//                            std::string verticalLetters;
//                            std::string horizontalLetters;
//                            if (angle == VERTICAL) {
//                                verticalLetters = letter + wordBeingBuilt.wordBeingBuilt;
//                                horizontalLetters = std::string(1, letter);
//                                // If going vertical, the backward direction is top
//                                if (!at->adjacentLetters[UPWARD].empty())
//                                    verticalLetters = at->adjacentLetters[UPWARD] + verticalLetters;
//                                if (!at->adjacentLetters[LEFTWARD].empty())
//                                    horizontalLetters = at->adjacentLetters[LEFTWARD] + horizontalLetters;
//                                if (!at->adjacentLetters[RIGHTWARD].empty())
//                                    horizontalLetters += at->adjacentLetters[RIGHTWARD];
//                            } else { // If HORIZONTAL
//                                verticalLetters = std::string(1, letter);
//                                horizontalLetters = letter + wordBeingBuilt.wordBeingBuilt;
//                                // If going horizontal, the backward direction is left
//                                if (!at->adjacentLetters[UPWARD].empty())
//                                    verticalLetters = at->adjacentLetters[UPWARD] + verticalLetters;
//                                if (!at->adjacentLetters[DOWNWARD].empty())
//                                    verticalLetters += at->adjacentLetters[DOWNWARD];
//                                if (!at->adjacentLetters[LEFTWARD].empty())
//                                    horizontalLetters = at->adjacentLetters[LEFTWARD] + horizontalLetters;
//                            }
//
//                            if ((angle == VERTICAL && dictionary->isInDict(horizontalLetters)) ||
//                                (angle == HORIZONTAL && dictionary->isInDict(verticalLetters))) {
//                                Word word2(wordBeingBuilt);
//
//                                if (angle == VERTICAL)
//                                    word2.wordBeingBuilt = verticalLetters;
//                                else
//                                    word2.wordBeingBuilt = horizontalLetters;//       tempWords.push_back(verticalLetters);
//                                word2.tileIndices.insert(std::make_pair(backwardIdx, letter));
//                                // Erase the letter from the lettersInHand
//                                word2.erase(letter);
//                                // Traverse forwards until either an empty cell is encountered or out of the current line
//                                backwardIdx += backwardDir;
//                                while (isOnBaseLine(backwardIdx, currLine, angle) &&
//                                       board->hasPlacedTile(backwardIdx))
//                                    backwardIdx += backwardDir;
//
//                                buildWord(backwardIdx, forwardIdx, currLine, angle, isForwardable, isBackwardable,
//                                          word2);
//                                letterFound = true;
//                            }
//                        } else {
//                            Word word2(wordBeingBuilt);
//                            word2.wordBeingBuilt += letter;
//                            word2.tileIndices.insert(std::make_pair(backwardIdx, letter));
//                            // Erase the letter from the lettersInHand
//                            word2.erase(letter);
//                            // Traverse forwards until either an empty cell is encountered or out of the current line
//                            backwardIdx += backwardDir;
//                            while (isOnBaseLine(backwardIdx, currLine, angle) && board->hasPlacedTile(forwardIdx))
//                                backwardIdx += backwardDir;
//
//                            buildWord(backwardIdx, forwardIdx, currLine, angle, isForwardable, isBackwardable,
//                                      word2);
//                            letterFound = true;
//                        }
//                    }
//                }
//                if (!letterFound)
//                    isBackwardable = false;
//
//            } else {
//                isBackwardable = false;
//            }
//        }
//    }
//}

void WordBuilder::buildWordForwards(int forwardIdx,
                                    int backwardIdx,
                                    int currLine,
                                    Angle angle,
                                    WordPtr& word) {
    if (word->lettersInHand.empty() ||
    !isOnBaseLine(forwardIdx, currLine, angle) ||
    !isOnBoard(forwardIdx) ||
    !isOnBoard(backwardIdx) )  {
    } else {
        // Recursive case
        // 1. Compare the two indices with respect to the number of wordsInQueue that can be extended given all the tiles in the hand.
        // 2. Choose the letter with the highest number of counts from either index
        // 3. Add the letter to a string

        // Set up the directions
        int forwardDir;
        if (angle == VERTICAL) {
            forwardDir = BOTTOM;
        } else {
            forwardDir = RIGHT;
        }

        int forwardIdxToSearch;

        if (dictionary->isInDict(word->wordBeingBuilt))
            wordsInQueue->push(word);

        // Check whether forwardIdx is on the same line as the original start index
        if (isOnBaseLine(forwardIdx, currLine, angle)) {
            for (const auto letter: word->lettersInHand) {
                forwardIdxToSearch = forwardIdx;
                // Check whether letter can be found in the map
                if ((*greedyForwardMap)[word->wordBeingBuilt] && (*greedyForwardMap)[word->wordBeingBuilt]->count(letter)) {
                    auto at = adjacentTiles[forwardIdx];
                    // If on an AdjacentTile
                    if (at != nullptr) {
                        std::string verticalLetters;
                        std::string horizontalLetters;
                        if (angle == VERTICAL) {
                            verticalLetters = word->wordBeingBuilt + letter;
                            horizontalLetters = std::string(1, letter);
                            // If going vertical, the forward direction is bottom, which means the adjacentTile can have at most three elements
                            if (!at->adjacentLetters[DOWNWARD].empty())
                                verticalLetters += at->adjacentLetters[DOWNWARD];
                            if (!at->adjacentLetters[LEFTWARD].empty())
                                horizontalLetters = at->adjacentLetters[LEFTWARD] + horizontalLetters;
                            if (!at->adjacentLetters[RIGHTWARD].empty())
                                horizontalLetters += at->adjacentLetters[RIGHTWARD];
                        } else {
                            verticalLetters = std::string(1, letter);
                            horizontalLetters = word->wordBeingBuilt + letter;
                            // If going horizontal, the forward direction is right, which means the adjacentTile can have at most three elements
                            if (!at->adjacentLetters[UPWARD].empty())
                                verticalLetters = at->adjacentLetters[UPWARD] + verticalLetters;
                            if (!at->adjacentLetters[DOWNWARD].empty())
                                verticalLetters += at->adjacentLetters[DOWNWARD];
                            if (!at->adjacentLetters[RIGHTWARD].empty())
                                horizontalLetters += at->adjacentLetters[RIGHTWARD];
                        }

                        if ((angle == VERTICAL && dictionary->isInDict(horizontalLetters)) ||
                            (angle == HORIZONTAL && dictionary->isInDict(verticalLetters))) {
                            Word word2(word);

                            if (angle == VERTICAL)
                                word2.wordBeingBuilt = verticalLetters;
                            else
                                word2.wordBeingBuilt = horizontalLetters;//       tempWords.push_back(verticalLetters);
                            word2.tileIndices.insert(std::make_pair(forwardIdx, letter));
                            // Erase the letter from the lettersInHand
                            word2.erase(letter);
                            // Traverse forwards until either an empty cell is encountered or out of the current line
                            forwardIdxToSearch += forwardDir;
                            while (isOnBaseLine(forwardIdxToSearch, currLine, angle) &&
                                   board->hasPlacedTile(forwardIdxToSearch))
                                forwardIdxToSearch += forwardDir;

                            buildWordForwards(forwardIdxToSearch, backwardIdx, currLine, angle, word2);
                            buildWordBackwards( forwardIdxToSearch, backwardIdx, currLine, angle, word2);
                        }
                    } else {
                        Word word2(word);
                        word2.wordBeingBuilt += letter;
                        word2.tileIndices.insert(std::make_pair(forwardIdx, letter));
                        // Erase the letter from the lettersInHand
                        word2.erase(letter);
                        // Traverse forwards until either an empty cell is encountered or out of the current line
                        forwardIdxToSearch += forwardDir;
                        while (isOnBaseLine(forwardIdxToSearch, currLine, angle) &&
                               board->hasPlacedTile(forwardIdxToSearch))
                            forwardIdxToSearch += forwardDir;

                        buildWordForwards(forwardIdxToSearch, backwardIdx, currLine, angle, word2);
                        buildWordBackwards(forwardIdxToSearch, backwardIdx,  currLine, angle, word2);
                    }
                }
            }
        }
    }
}

void WordBuilder::buildWordBackwards(int forwardIdx,
                                     int backwardIdx,
                                     int currLine,
                                     Angle angle,
                                     WordPtr& word) {
    // Base case
    // There is no more letter to place in the hand
    if (word->lettersInHand.empty()) {
        // Then do not enter the recursion
    } else {
        // Recursive case

        // Set up the directions
        int backwardDir;
        if (angle == VERTICAL) {
            backwardDir = TOP;
        } else {
            backwardDir = LEFT;
        }

        int backwardIdxToSearch;
        WordPtr newWord;
        bool isForwardable;
        bool isBackwardable;

        if (isOnBaseLine(backwardIdx, currLine, angle)) {
            for (const auto letter: word->lettersInHand) {
                // Check whether backwardIdx is on the same line as the original start index
                // Check whether letter can be found in the map
                if ((*greedyBackwardMap)[word->wordBeingBuilt] && (*greedyBackwardMap)[word->wordBeingBuilt]->count(letter)) {
                    backwardIdxToSearch = backwardIdx;
                    newWord = nullptr;
                    isForwardable = false;
                    isBackwardable = false;
                    auto at = adjacentTiles[backwardIdx];
                    if (at != nullptr) {
                        std::string verticalLetters;
                        std::string horizontalLetters;
                        // Process all the connected letters
                        if (angle == VERTICAL) {
                            verticalLetters = letter + word->wordBeingBuilt;
                            horizontalLetters = std::string(1, letter);
                            // If going vertical, the backward direction is top
                            if (!at->adjacentLetters[UPWARD].empty())
                                verticalLetters = at->adjacentLetters[UPWARD] + verticalLetters;
                            if (!at->adjacentLetters[LEFTWARD].empty())
                                horizontalLetters = at->adjacentLetters[LEFTWARD] + horizontalLetters;
                            if (!at->adjacentLetters[RIGHTWARD].empty())
                                horizontalLetters += at->adjacentLetters[RIGHTWARD];
                        } else { // If HORIZONTAL
                            verticalLetters = std::string(1, letter);
                            horizontalLetters = letter + word->wordBeingBuilt;
                            // If going horizontal, the backward direction is left
                            if (!at->adjacentLetters[UPWARD].empty())
                                verticalLetters = at->adjacentLetters[UPWARD] + verticalLetters;
                            if (!at->adjacentLetters[DOWNWARD].empty())
                                verticalLetters += at->adjacentLetters[DOWNWARD];
                            if (!at->adjacentLetters[LEFTWARD].empty())
                                horizontalLetters = at->adjacentLetters[LEFTWARD] + horizontalLetters;
                        }

                        // Check whether the connected letters at a perpendicular angle is in the dictionary
                        if (angle == VERTICAL && dictionary->isInDict(horizontalLetters)) {
                            if ((*greedyForwardMap)[verticalLetters]) {
                                newWord = std::make_shared<Word>(*word);
                                newWord->wordBeingBuilt = verticalLetters;
                                newWord->builtWord = horizontalLetters;
                                isForwardable = true;
                            }
                            if ((*greedyBackwardMap)[verticalLetters]) {
                                newWord = std::make_shared<Word>(*word);
                                newWord->wordBeingBuilt = verticalLetters;
                                newWord->builtWord = horizontalLetters;
                                isBackwardable = true;
                            }
                        }

                        // Check whether the connected letters at a perpendicular angle is in the dictionary
                        if (angle == HORIZONTAL && dictionary->isInDict(verticalLetters)) {
                            if ((*greedyForwardMap)[horizontalLetters]) {
                                newWord = std::make_shared<Word>(*word);
                                newWord->wordBeingBuilt = horizontalLetters;
                                newWord->builtWord = verticalLetters;
                                isForwardable = true;
                            }
                            if ((*greedyBackwardMap)[horizontalLetters]) {
                                newWord = std::make_shared<Word>(*word);
                                newWord->wordBeingBuilt = horizontalLetters;
                                newWord->builtWord = verticalLetters;
                                isBackwardable = true;
                            }
                        }

                    } else {
                        // If the cell at the current index is an empty cell
                        newWord = std::make_shared<Word>(*word);
                        newWord->wordBeingBuilt = letter + newWord->wordBeingBuilt;
                        if ((*greedyForwardMap)[newWord->wordBeingBuilt])
                            isForwardable = true;
                        if ((*greedyBackwardMap)[newWord->wordBeingBuilt])
                            isBackwardable = true;
                    }
                }

                // If newWord has been assigned
                if (newWord) {
                    newWord->tileIndices.insert(std::make_pair(backwardIdx, letter));
                    // Erase the letter from the lettersInHand
                    newWord->erase(letter);

                    // Traverse forwards until either an empty cell is encountered or out of the current line
                    backwardIdxToSearch += backwardDir;
                    while (isPlacedTileOnTheSameLine(backwardIdxToSearch, currLine, angle))
                        backwardIdxToSearch += backwardDir;
                    if (isBackwardable && isEmptyTileOnTheSameLine(backwardIdxToSearch, currLine, angle)) {
                        buildWordBackwards(forwardIdx, backwardIdx, currLine, angle, newWord);
                    }
                    if (isForwardable && isEmptyTileOnTheSameLine(forwardIdx, currLine, angle)) {
                        buildWordForwards(forwardIdx, backwardIdx, currLine, angle, newWord);
                    }

                    // Add a word if it exists in a dictionary
                    if (dictionary->isInDict(newWord->wordBeingBuilt))
                        wordsInQueue->push(newWord);

                }

            }
        }
    }
}


//    while (isForwardable && isBackwardable && !wordBeingBuilt.lettersInHand.empty()) {
//        isLetterFound = false;
//        forwardMap.clear();
//        backwardMap.clear();
//        // Check the lettersInHand
//
//        for (const auto letter: wordBeingBuilt.lettersInHand) {
//            // Check whether backwardIdx is on the same line as the original start index
//            if (isOnBaseLine(backwardIdx, currLine, angle)) {
//                // Check whether letter can be found in the map
//
//                if ((*greedyBackwardMap)[wordBeingBuilt.wordBeingBuilt]->count(letter)) {
//                    size_t letterCount = (*(*greedyBackwardMap)[wordBeingBuilt.wordBeingBuilt])[letter];
//                    auto at = adjacentTiles[backwardIdx];
//                    // If on an AdjacentTile
//                    if (at != nullptr) {
//                        std::string verticalLetters (1, letter);
//                        std::string horizontalLetters (1, letter);
//                        if (angle == VERTICAL) {
//                            // If going vertical, the backward direction is top
//                            if (!at->adjacentLetters[UPWARD].empty())
//                                verticalLetters = at->adjacentLetters[UPWARD] + letter + wordBeingBuilt.wordBeingBuilt;
//                            if (!at->adjacentLetters[LEFTWARD].empty())
//                                horizontalLetters = at->adjacentLetters[LEFTWARD] + letter;
//                            if (!at->adjacentLetters[RIGHTWARD].empty())
//                                horizontalLetters += at->adjacentLetters[RIGHTWARD];
//                        } else { // If HORIZONTAL
//                            // If going horizontal, the backward direction is left
//                            if (!at->adjacentLetters[UPWARD].empty())
//                                verticalLetters = at->adjacentLetters[UPWARD];
//                            if (!at->adjacentLetters[DOWNWARD].empty())
//                                verticalLetters += letter +
//                                                   at->adjacentLetters[DOWNWARD];
//                            if (!at->adjacentLetters[LEFTWARD].empty())
//                                horizontalLetters = at->adjacentLetters[LEFTWARD] + letter + wordBeingBuilt.wordBeingBuilt;
//                        }
//
//                        if (verticalLetters.size() > 1 && horizontalLetters.size() > 1) {
//                            if (dictionary->isInDict(verticalLetters) &&
//                                dictionary->isInDict(horizontalLetters)) {
//                                // If both are words found in dictionary, add a wordBeingBuilt in the perpendicular direction
//                                Word word2 (wordBeingBuilt);
//                                if (angle == VERTICAL) {
//                                    if (horizontalLetters.size() > 1)
//                                        tempWords.push_back(horizontalLetters);
//                                    word2.wordBeingBuilt = verticalLetters;
//                                } else {
//                                    if (verticalLetters.size() > 1)
//                                        tempWords.push_back(verticalLetters);
//                                    word2.wordBeingBuilt = horizontalLetters;
//                                }
//                                word2.tileIndices.insert(std::make_pair(backwardIdx, letter));
//
//                                // Erase the letter from the lettersInHand
//                                word2.erase(letter);
//
//                                // Set isLetterFound to true so that isBackwardable is not set to true
//                                isLetterFound = true;
//
//                                // Traverse backwards until either an empty cell is encountered or out of the current line
//                                backwardIdx += backwardDir;
//                                while (isOnBaseLine(backwardIdx, currLine, angle) && board->hasPlacedTile(backwardIdx))
//                                    backwardIdx += backwardDir;
//                                buildWord(backwardIdx, forwardIdx, currLine, angle, word2);
//                            }
//                        } else {
//                            backwardMap.insert(std::make_pair(letterCount, letter));
//                        }
//
//                    } else { // If Adjacent Tile is a nullptr, which means the cell at backwardIdx is an empty cell
//                        backwardMap.insert(std::make_pair(letterCount, letter));
//                    }
//                }
//            } else {
//                isBackwardable = false;
//            }
//        }
//
//        if (backwardMap.empty() && !isLetterFound)
//            isBackwardable = false;
//
//        for (const auto letter: lettersInHand) {
//            // Check whether forwardIdx is on the same line as the original start index
//            if (isOnBaseLine(forwardIdx, currLine, angle)) {
//                // Check whether letter can be found in the map
//                // TODO Check if a letter's score is higher than the highest
//                if ((*greedyForwardMap)[tempLetters]->count(letter) && !isLetterFound) {
//                    size_t letterCount = (*(*greedyForwardMap)[tempLetters])[letter];
//                    auto at = adjacentTiles[forwardIdx];
//                    // If on an AdjacentTile
//                    if (at != nullptr) {
//                        std::string verticalLetters(1, letter);
//                        std::string horizontalLetters(1, letter);
//                        if (angle == VERTICAL) {
//                            // If going vertical, the forward direction is bottom, which means the adjacentTile can have at most three elements
//                            if (!at->adjacentLetters[DOWNWARD].empty())
//                                verticalLetters = tempLetters + letter + at->adjacentLetters[DOWNWARD];
//                            if (!at->adjacentLetters[LEFTWARD].empty())
//                                horizontalLetters = at->adjacentLetters[LEFTWARD] + letter;
//                            if (!at->adjacentLetters[RIGHTWARD].empty())
//                                horizontalLetters += at->adjacentLetters[RIGHTWARD];
//                        } else {
//                            // If going horizontal, the forward direction is right, which means the adjacentTile can have at most three elements
//                            if (!at->adjacentLetters[UPWARD].empty())
//                                verticalLetters = at->adjacentLetters[UPWARD];
//                            if (!at->adjacentLetters[DOWNWARD].empty())
//                                verticalLetters += letter + at->adjacentLetters[DOWNWARD];
//                            if (!at->adjacentLetters[RIGHTWARD].empty())
//                                horizontalLetters = tempLetters + letter + at->adjacentLetters[RIGHTWARD];
//                        }
//
//                        if (verticalLetters.size() > 1 && horizontalLetters.size() > 1) {
//                            if (dictionary->isInDict(verticalLetters) &&
//                                dictionary->isInDict(horizontalLetters)) {
//                                // If both are words found in dictionary, add a wordBeingBuilt in the perpendicular direction
//                                if (angle == VERTICAL) {
//                                    if (horizontalLetters.size() > 1)
//                                        tempWords.push_back(horizontalLetters);
//                                    tempLetters = verticalLetters;
//                                } else {
//                                    if (verticalLetters.size() > 1)
//                                        tempWords.push_back(verticalLetters);
//                                    tempLetters = horizontalLetters;
//                                }
//                                tilesIndices.insert(std::make_pair(backwardIdx, letter));
//
//                                // Erase the letter from the lettersInHand
//                                auto it = std::find(lettersInHand.begin(), lettersInHand.end(), letter);
//                                lettersInHand.erase(it);
//
//                                // Set isLetterFound to true so that isForwardable is set to false
//                                isLetterFound = true;
//
//                                // Traverse forwards until either an empty cell is encountered or out of the current line
//                                forwardIdx += forwardDir;
//                                while (isOnBaseLine(forwardIdx, currLine, angle) && board->hasPlacedTile(forwardIdx))
//                                    forwardIdx += forwardDir;
//                            }
//                        } else {
//                            forwardMap.insert(std::make_pair(letterCount, letter));
//                        }
//                    } else {
//                        forwardMap.insert(std::make_pair(letterCount, letter));
//                    }
//                }
//            } else {
//                isForwardable = false;
//            }
//        }
//
//        if (forwardMap.empty() && !isLetterFound)
//            isForwardable = false;
//
//        // Compare two of the generated maps, namely forwardMap and backwardMap and find the letter with the highest count
//        if (isBackwardable && isForwardable) {
//            auto forwardIt = forwardMap.rbegin();
//            auto backwardIt = backwardMap.rbegin();
//
//            decltype(forwardIt) chosenIt = forwardIt;
//            if (chosenIt->first < backwardIt->first) {
//                chosenIt = backwardIt;
//            }
//
//            // Add the chosen character to tempLetters and the corresponding index to tileIndices
//            int *chosenIdx;
//            if (chosenIt == backwardIt) {
//                chosenIdx = &backwardIdx;
//                tempLetters.insert(0, 1, chosenIt->second);
//            } else {
//                chosenIdx = &forwardIdx;
//                tempLetters += chosenIt->second;
//            }
//            tilesIndices.insert(std::make_pair(*chosenIdx, chosenIt->second));
//
//            // Erase the letter from the lettersInHand
//            auto it = std::find(lettersInHand.begin(), lettersInHand.end(), chosenIt->second);
//            lettersInHand.erase(it);
//
//            // Traverse in whichever chosen direction until either an empty cell is encountered or out of the current line
//            int chosenDir;
//            if (chosenIt == backwardIt) {
//                chosenDir = backwardDir;
//            } else {
//                chosenDir = forwardDir;
//            }
//
//            *chosenIdx += chosenDir;
//            while (isOnBaseLine(*chosenIdx, currLine, angle) && board->hasPlacedTile(*chosenIdx))
//                *chosenIdx += chosenDir;
//
//        } else if (isForwardable) {
//            auto forwardIt = forwardMap.rbegin();
//            tempLetters += forwardIt->second;
//            tilesIndices.insert(std::make_pair(forwardIdx, forwardIt->second));
//            // Erase the letter from the lettersInHand
//            auto it = std::find(lettersInHand.begin(), lettersInHand.end(), forwardIt->second);
//            lettersInHand.erase(it);
//            // Traverse forward until either an empty cell is encountered or out of the current line
//            forwardIdx += forwardDir;
//            while (isOnBaseLine(forwardIdx, currLine, angle) && board->hasPlacedTile(forwardIdx))
//                forwardIdx += forwardDir;
//        } else if (isBackwardable) {
//            auto backwardIt = backwardMap.rbegin();
//            tempLetters += backwardIt->second;
//            tilesIndices.insert(std::make_pair(backwardIdx, backwardIt->second));
//            // Erase the letter from the lettersInHand
//            auto it = std::find(lettersInHand.begin(), lettersInHand.end(), backwardIt->second);
//            lettersInHand.erase(it);
//            // Traverse forward until either an empty cell is encountered or out of the current line
//            backwardIdx += backwardDir;
//            while (isOnBaseLine(backwardIdx, currLine, angle) && board->hasPlacedTile(backwardIdx))
//                backwardIdx += backwardDir;
//        }
//    }


//

//
//    std::cout << tempLetters << std::endl;
//                // Check if tempLetters count as a wordBeingBuilt. If so, add it to the priority queue
//    tempWords.push_back(tempLetters);
//    if (dictionary->isInDict(tempWords)) {
////        wordsInQueue->push(std::make_shared<Word>(tempWords, tilesIndices));
//    }


//}


void WordBuilder::setBoard(BoardPtr board) {
    this->board = board;
}

void WordBuilder::setDictionary(DictionaryPtr dictionary) {
    this->dictionary = dictionary;
}

void WordBuilder::beAwareOfTiles() {
    auto placedDir = board->getPlacedDir();
    Angle singleSearchDir;
    Angle multipleSearchDir;
    if (placedDir == HORIZONTAL) {
        singleSearchDir = HORIZONTAL;
        multipleSearchDir = VERTICAL;
    } else {
        singleSearchDir = VERTICAL;
        multipleSearchDir = HORIZONTAL;
    }

    // Access the leftmost or uppermost index among the tiles just placed
    placedIndicesPtr placedIndices = board->getPlacedIndices();
    int currIdx = placedIndices->top();
    int currLine = getCurrLine(currIdx, placedDir);

    // First, build letters in the same direction as placedDir
    createOrUpdateEmptyAdjacencyTiles(singleSearchDir, currIdx, currLine, placedDir);

    // Next, build letters in the perpendicular direction as placedDir
    // Process the placed indices in a priority queue
    while (!placedIndices->empty()) {
        // Access the most leftmost or uppermost index among those of the placed tiles
        currIdx = placedIndices->top();
        currLine = getCurrLine(currIdx, multipleSearchDir);
        placedIndices->pop();
        // Destroy the emptyAdjacent Tile if found on the placed indices
        if (adjacentTiles[currIdx] != nullptr) {
            createOrUpdateEmptyAdjacencyTiles(multipleSearchDir, currIdx, currLine, placedDir);
            adjacentTiles[currIdx] = nullptr;
        } else {
            createOrUpdateEmptyAdjacencyTiles(multipleSearchDir, currIdx, currLine, placedDir);
        }
    }
}

void WordBuilder::prioritiseTiles() {
    for (auto adjacentTile : adjacentTiles) {
        if (adjacentTile)
            tilesToStartFrom.push(adjacentTile);
    }
}

/*
 * There are two algorithms used to place tiles.
 *
 * First, if a given row or column has only one letter in the center, sortedMap can be used to increase the chances of
 * hitting a bingo, as no ordering needs to be factored in when placing the entire tiles in the hand.
 *
 * In order to check whether a given AdjacentTile is solvable by sortedMap, first its data member adjacentLetters
 * is checked to see how many placed tile is connected to it. If there is only one, the presumably adjacent tile
 * on the opposite side of it and its data member adjacentLetters is also checked. If this check passes,
 * the rows or columns next to the one the original AdjacentTile is on are checked in its entirety.
 *
 * If there are no placed tiles found other than the ones on the centre, the sortedMap can be used to quickly
 * find 7 or 8 letter-long wordsInQueue.
 *
 * Second, if the check above does not turn up any valid wordBeingBuilt, the greedy algorithm is used.
 * As all the AdjacentTiles are stored in a priority queue with the potentialScore data member as a key,
 * the one with the highest potential score can be looked at first.
 *
 * If the AdjacentTile has only one placed tile connected to it, the algorithm is first made to go in
 * the perpendicular direction to generate a long wordBeingBuilt.
 *
 * The algorithm first looks up connected letters in forwardGreedyMap if they are placed to the right or bottom
 * of an AdjacentTile and iterates each letter in the hand to see a wordBeingBuilt can be made with any of the letters in the hand
 * in front of the existing connected letters. If they are placed to the left or top of an AdjacentTile, backwardGreedyMap
 * is accessed to find whether the existing connected letters can be extended by any of the letters in the hand.
 *
 * If not, its data member misExtendible is set to false and its potentialScores is also set to 0.
 * Otherwise, the aggregate number of wordsInQueue that can be made with any of the letters in the hand is stored.
 * The number of wordsInQueue is then compared to that of an AdjacentTile on its opposite end to see which presents
 * higher chances of being made into a wordBeingBuilt
 * (in each turn, built letters are searched in a dictionary and put into a priority queue with its score).
 *
 * Once one of the AdjacentTiles is selected, the most frequent letter is placed (since the starting letter(s) that was
 * already placed was deemed the highest, the most frequent letter is chosen from the hand).
 *
 * In the next round, the same step is repeated. If the current tile on the board being enquired is an AdjacentTile,
 * its data member adjacentLetters is accessed to check whether the trailing letters can be connected to all the letters
 * found in adjacentLetters. If not, the algorithm no longer goes forward
 *
 *
 *
 *
 *
 *
 *
 *
 *
 */
void WordBuilder::placeTiles() {

}

//void WordBuilder::think() {
//
//}

// Iterate placedTiles to update a vector of a vector of adjacent tiles
// and place the updated adjacent tiles into singleWordTiles or multiWordTiles
// depending on how many adjacent tiles they themselves have next to them
// If there is only one adjacent tile (the one placed on the board),
// it will go into singleWordTiles. If more than one, multiWordTiles.
void WordBuilder::updateAdjacentTiles() {


//    std::shared_ptr<std::multimap<int, std::tuple<int, int>>> testMap =
//            std::make_shared<std::multimap<int, std::tuple<int, int>>>();




//
//    int mostFowardIdx = getMostForwardIdx(currIdx);
//    workMostBackward(mostFowardIdx);
//
//
//
//    // Process the placed indices
//    while (!placedIndices->empty()) {
//        //TODO destroy the emptyAdjacnet Tile if found on the placed indices
//        int placedIdx = placedIndices->top();
//        placedIndices->pop();
//
//        int currIdx = placedIdx;
//
//
//
//
//
//        // TODO incorporate conditional checking dependent on placedDirection
//        // Create EmptyAdjacentTiles around the placed tile by looking to the top, right, bottom, and left
//        // if the cell under search is inside the board AND there is no placed tile.
//        //
//        // if emptyAdjacentTile is in the given position, update it
//        //
//
//        // If placed horizontally, go through up and down first
//        // TODO: Consider avoiding copying below
//        std::array<BoardDir, TOTALDIRECTIONS / 2> directions;
//        if (placedDir == HORIZONTAL) {
//            directions = verticlDirections;
//        } else if (placedDir == VERTICAL) {
//            directions = horizontalDirections;
//        }
//
//        for (BoardDir direction: directions) {
//            currIdx = placedIdx + direction;
//            if (isOnBaseLine(currIdx)) {
//                // While there is no placedTile in the direction
//                std::istringstream iss;
//                while (!board->hasPlacedTile(currIdx)) {
//                    // Collect letters
//                    currIdx += direction;
//                }
//                // If adjacentTile does not exist in the given index
//                if (!adjacentTiles[currIdx]) {
//                    // the cell in question has been never adjacent to any placed tile
//                    // TODO find a way to store a score made of more than one letter
//                    EmptyAdjacentTileWeakPtr currEat = std::make_shared<AdjacentTile>(
//                            board->getValue(placedIdx), board->getLetters(placedIdx),
//                            direction, currIdx);
//                    adjacentTiles[currIdx] = currEat;
//                    tilesToStartFrom->insert(currEat);
//                }
//
//            }
//        }
//    }
//
//    for (auto it = tilesToStartFrom->rbegin(); it != tilesToStartFrom->rend(); ++it) {
//        for (auto letters : (*it)->adjacentLetters) {
//            std::cout << letters << " ";
//        }
//        std::cout << std::endl;
//    }






        // TODO refactor

//        if (board->getPlacedDir() == NONE) {
//            // Iterate over all the immediate valid cells on the board relative to currIdx
//            for (BoardDir direction : directions) {
//                currIdx += direction;
//                if (isOnBaseLine(currIdx)) {
//                    // If adjacentTile does not exist in the given index
//                    if (!hasAdjacentTiles[currIdx]) {
//                        // the cell in question has been never adjacent to any placed tile
//                        singleWordTiles.insert(
//                                std::make_shared<AdjacentTile>(
//                                        board->getValue(placedIdx),
//                                        placedIdx,
//                                        direction
//                                        ));
//                        hasAdjacentTiles[currIdx] = true;
//                        // Traverse in the same direction until another adjacent tile is found
//                        // If found, update its boundingIndices, especially its bounding index in the opposite direction
//                        currIdx += direction;
//                        while (isOnBaseLine(currIdx)) {
//                            if (hasAdjacentTiles[currIdx])
//                                // TODO
//
//                            currIdx += direction;
//                        }
//                    } else {
//
//                    }
//
//                }
//            }
//        }
//        if (board->getPlacedDir() == HORIZONTAL) {
//            testMap->insert(std::make_pair(board->getValue(placedIdx),
//                                           std::make_tuple<>(placedIdx, board->getPlacedDir())));
//        }
//        if (board->getPlacedDir() == VERTICAL) {
//
//        }
//    }
//
//    for (auto rit = testMap->rbegin(); rit != testMap->rend(); ++rit) {
//        std::string currLetters = this->getHand()->getLetters();
//        size_t idx = std::get<0>(rit->second);
//        char boardLetter = board->getLetter(idx);
//        currLetters += boardLetter;
//        std::sort(currLetters.begin(), currLetters.end());
//        if (sortedMap->count(currLetters)) {
//            std::cout << "BINGO: " << currLetters << std::endl;
//        }
//
//        // TODO fix the constant number 1
//        int numPlacedTile = 1;
//        std::ostringstream os;
////        // Get rid of i many characters in every possible position
////        for (int i = 1; i < NUM_PLAYER_TILES; ++i) {
//        // j refers to the index
//        std::vector<std::string> sevenLetters;
//        for (int j = 0; j < NUM_PLAYER_TILES + numPlacedTile; ++j) {
//            std::string subStrSevenLetter = currLetters.substr(0, j) + currLetters.substr(j + 1, NUM_PLAYER_TILES - j);
//            sevenLetters.push_back(subStrSevenLetter);
//            if (sortedMap->count(subStrSevenLetter)) {
//                std::cout << "7 LETTER COMBINATION: " << subStrSevenLetter << std::endl;
//            }
//        }
//        // TODO Check whether any of the 7 letter combinations doesn't include the letter placed on the board
//
//    }
}

void WordBuilder::createOrUpdateEmptyAdjacencyTiles(Angle searchingDir,
                                                    int currIdx,
                                                    int currLine,
                                                    Angle placedDirection) {
    // First, build letters in the same direction as placedDir
    // TODO: implement the one wordBeingBuilt scenario (no direction)
    std::ostringstream oss;
    BoardDir backwardDir;
    BoardDir forwardDir;
    if (searchingDir == HORIZONTAL) {
        backwardDir = LEFT;
        forwardDir = RIGHT;
    } else {
        backwardDir = TOP;
        forwardDir = BOTTOM;
    }

    // Save the original index for checking whether an AdjacentTile to be created is solvable by sortedMap
    int originalIdx = currIdx;

    // Traverse to the uppermost or leftmost, depending on the placedDir,
    // index of a placed tile
    int mostForwardIdx = currIdx;
    currIdx += backwardDir;
    // Find out the leftmost tile that is placed on the board
    while (isOnBaseLine(currIdx, currLine, searchingDir) && board->hasPlacedTile(currIdx)) {
        mostForwardIdx += backwardDir;
        currIdx += backwardDir;
    }

    // Build a string while traversing to the bottommost or rightmost index
    // of a placed tile and then create or update emptyAdjacentTile
    int totalLetterScores = 0;
    currIdx = mostForwardIdx;
    while (isOnBaseLine(currIdx, currLine, searchingDir) && board->hasPlacedTile(currIdx)) {
        totalLetterScores += board->getValue(currIdx);
        oss << (board->getLetter(currIdx));
        currIdx += forwardDir;
    }
    // TODO delete adjacentTiles (copy them first with misSolvablebysortedMap set to false)
    int mostBackwardIdx = currIdx + backwardDir;

    // Check whether the current index can produce AdjacentTiles that can be solvable by sortedMap
    // Condition 1. the Adjacent tiles to be created should be around the placed tile on the centre line
    bool isSolvableBySortedMap = false;
    if (mostForwardIdx == mostBackwardIdx) {
        if (placedDirection == HORIZONTAL) {
            if (originalIdx / BOARD_LENGTH == BOARD_LENGTH / 2)
                isSolvableBySortedMap = true;
        } else {
            if (originalIdx % BOARD_LENGTH == BOARD_LENGTH / 2)
                isSolvableBySortedMap = true;
        }
    }

    // Check further whether the AdjacentTile to be created can be solvable by sortedMap
    // Condition 2. the Adjacent tiles to be created should have no adjacentTile on the same line
    if (isSolvableBySortedMap) {
        int idxToSearch;
        int lineToSearch = getCurrLine(originalIdx, placedDirection);

        if (searchingDir == VERTICAL) {
            idxToSearch = originalIdx % BOARD_LENGTH;
        } else {
            idxToSearch = ((originalIdx / BOARD_LENGTH) + 1) * BOARD_LENGTH;
        }
        while (isOnBaseLine(idxToSearch, lineToSearch, searchingDir)) {
            // Check whether the line contains any adjacent tile
            if (adjacentTiles[idxToSearch] != nullptr) {
                isSolvableBySortedMap = false;
            }
            idxToSearch += forwardDir;
        }
    }


    // Create or update the bottommost or rightmost AdjacentTile
    if (isOnBaseLine(currIdx, currLine, searchingDir)) {
        // If the next following tile from the bottommost or rightmost has no emptyAdjacencyTile
        if (!adjacentTiles[currIdx]) {
            AdjacentTilePtr currAt = std::make_shared<AdjacentTile>(
                    totalLetterScores, oss.str(),
                    forwardDir, isSolvableBySortedMap, currIdx);
            adjacentTiles[currIdx] = currAt;
        } else {
            adjacentTiles[currIdx]->update(totalLetterScores,oss.str(),forwardDir);
        }

    }

    // Create or update the uppermost or leftmost emptyAdjacencyTile
    currIdx = mostForwardIdx + backwardDir;
    if (isOnBaseLine(currIdx, currLine, searchingDir)) {
        if (!adjacentTiles[currIdx]) {
            AdjacentTilePtr currAt = std::make_shared<AdjacentTile>(
                    totalLetterScores, oss.str(),
                    backwardDir, isSolvableBySortedMap, currIdx);
            adjacentTiles[currIdx] = currAt;
        } else {
            adjacentTiles[currIdx]->update(totalLetterScores,oss.str(),backwardDir);
        }

    }

}

int WordBuilder::getCurrLine(int idx, Angle dir) {
    int currLine;
    if (dir == HORIZONTAL) {
        currLine = idx / BOARD_LENGTH;
    } else {
        currLine = idx % BOARD_LENGTH;
    }
    return currLine;
}

bool WordBuilder::isOnBaseLine(int idx, int baseLine , Angle dir) {
    bool isOnTheSameLine;
    if (dir == HORIZONTAL) {
        isOnTheSameLine = (idx / BOARD_LENGTH == baseLine);
    } else {
        isOnTheSameLine = (idx % BOARD_LENGTH == baseLine);
    }
    return (idx >= 0) && (idx < BOARD_LENGTH * BOARD_LENGTH) && isOnTheSameLine;
}

bool WordBuilder::isOnBoard(int idx) {
    return idx >= 0 && idx < BOARD_LENGTH * BOARD_LENGTH;
}

bool WordBuilder::isPlacedTileOnTheSameLine(int idx, int baseLine, Angle dir) {
    return isOnBoard(idx) && isOnBaseLine(idx, baseLine, dir) && board->hasPlacedTile(idx);
}

bool WordBuilder::isEmptyTileOnTheSameLine(int idx, int baseLine, Angle dir) {
    return isOnBoard(idx) && isOnBaseLine(idx, baseLine, dir) && !board->hasPlacedTile(idx);
}

void WordBuilder::updateSortedMappableAdjacentTile() {
    // Go through the existing AdjacentTiles from the indices BOARD_LENGTH * (BOARD_LENGTH / 2 - 1)
    // to BOARD_LENGTH * (BOARD_LENGTH / 2 - 1) + BOARD_LENGTH
    // as well as BOARD_LENGTH * (BOARD_LENGTH / 2 + 1) to
    // BOARD_LENGTH * (BOARD_LENGTH / 2 + 1) + BOARD_LENGTH (Scan up and down) for AdjacentTiles or
    // placedTiles. If there are, set the misSolvableBySortedMap of the AdjacentTiles on the same vertical line
    // to false.
    int startIdxHorizontal = (BOARD_LENGTH / 2) * BOARD_LENGTH;
    int at1 = 0; // AdjacentTile1
    int at2 = 0; // AdjacentTile2
    int baseLine = 0;
    bool isSolvableBySortedMap;
    while (startIdxHorizontal != (BOARD_LENGTH / 2) * BOARD_LENGTH + BOARD_LENGTH) {
        isSolvableBySortedMap = true;
        if (board->hasPlacedTile(startIdxHorizontal)
            && adjacentTiles[startIdxHorizontal + TOP]
            && adjacentTiles[startIdxHorizontal + BOTTOM]) {
            baseLine = getCurrLine(startIdxHorizontal, VERTICAL);
            at1 = startIdxHorizontal + TOP + TOP;
            while (isOnBaseLine(at1, baseLine, VERTICAL)) {
                if (adjacentTiles[at1] || board->hasPlacedTile(at1))
                    isSolvableBySortedMap = false;
                at1 += TOP;
            }

            at2 = startIdxHorizontal + BOTTOM + BOTTOM;
            while (isOnBaseLine(at2, baseLine, VERTICAL)) {
                if (adjacentTiles[at2] || board->hasPlacedTile(at2))
                    isSolvableBySortedMap = false;
                at2 += BOTTOM;
            }
            if (!isSolvableBySortedMap) {
                adjacentTiles[startIdxHorizontal + TOP]->misSolvableBySortedMap = isSolvableBySortedMap;
                adjacentTiles[startIdxHorizontal + BOTTOM]->misSolvableBySortedMap = isSolvableBySortedMap;
            }
        }

        startIdxHorizontal += RIGHT;
    }

    // Repeat the same process for indices from (BOARD_LENGTH * 0 + (BOARD_LENGTH / 2 - 1))
    // to (BOARD_LENGTH * BOARD_LENGTH + (BOARD_LENGTH / 2 - 1))
    // as well as (BOARD_LENGTH * 0 + (BOARD_LENGTH / 2 + 1) to (BOARD_LENGTH * BOARD_LENGTH + (BOARD_LENGTH / 2 + 1))
    // Scan left and right to see whether there are any AdjacentTiles or placedTiles.
    for (int startIdxVertical = BOARD_LENGTH / 2;
         startIdxVertical < BOARD_LENGTH * BOARD_LENGTH;
         startIdxVertical += BOARD_LENGTH) {

        isSolvableBySortedMap = true;
        if (board->hasPlacedTile(startIdxVertical)
            && adjacentTiles[startIdxVertical + LEFT]
            && adjacentTiles[startIdxVertical + RIGHT]) {
            baseLine = getCurrLine(startIdxVertical, HORIZONTAL);
            at1 = startIdxVertical + LEFT + LEFT;
            while (isOnBaseLine(at1, baseLine, HORIZONTAL)) {
                if (adjacentTiles[at1] || board->hasPlacedTile(at1))
                    isSolvableBySortedMap = false;
                at1 += LEFT;
            }

            at2 = startIdxVertical + RIGHT + RIGHT;
            while (isOnBaseLine(at2, baseLine, HORIZONTAL)) {
                if (adjacentTiles[at2] || board->hasPlacedTile(at2))
                    isSolvableBySortedMap = false;
                at2 += RIGHT;
            }
            if (!isSolvableBySortedMap) {
                adjacentTiles[startIdxVertical + LEFT]->misSolvableBySortedMap = isSolvableBySortedMap;
                adjacentTiles[startIdxVertical + RIGHT]->misSolvableBySortedMap = isSolvableBySortedMap;
            }
        }
        startIdxHorizontal += BOTTOM;
    }
}