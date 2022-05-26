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
        wordsInQueue(std::make_shared<std::priority_queue<WordPtr, std::vector<WordPtr>, CompareWord>>()),
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
        wordsInQueue(std::make_shared<std::priority_queue<WordPtr, std::vector<WordPtr>, CompareWord>>()),
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

void WordBuilder::execute() {
    updateSortedMappableAdjacentTile();
    beAwareOfTiles();
    convertTilesIntoPQ();
    suggestTiles();
    // TODO check how the priority queue is ordered
}

void WordBuilder::suggestTiles() {
    // Get all the letters from the hand
    std::string letters = getHand()->getLetters();
    std::string updatedLetters;

    while(!tilesToStartFrom.empty()) {
        auto at = tilesToStartFrom.top();
        // recursivelyBuildWord



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
//                for (auto word: *(*sortedMap)[updatedLetters]) {
//                    std::cout << word << std::endl;
//                }
//            }
//        } else {
//            // Greedy Algo goes here
//        }





        // Implement the greedy algorithm
        // First find out in which direction the given AdjacentTile is connected to other letters
        std::vector<int> directionsToSearch;
        for (int idx = UPWARD; idx < TOTALDIRECTIONS; ++idx) {
            // If the string stored at the index is not empty
            if (!at->adjacentLetters[idx].empty())
                directionsToSearch.push_back(idx);
        }

        // TODO decide on whether letters need to be turned into a set so each unique letter is checked
        // First, given the AdjacentTile, try all the letters in the hand to see whether adjacent letters can be extended
        std::string partialWord;
        greedyMapPtr greedyMap;
        // the number of combined letters, in a dictionary, with each of the extendible letters mapped to itself
        std::map<int, char> extendibleLetters;


        for (const auto &ch : letters) {
            bool isExtendible = true;
            for (const int dir : directionsToSearch) {
                partialWord = at->adjacentLetters[dir];
                // Decide on which map to use to look up whether a partial combination of letters exists
                if (dir == UPWARD || dir == LEFTWARD) {
                    // If the previous dir has not set isExtendible to false
                    if (isExtendible) {
                        greedyMap = greedyForwardMap;
                        // TODO see if std::string::reserve is necessary

                        // If partialWord key is not found in greedyMap
                        if (!greedyMap->count(partialWord)) {
                            // TODO see whether it is necessary to set isExtendible of AdjacentTile to false
                            isExtendible = false;
                        } else if (!(*greedyMap)[partialWord]->count(ch)){
                            // If partialWord key is found but the given letter in the hand is not found associated with it
                            isExtendible = false;
                        }
                    }
                } else {
                    // If the previous dir has not set isExtendible to false
                    if (isExtendible) {
                        greedyMap = greedyBackwardMap;
                        // Prepend a letter
                        // TODO: Check whether partialWord exists first
                        if (!greedyMap->count(partialWord)) {
                            isExtendible = false;
                        } else if (!(*greedyMap)[partialWord]->count(ch)){
                            // If partialWord key is found but the given letter in the hand is not found associated with it
                            isExtendible = false;
                        }
                    }
                }
            }

            // If the letter can extend all the connected wordsInQueue, store it and the number of wordsInQueue that can be made from the extension
            if (isExtendible) {
                extendibleLetters.insert(std::make_pair((*(*greedyMap)[partialWord])[ch], ch));
            }
        }

        // Find out the most backward index and forward index connected to the AdjacentIdx
        // and then build wordsInQueue
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
        std::map<int, char> tileIndices;
        Angle angles[] = {VERTICAL, HORIZONTAL};
        std::priority_queue<WordPtr> words;
        if (!extendibleLetters.empty()) {
            // TODO refactor the code into one
            currIdx = at->idx;
            currVerticalLine = getCurrLine(currIdx, VERTICAL);
            currHorizontalLine = getCurrLine(currIdx, HORIZONTAL);

            for (auto angle: angles) {
                mostBackwardIdx = currIdx;
                mostForwardIdx = currIdx;
                // Insert a temporary value \0 so that mostBackwardIdx and forwardIdx can be set correctly.
                tileIndices.insert(std::make_pair(currIdx, '\0'));
                if (angle == VERTICAL) {
                    backwardDir = TOP;
                    forwardDir = BOTTOM;

                    // Find out mostBackwardIdx first
                    mostBackwardIdx = currIdx + backwardDir;
                    // As long as on the board and there are already placed tiles
                    while (isOnBaseLine(mostBackwardIdx, currVerticalLine, VERTICAL) && board->hasPlacedTile(mostBackwardIdx)) {
                        mostBackwardIdx += backwardDir;
                    }

                    // Find out mostForwardIdx
                    mostForwardIdx = currIdx + forwardDir;
                    // As long as on the board and there are already placed tiles
                    while (isOnBaseLine(mostForwardIdx, currVerticalLine, VERTICAL) && board->hasPlacedTile(mostForwardIdx)) {
                        mostForwardIdx += forwardDir;
                    }

                    // If mostBackwardIdx is the same as currIdx, go backwards again (taking into account the letter to be placed)
                    if (mostBackwardIdx == currIdx)
                        mostBackwardIdx += backwardDir;
                    // If mostForwardIdx is the same as currIdx, go forwards again (taking into account the letter to be placed)
                    if (mostForwardIdx == currIdx)
                        mostForwardIdx += forwardDir;

                    // Build wordsInQueue
                    std::string tempLetters;
                    for (auto it = extendibleLetters.rbegin(); it != extendibleLetters.rend(); ++it) {

                        // Clear the contents of tileIndices every time a new letter is tried
                        tileIndices.clear();
                        tempWords.clear();
                        tileIndices[currIdx] = it->second;
                        // Make the currently connected letters into strings
                        verticalWord = at->adjacentLetters[UPWARD] + it->second + at->adjacentLetters[DOWNWARD];
                        horizontalWord = at->adjacentLetters[LEFTWARD] + it->second + at->adjacentLetters[RIGHTWARD];
                        tempWords.push_back(verticalWord);
                        tempWords.push_back(horizontalWord);
                        // See whether they exist in a dictionary
                        if (dictionary->isInDict(tempWords)) {
                            wordsInQueue->push(std::make_shared<Word>(tempWords, tileIndices));
                        }
                        // Reset the tempLetters every iteration
                        tempLetters = letters;
                        auto charPos = std::find(tempLetters.begin(), tempLetters.end(), it->second);
                        tempLetters.erase(charPos);
                        recursivelyBuildWord(mostBackwardIdx, mostForwardIdx, currVerticalLine, VERTICAL, verticalWord, tempLetters, false, tileIndices);
                    }

                } else {
                    backwardDir = LEFT;
                    forwardDir = RIGHT;

                    // Find out mostBackwardIdx first
                    mostBackwardIdx = currIdx + backwardDir;
                    // As long as on the board and there are already placed tiles
                    while (isOnBaseLine(mostBackwardIdx, currHorizontalLine, HORIZONTAL) &&
                           board->hasPlacedTile(mostBackwardIdx)) {
                        mostBackwardIdx = currIdx + backwardDir;
                    }

                    // Find out mostForwardIdx
                    mostForwardIdx = currIdx + forwardDir;
                    // As long as on the board and there are already placed tiles
                    while (isOnBaseLine(mostForwardIdx, currHorizontalLine, HORIZONTAL) &&
                           board->hasPlacedTile(mostForwardIdx)) {
                        mostForwardIdx = currIdx + forwardDir;
                    }

                    // If mostBackwardIdx is the same as currIdx, go backwards again (taking into account the letter to be placed)
                    if (mostBackwardIdx == currIdx)
                        mostBackwardIdx += backwardDir;
                    // If mostForwardIdx is the same as currIdx, go forwards again (taking into account the letter to be placed)
                    if (mostForwardIdx == currIdx)
                        mostForwardIdx += forwardDir;

                    // Build wordsInQueue
                    std::string tempLetters;
                    for (auto it = extendibleLetters.rbegin(); it != extendibleLetters.rend(); ++it) {
                        // Clear the contents of tileIndices every time a new letter is tried
                        tileIndices.clear();
                        tileIndices[currIdx] = it->second;

                        // Reset the tempLetters every iteration
                        tempLetters = letters;
                        auto charPos = std::find(tempLetters.begin(), tempLetters.end(), it->second);
                        tempLetters.erase(charPos);

                        recursivelyBuildWord(mostBackwardIdx, mostForwardIdx, currHorizontalLine, HORIZONTAL, horizontalWord, tempLetters, false, tileIndices);
                    }
                }
            }
        }






//        std::string verticalWord;
//        std::string horizontalWord;
//
//        for (auto it = extendibleLetters.rbegin(); it != extendibleLetters.rend(); ++it) {
//            mostBackwardIdx = currIdx;
//            mostForwardIdx = currIdx;
//            // Build wordsInQueue vertically and horizontally
//            for (auto angle: angles) {
//                if (angle == VERTICAL) {
//                    backwardDir = TOP;
//                    forwardDir = BOTTOM;
//
//                    mostBackwardIdx = currIdx + backwardDir;
//                    // Scenario 1: Run into a tile placed
//                    // Scenario 2: Run into an AdjacentTile
//                    // Scenario 3: Run into an empty cell
//                    // As long as on the board
//                    while (isOnBaseLine(mostBackwardIdx, currVerticalLine, VERTICAL)) {
//
//                        mostBackwardIdx = currIdx + backwardDir;
//                    }
//                } else {
//                    backwardDir = LEFT;
//                    forwardDir = RIGHT;
//                    tempWord = at->adjacentLetters[LEFTWARD] + it->second + at->adjacentLetters[RIGHTWARD];
//                }
//            }
//
//        }
//
//        // Now, decide which direction to build a word. Make the algorithm favour the direction that has the greatest number of wordsInQueue
//        // Additionally, do not traverse in the direction of an AdjacentTile whose misSolvableBySortedMap set to True.
//        BoardDir dir;
//        int currIdx;
//        int currVerticalLine = getCurrLine(currIdx, VERTICAL);
//        int currHorizontalLine = getCurrLine(currIdx, HORIZONTAL);
//        int currLine;
//        Angle travelAngle;
//        std::map<size_t, BoardDir> directionsToStartFrom;
//        // Iterate TOP, RIGHT, BOTTOM, and LEFT
//        for (size_t i = 0; i < TOTALDIRECTIONS; ++i) {
//            currIdx = at->idx;
//            dir = boardDirections[i];
//            // If the direction is either top or bottom, the travel angle is vertical
//            if (dir == TOP || dir == BOTTOM) {
//                currLine = currVerticalLine;
//                travelAngle = VERTICAL;
//            } else {
//                // If the direction is either left or right, the travel angle is horizontal
//                currLine = currHorizontalLine;
//                travelAngle = HORIZONTAL;
//            }
//
//            // While travelling on the same line in the same direction
//            // TODO: find a direction away from AdjacentTiles solvable by sortedMap
//            for (auto it = extendibleLetters.rbegin(); it != extendibleLetters.rend(); ++it) {
//                // Set partialWord to the string stored in the opposite direction to the direction to be traversed
//                partialWord = at->adjacentLetters[adjacentTileDirection[i]];
//            }
//            currIdx += dir;
//            while (isOnBaseLine(currIdx, currLine, travelAngle) && !sortedMapATExists) {
//                if (adjacentTiles[currIdx]) {
//
//                }
//                currIdx += dir;
//            }
//
//
////             Iterate extendibleLetters and compare each of it to the rest
//
//
//
//
//        }
//
//        size_t numAdjacentTiles = 0;
//        BoardDir dir = TOP;
//



        // Move over to the next adjacent tile
        tilesToStartFrom.pop();
    }






}

void WordBuilder::recursivelyBuildWord(
                             int backwardIdx,
                             int forwardIdx,
                             int currLine,
                             Angle angle,
                             std::string& tempLetters,
                             std::string& lettersInHand,
                             bool noLetterPlaceable,
                             std::map<int, char>& tilesIndices) {

    // Set up the directions
    int forwardDir;
    int backwardDir;
    if (angle == VERTICAL) {
        backwardDir = TOP;
        forwardDir = BOTTOM;
    } else {
        backwardDir = LEFT;
        forwardDir = RIGHT;
    }

    // Base case:
    // 1. If the index is not on the same line
    // OR
    // 2. If no letter can be placed
    // OR
    // 3. If no letter is left in the hand
    if ((!isOnBaseLine(backwardDir, currLine, angle) && !isOnBaseLine(forwardDir, currLine, angle))
        || noLetterPlaceable || lettersInHand.empty()) {

    } else {
        // Recursive case:
        // 1. Compare the two indices with respect to the number of wordsInQueue that can be extended given all the tiles in the hand.
        // 2. Choose the letter with the highest number of counts from either index
        // 3. Add the letter to a string
        bool isForwardable = true;
        bool isBackwardable = true;
        std::map<int, char> forwardMap;
        std::map<int, char> backwardMap;
        while (isForwardable && isBackwardable) {
            // Check the lettersInHand
            if (isOnBaseLine(backwardIdx, currLine, angle)) {
                // If run into an AdjacentTile
                std::string verticalLetters = "";
                std::string horizontalLetters = "";
                for (const auto letter: lettersInHand) {
                    if (adjacentTiles[backwardIdx]) {
                        if (angle == VERTICAL) {
                            // If going vertical, the backward direction is top, which means the adjacentTile can have at most three elements
                            if (!adjacentTiles[backwardIdx]->adjacentLetters[UPWARD].empty())
                                verticalLetters = adjacentTiles[backwardIdx]->adjacentLetters[UPWARD] + letter + tempLetters;
                            if (!adjacentTiles[backwardIdx]->adjacentLetters[LEFTWARD].empty())
                                horizontalLetters = adjacentTiles[backwardIdx]->adjacentLetters[LEFTWARD] + letter;
                            if (!adjacentTiles[backwardIdx]->adjacentLetters[RIGHTWARD].empty())
                                horizontalLetters = horizontalLetters + adjacentTiles[backwardIdx]->adjacentLetters[RIGHTWARD];
                            // Check if word(s) can be made
                            std::vector<std::string> tempWords;
                            tempWords.push_back(verticalLetters);
                            tempWords.push_back(horizontalLetters);
                            if (dictionary->isInDict(tempWords)) {
                                tilesIndices.insert(std::make_pair(backwardIdx, letter));
                                wordsInQueue->push(std::make_shared<Word>(tempWords, tilesIndices));
                            }
                        } else {
                            // If going horizontal, the backward direction is left, which means the adjacentTile can have at most three elements
                            if (!adjacentTiles[backwardIdx]->adjacentLetters[UPWARD].empty())
                                verticalLetters = adjacentTiles[backwardIdx]->adjacentLetters[UPWARD];
                            if (!adjacentTiles[backwardIdx]->adjacentLetters[DOWNWARD].empty())
                                horizontalLetters = adjacentTiles[backwardIdx]->adjacentLetters[DOWNWARD];
                            if (!adjacentTiles[backwardIdx]->adjacentLetters[LEFTWARD].empty())
                                horizontalLetters = adjacentTiles[backwardIdx]->adjacentLetters[LEFTWARD];
                            // Check if word(s) can be made
                            std::vector<std::string> tempWords;
                            tempWords.push_back(verticalLetters);
                            tempWords.push_back(horizontalLetters);
                            if (dictionary->isInDict(tempWords)) {
                                tilesIndices.insert(std::make_pair(backwardIdx, letter));
                                wordsInQueue->push(std::make_shared<Word>(tempWords, tilesIndices));
                            }
                        }
                    } else {
                        // Compare each of the letters in the backward greedy Dictionary
                        if (!board->hasPlacedTile(backwardIdx)) {

                        } else {
                            isBackwardable = false;
                        }
                    }




                    // If the AdjacentTile has lettersInHand upwards
                    for (auto letters: adjacentTiles[backwardIdx]->adjacentLetters) {
                        for (const auto letter: letters) {
                            if (!greedyBackwardMap->count(letter + letters)) {
                                isBackwardable = false;
                            }
                        }
                    }

                }
            }
        }

    }




}


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

void WordBuilder::convertTilesIntoPQ() {
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
 * Second, if the check above does not turn up any valid word, the greedy algorithm is used.
 * As all the AdjacentTiles are stored in a priority queue with the potentialScore data member as a key,
 * the one with the highest potential score can be looked at first.
 *
 * If the AdjacentTile has only one placed tile connected to it, the algorithm is first made to go in
 * the perpendicular direction to generate a long word.
 *
 * The algorithm first looks up connected letters in forwardGreedyMap if they are placed to the right or bottom
 * of an AdjacentTile and iterates each letter in the hand to see a word can be made with any of the letters in the hand
 * in front of the existing connected letters. If they are placed to the left or top of an AdjacentTile, backwardGreedyMap
 * is accessed to find whether the existing connected letters can be extended by any of the letters in the hand.
 *
 * If not, its data member misExtendible is set to false and its potentialScores is also set to 0.
 * Otherwise, the aggregate number of wordsInQueue that can be made with any of the letters in the hand is stored.
 * The number of wordsInQueue is then compared to that of an AdjacentTile on its opposite end to see which presents
 * higher chances of being made into a word
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
    // TODO: implement the one word scenario (no direction)
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

bool WordBuilder::isLineOnBoard(int line) {
    return line >= 0 || line <= BOARD_LENGTH;
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