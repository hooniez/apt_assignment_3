//
// Created by Myeonghoon Sun on 20/5/2022.
//

#include <chrono>

#include "WordBuilder.h"
WordBuilder::WordBuilder(const std::string &forwardSearchMapFileName,
                         const std::string &backwardSearchMapFileName,
                         const std::string& sortedMapFileName,
                         BoardPtr board,
                         bool canGiveHints):
                         canGiveHints(canGiveHints),
                         isPlaying(false),
                         board(board),
                         tilesToStartFrom(std::make_shared<EmptyAdjacentTilesType>())
//                         :Player() {
{
    std::cout << "Setting Up AI (it takes an average of 10 seconds)" << std::endl;
    initialiseMaps();
    std::cout << "Set-up Completed" << std::endl;
    std::fill_n(adjacentTiles, BOARD_LENGTH * BOARD_LENGTH, nullptr);
}

WordBuilder::WordBuilder(const std::string &forwardSearchMapFileName,
                         const std::string &backwardSearchMapFileName,
                         const std::string& sortedMapFileName,
                         const std::string &name,
                         BoardPtr board,
                         bool canGiveHints):
                         Player(name),
                         canGiveHints(canGiveHints),
                         isPlaying(true),
                         board(board),
                         tilesToStartFrom(std::make_shared<EmptyAdjacentTilesType>()) {
//    std::cout << "Setting Up AI (it takes an average of 10 seconds)" << std::endl;
//    auto start = std::chrono::high_resolution_clock::now();
    initialiseMaps();
//    auto stop = std::chrono::high_resolution_clock::now();
//    std::cout << "Set-up Completed" << std::endl;
//    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop -start);
//    std::cout << duration.count() / 1000000 << " seconds taken" << std::endl;
}

void WordBuilder::readFileToMap(const std::string &fileName, greedyMapPtr& mainMap) {

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

void WordBuilder::readFileToMap(const std::string &fileName, sortedMapPtr& mainMap) {

    std::string line;
    std::string mainKey;
    std::string originalWord;

    std::ifstream in(fileName);
    // Read a file as long as there is a line to read
    while (std::getline(in, line)) {
        // Use istringstream to work with individual words on a line
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
    forwardMap = std::make_shared<greedyMapType>();
    forwardMap->reserve(NUM_ELEMENTS_IN_GREEDY_MAP);
    readFileToMap("forwardGreedyMap", forwardMap);
    backwardMap = std::make_shared<greedyMapType>();
    backwardMap->reserve(NUM_ELEMENTS_IN_GREEDY_MAP);
    readFileToMap("backwardGreedyMap", backwardMap);
    sortedMap = std::make_shared<sortedMapType>();
    sortedMap->reserve(NUM_ELEMENTS_IN_SORTED_MAP);
    readFileToMap("sortedMap", sortedMap);
}

void WordBuilder::execute() {
    beAwareOfTiles();
    placeTiles();


}


void WordBuilder::beAwareOfTiles() {
    auto placedDir = board->getPlacedDir();
    Direction singleSearchDir;
    Direction multipleSearchDir;
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
            adjacentTiles[currIdx]->hasPlacedTile();
        } else {
            createOrUpdateEmptyAdjacencyTiles(multipleSearchDir, currIdx, currLine, placedDir);
        }
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
 * find 7 or 8 letter-long words.
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
 * Otherwise, the aggregate number of words that can be made with any of the letters in the hand is stored.
 * The number of words is then compared to that of an AdjacentTile on its opposite end to see which presents
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
//        std::array<DirectionFromPlacedTile, TOTALDIRECTIONS / 2> directions;
//        if (placedDir == HORIZONTAL) {
//            directions = verticlDirections;
//        } else if (placedDir == VERTICAL) {
//            directions = horizontalDirections;
//        }
//
//        for (DirectionFromPlacedTile direction: directions) {
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
//            for (DirectionFromPlacedTile direction : directions) {
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

bool WordBuilder::isOnBaseLine(int idx, int baseLine , Direction dir) {
    bool isOnTheSameLine;
    if (dir == HORIZONTAL) {
        isOnTheSameLine = (idx / BOARD_LENGTH == baseLine);
    } else {
        isOnTheSameLine = (idx % BOARD_LENGTH == baseLine);
    }
    return (idx >= 0) && (idx < BOARD_LENGTH * BOARD_LENGTH) && isOnTheSameLine;
}

void WordBuilder::createOrUpdateEmptyAdjacencyTiles(Direction searchingDir,
                                                    int currIdx,
                                                    int currLine,
                                                    Direction placedDirection) {
    // First, build letters in the same direction as placedDir
    // TODO: implement the one word scenario (no direction)
    std::ostringstream oss;
    DirectionFromPlacedTile backwardDir;
    DirectionFromPlacedTile forwardDir;
    if (searchingDir == HORIZONTAL) {
        backwardDir = LEFT;
        forwardDir = RIGHT;
    } else {
        backwardDir = TOP;
        forwardDir = BOTTOM;
    }



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

    // Check whether the current index can produce AdjacentTiles that can be solvable by sortedMap
    // Condition 1. the Adjacent tiles to be created should be around the placed tile on the centre line
    bool isSolvableBySortedMap = false;
    if (mostForwardIdx == currIdx) {
        if (placedDirection == HORIZONTAL) {
            if (currIdx / BOARD_LENGTH == BOARD_LENGTH / 2)
                isSolvableBySortedMap = true;
        } else {
            if (currIdx % BOARD_LENGTH == BOARD_LENGTH / 2)
                isSolvableBySortedMap = true;
        }
    }

    // Create or update the bottommost or rightmost AdjacentTile
    if (isOnBaseLine(currIdx, currLine, searchingDir)) {
        // If the next following tile from the bottommost or rightmost has no emptyAdjacencyTile
        if (!adjacentTiles[currIdx]) {
            // Check further whether the AdjacentTile to be created can be solvable by sortedMap
            // Condition 2. the Adjacent tiles to be created should have no placed tiles other than the one they are adjacent to
            int nextIdx = currIdx + forwardDir;
            while (isOnBaseLine(nextIdx, currLine, searchingDir)) {
                if (board->hasPlacedTile(nextIdx)) {
                    isSolvableBySortedMap = false;
                }
                nextIdx = currIdx + forwardDir;
            }

            AdjacentTilePtr currAt = std::make_shared<AdjacentTile>(
                    totalLetterScores, oss.str(),
                    forwardDir, isSolvableBySortedMap, currIdx);
            adjacentTiles[currIdx] = currAt;
            tilesToStartFrom->insert(currAt);
        } else {
            adjacentTiles[currIdx]->update(totalLetterScores, oss.str(), forwardDir);
        }
    }




    // Create or update the uppermost or leftmost emptyAdjacencyTile
    // TODO refactor the repeated code below as above
    currIdx = mostForwardIdx + backwardDir;
    if (isOnBaseLine(currIdx, currLine, searchingDir)) {
        if (!adjacentTiles[currIdx]) {
            AdjacentTilePtr currEat = std::make_shared<AdjacentTile>(
                    totalLetterScores, oss.str(),
                    backwardDir, currIdx);
            adjacentTiles[currIdx] = currEat;
            tilesToStartFrom->insert(currEat);
        } else {
            adjacentTiles[currIdx]->update(totalLetterScores, oss.str(), backwardDir);
        }
    }







}

int WordBuilder::getCurrLine(int idx, Direction dir) {
    int currLine;
    if (dir == HORIZONTAL) {
        currLine = idx / BOARD_LENGTH;
    } else {
        currLine = idx % BOARD_LENGTH;
    }
    return currLine;
}