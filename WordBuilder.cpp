//
// Created by Myeonghoon Sun on 20/5/2022.
//

#include "WordBuilder.h"

WordBuilder::WordBuilder(const GreedyMapPtr& greedyMap,
                         const DictionaryPtr& dictionary,
                         const std::string &name,
                         size_t score,
                         LinkedListPtr<TilePtr> hand,
                         BoardPtr board):
                         Player(name, score, hand),
                         greedyMap(greedyMap),
                         board(board),
                         dictionary(dictionary),
                         wordsInQueue(std::make_shared<WordsInQueue>())
                         {}

WordBuilder::WordBuilder(const GreedyMapPtr& greedyMap,
                         const DictionaryPtr& dictionary,
                         const std::string& name,
                         BoardPtr board):
                         Player(name),
                         greedyMap(greedyMap),
                         board(board),
                         dictionary(dictionary){}

void WordBuilder::scanTheBoard() {
    if (board->isEmpty()) {
        // Place an empty AdjacentTile at the centre
        int centreIdx = BOARD_LENGTH * BOARD_LENGTH / 2;
        AdjacentTilePtr at = std::make_shared<AdjacentTile>(0, "", NA, centreIdx);
        (*adjacentTiles)[centreIdx] = at;
    } else {
        beAwareOfTiles();
    }
}

std::shared_ptr<std::map<std::string, char>> WordBuilder::getTheBestMove() {
    wordsInQueue = std::make_shared<std::priority_queue<WordPtr, std::vector<WordPtr>, CompareWord>>();
    prioritiseTiles();
    std::string lettersInHand = getHand()->getLetters();
    findWords(lettersInHand);
    std::map<int, char> chosenTileIndices;
    if (!wordsInQueue->empty())
        chosenTileIndices = wordsInQueue->top()->tileIndices;
    // TODO implement replace when no word can be made && make ai place first
    // Convert chosenTileIndices into the format Board's pre-existing function understands
    return convert(chosenTileIndices);
}


std::shared_ptr<std::map<std::string, char>> WordBuilder::convert(std::map<int, char>& tileIndices) {
    std::shared_ptr<std::map<std::string, char>> ret = nullptr;
    int rowIdx;
    int colIdx;
    std::string gridLoc;
    char letter;
    if (!tileIndices.empty()) {
        ret = std::make_shared<std::map<std::string, char>>();
        for (auto tileIndex : tileIndices) {
            rowIdx = getCurrLine(tileIndex.first, HORIZONTAL);
            colIdx = getCurrLine(tileIndex.first, VERTICAL);
            gridLoc = convertIntToRowLetter(rowIdx) + std::to_string(colIdx);
            letter = tileIndex.second;
            ret->insert(std::make_pair(gridLoc, letter));
        }
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

void WordBuilder::findWords(const std::string& lettersInHand) {
    while(!tilesToStartFrom.empty()) {
        auto at = tilesToStartFrom.top();
        Angle angles[] = {VERTICAL, HORIZONTAL};

        std::string verticalLetters;
        std::string horizontalLetters;
        std::map<WordPtr, std::tuple<bool, bool>> verticalValidLetters;
        std::map<WordPtr, std::tuple<bool, bool>> horizontalValidLetters;
        std::map<int, char> tileIndices;
        bool forwardWordExists;
        bool backwardWordExists;

        for (auto letter : lettersInHand) {
            tileIndices.clear();

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
                forwardWordExists = false;
                backwardWordExists = false;

                if (angle == VERTICAL) {
                    if (dictionary->isInDict(horizontalLetters)) {
                        if (greedyMap->forward->count(verticalLetters)) {
                            forwardWordExists = true;
                        }
                        if (greedyMap->backward->count(verticalLetters)) {
                            backwardWordExists = true;
                        }
                    }
                } else { // angle == HORIZONTAL
                    if (dictionary->isInDict(verticalLetters)) {
                        if (greedyMap->forward->count(horizontalLetters)) {
                            forwardWordExists = true;
                        }
                        if (greedyMap->backward->count(horizontalLetters)) {
                            backwardWordExists = true;
                        }
                    }
                }

                //debugging
                if (lettersInHand == "INSNCSP" && at->idx == 123)
                    std::cout << "HERE";

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
                        WordPtr newWord = std::make_shared<Word>(verticalLetters, lettersCopy, tileIndices);
                        // If the word in the perpendicular direction was more than one letter, add it to buiitWords
                        if (horizontalLetters.size() > 1) {
                            newWord->builtWords.push_back(horizontalLetters);
                        }
                        verticalValidLetters.insert(std::make_pair(newWord,wordAvailability));
                    } else {
                        WordPtr newWord = std::make_shared<Word>(horizontalLetters, lettersCopy, tileIndices);
                        if (verticalLetters.size() > 1) {
                            newWord->builtWords.push_back(verticalLetters);
                        }
                        horizontalValidLetters.insert(std::make_pair(newWord,wordAvailability));


                    }

                }
            }
        }

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
                    buildWordForwards(mostForwardIdx, mostBackwardIdx, currVerticalLine, VERTICAL, it->first);
                }
                if (isBackwardable && std::get<BACKWARDWORDEXISTS>(it->second)) {
                    buildWordBackwards(mostForwardIdx, mostBackwardIdx, currVerticalLine, VERTICAL, it->first);
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
                mostBackwardIdx += backwardDir;
            }

            // Find out mostForwardIdx
            mostForwardIdx = currIdx + forwardDir;
            // As long as on the board and there are already placed tiles
            while (isPlacedTileOnTheSameLine(mostForwardIdx, currHorizontalLine, HORIZONTAL)) {
                mostForwardIdx += forwardDir;
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
                if (isForwardable && std::get<FORWARDWORDEXISTS>(it->second))
                    buildWordForwards(mostForwardIdx, mostBackwardIdx, currHorizontalLine, HORIZONTAL, it->first);
                if (isBackwardable && std::get<BACKWARDWORDEXISTS>(it->second))
                    buildWordBackwards(mostForwardIdx, mostBackwardIdx, currHorizontalLine, HORIZONTAL, it->first);
            }
        }

        // Move over to the next adjacent tile
        tilesToStartFrom.pop();
    }
}

void WordBuilder::buildWordForwards(int forwardIdx,
                                    int backwardIdx,
                                    int currLine,
                                    Angle angle,
                                    const WordPtr& word) {
    // Base case
    // There is no more letter to place in the hand
    if (word->lettersInHand.empty())  {
    } else {
        // Recursive case

        // Set up the directions
        int forwardDir;
        if (angle == VERTICAL) {
            forwardDir = BOTTOM;
        } else {
            forwardDir = RIGHT;
        }

        int forwardIdxToSearch;
        WordPtr newWord;
        bool isForwardable;
        bool isBackwardable;

        // Check whether forwardIdx is on the same line as the original start index
        if (isOnBaseLine(forwardIdx, currLine, angle)) {
            for (const auto letter: word->lettersInHand) {
                newWord = nullptr;
                // Check whether letter can be found in the map
                if (greedyMap->forward->count(word->wordBeingBuilt) &&
                   (*greedyMap->forward)[word->wordBeingBuilt]->count(letter)) {
                    forwardIdxToSearch = forwardIdx;

                    isForwardable = false;
                    isBackwardable = false;
                    auto at = (*adjacentTiles)[forwardIdx];
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

                        // Check whether the connected letters at a perpendicular angle is in the dictionary
                        if (angle == VERTICAL && dictionary->isInDict(horizontalLetters)) {
                            if (greedyMap->forward->count(verticalLetters) || greedyMap->backward->count(verticalLetters)) {
                                newWord = std::make_shared<Word>(*word);
                                newWord->wordBeingBuilt = verticalLetters;

                                if (horizontalLetters.size() > 1)
                                    newWord->builtWords.push_back(horizontalLetters);

                                if (greedyMap->forward->count(verticalLetters))
                                    isForwardable = true;
                                if (greedyMap->backward->count(verticalLetters))
                                    isBackwardable = true;
                            }
                        }

                        // Check whether the connected letters at a perpendicular angle is in the dictionary
                        if (angle == HORIZONTAL && dictionary->isInDict(verticalLetters)) {
                            if (greedyMap->forward->count(horizontalLetters) || greedyMap->backward->count(horizontalLetters)) {
                                newWord = std::make_shared<Word>(*word);
                                newWord->wordBeingBuilt = horizontalLetters;

                                if (verticalLetters.size() > 1)
                                    newWord->builtWords.push_back(verticalLetters);

                                if (greedyMap->forward->count(horizontalLetters))
                                    isForwardable = true;
                                if (greedyMap->backward->count(horizontalLetters))
                                    isBackwardable = true;
                            }
                        }
                    } else {
                        // If the cell at the current index is an empty cell
                        newWord = std::make_shared<Word>(*word);
                        newWord->wordBeingBuilt = newWord->wordBeingBuilt + letter;
                        if (greedyMap->forward->count(newWord->wordBeingBuilt))
                            isForwardable = true;
                        if (greedyMap->backward->count(newWord->wordBeingBuilt))
                            isBackwardable = true;
                    }
                }

                // If newWord has been assigned
                if (newWord) {
                    newWord->tileIndices.insert(std::make_pair(forwardIdx, letter));
                    // Erase the letter from the lettersInHand
                    newWord->erase(letter);

                    // Traverse forwards until either an empty cell is encountered or out of the current line
                    forwardIdxToSearch += forwardDir;
                    while (isPlacedTileOnTheSameLine(forwardIdxToSearch, currLine, angle))
                        forwardIdxToSearch += forwardDir;
                    if (isBackwardable && isEmptyTileOnTheSameLine(backwardIdx, currLine, angle)) {
                        buildWordBackwards(forwardIdxToSearch, backwardIdx, currLine, angle, newWord);
                    }
                    if (isForwardable && isEmptyTileOnTheSameLine(forwardIdxToSearch, currLine, angle)) {
                        buildWordForwards(forwardIdxToSearch, backwardIdx, currLine, angle, newWord);
                    }

                    // Debugging
                    if (newWord->wordBeingBuilt == "PSI") {
                        std::cout << "here";
                    }

                    // Add a word if it exists in a dictionary
                    if (dictionary->isInDict(newWord->wordBeingBuilt))
                        wordsInQueue->push(newWord);
                }
            }
        }
    }
}

void WordBuilder::buildWordBackwards(int forwardIdx,
                                     int backwardIdx,
                                     int currLine,
                                     Angle angle,
                                     const WordPtr& word) {
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

        // TODO: get rid of the check below
        if (isOnBaseLine(backwardIdx, currLine, angle)) {
            for (const auto letter: word->lettersInHand) {
                newWord = nullptr;
                // Check whether letter can be found in the map
                if (greedyMap->backward->count(word->wordBeingBuilt) &&
                   (*greedyMap->backward)[word->wordBeingBuilt]->count(letter)) {
                    backwardIdxToSearch = backwardIdx;
                    isForwardable = false;
                    isBackwardable = false;
                    auto at = (*adjacentTiles)[backwardIdx];
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
                            if (greedyMap->forward->count(verticalLetters) ||
                                greedyMap->backward->count(verticalLetters)) {
                                newWord = std::make_shared<Word>(*word);
                                newWord->wordBeingBuilt = verticalLetters;

                                if (horizontalLetters.size() > 1)
                                    newWord->builtWords.push_back(horizontalLetters);

                                if (greedyMap->forward->count(verticalLetters))
                                    isForwardable = true;
                                if (greedyMap->backward->count(verticalLetters))
                                    isBackwardable = true;
                            }
                        }

                        // Check whether the connected letters at a perpendicular angle is in the dictionary
                        if (angle == HORIZONTAL && dictionary->isInDict(verticalLetters)) {
                            if (greedyMap->forward->count(horizontalLetters) ||
                                greedyMap->backward->count(horizontalLetters)) {
                                newWord = std::make_shared<Word>(*word);
                                newWord->wordBeingBuilt = horizontalLetters;

                                if (verticalLetters.size() > 1)
                                    newWord->builtWords.push_back(verticalLetters);

                                if (greedyMap->forward->count(horizontalLetters))
                                    isForwardable = true;
                                if (greedyMap->backward->count(horizontalLetters))
                                    isBackwardable = true;
                            }
                        }

                    } else {
                        // If the cell at the current index is an empty cell
                        newWord = std::make_shared<Word>(*word);
                        newWord->wordBeingBuilt = letter + newWord->wordBeingBuilt;
                        if (greedyMap->forward->count(newWord->wordBeingBuilt))
                            isForwardable = true;
                        if (greedyMap->backward->count(newWord->wordBeingBuilt))
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
                        buildWordBackwards(forwardIdx, backwardIdxToSearch, currLine, angle, newWord);
                    }
                    if (isForwardable && isEmptyTileOnTheSameLine(forwardIdx, currLine, angle)) {
                        buildWordForwards(forwardIdx, backwardIdxToSearch, currLine, angle, newWord);
                    }

                    // Debugging
                    if (newWord->wordBeingBuilt == "PSI") {
                        std::cout << "here";
                    }


                    // Add a word if it exists in a dictionary
                    if (dictionary->isInDict(newWord->wordBeingBuilt))
                        wordsInQueue->push(newWord);

                }

            }
        }
    }
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


        createOrUpdateEmptyAdjacencyTiles(multipleSearchDir, currIdx, currLine, placedDir);
        // Destroy the AdjacentTile if found on the placed indices
        (*adjacentTiles)[currIdx] = nullptr;
        placedIndices->pop();
    }
}

void WordBuilder::prioritiseTiles() {
    // Clear tilesToStartFrom before putting
    tilesToStartFrom = TilesToStartFrom();
    for (auto adjacentTile : *adjacentTiles) {
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
 * The algorithm first looks up connected letters in forwardMap if they are placed to the right or bottom
 * of an AdjacentTile and iterates each letter in the hand to see a wordBeingBuilt can be made with any of the letters in the hand
 * in front of the existing connected letters. If they are placed to the left or top of an AdjacentTile, backwardMap
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
 */


// The leftmost or uppermost index among the tiles placed at the current turn is passed as currIdx
void WordBuilder::createOrUpdateEmptyAdjacencyTiles(Angle searchingDir,
                                                    int currIdx,
                                                    int currLine,
                                                    Angle placedDirection) {
    // First, build letters in the same direction as placedDir
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

    int mostBackwardIdx = currIdx;

    // Since the tile could have been placed next to an existing tile,
    // Traverse backwards once to check whether another placed tile is connected to the one just placed
    mostBackwardIdx += backwardDir;
    // Find out the leftmost or uppermost tile that is placed on the board on the same row or column
    while (isPlacedTileOnTheSameLine(mostBackwardIdx, currLine, searchingDir)) {
        mostBackwardIdx += backwardDir;
    }

    // After the while loop above, mostBackwardIdx lands on a spot one index further back
    // Pull it back with the line below so that a string can be built by traversing to the
    // bottommost or rightmost index of a placed tile and then create or update emptyAdjacentTile
    currIdx = mostBackwardIdx + forwardDir;
    int totalLetterScores = 0;
    while (isPlacedTileOnTheSameLine(currIdx, currLine, searchingDir)) {
        totalLetterScores += board->getValue(currIdx);
        oss << (board->getLetter(currIdx));
        currIdx += forwardDir;
    }

    // currIdx now refers to the index on which bottommost or rightmost placed tile is found or off the line.
    // Assign mostFowardIdx with currIdx for readability.
    int mostForwardIdx = currIdx;
    // Create or update the bottommost or rightmost AdjacentTile
    if (isOnBaseLine(mostForwardIdx, currLine, searchingDir)) {
        // If the next following tile from the bottommost or rightmost has no AdjacentTile
        if (!(*adjacentTiles)[mostForwardIdx]) {
            AdjacentTilePtr currAt = std::make_shared<AdjacentTile>(
                    totalLetterScores, oss.str(),
                    forwardDir, mostForwardIdx);
            (*adjacentTiles)[mostForwardIdx] = currAt;
        } else {
            (*adjacentTiles)[mostForwardIdx]->update(totalLetterScores, oss.str(), forwardDir);
        }
    }

    // Create or update the uppermost or leftmost emptyAdjacencyTile
    if (isOnBaseLine(mostBackwardIdx, currLine, searchingDir)) {
        if (!(*adjacentTiles)[mostBackwardIdx]) {
            AdjacentTilePtr currAt = std::make_shared<AdjacentTile>(
                    totalLetterScores, oss.str(),
                    backwardDir, mostBackwardIdx);
            (*adjacentTiles)[mostBackwardIdx] = currAt;
        } else {
            (*adjacentTiles)[mostBackwardIdx]->update(totalLetterScores, oss.str(), backwardDir);
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

void WordBuilder::setGreedyMap(GreedyMapPtr greedyMap) {
    this->greedyMap = greedyMap;
}

void WordBuilder::setBoard(BoardPtr board) {
    this->board = board;
}

void WordBuilder::setDictionary(DictionaryPtr dictionary) {
    this->dictionary = dictionary;
}

void WordBuilder::setAdjacentTiles(AdjacentTilesPtr adjacentTiles) {
    this->adjacentTiles = adjacentTiles;
}

void WordBuilder::giveHint(const LinkedListPtr<TilePtr>& hand) {
    wordsInQueue = std::make_shared<std::priority_queue<WordPtr, std::vector<WordPtr>, CompareWord>>();
    prioritiseTiles();
    findWords(hand->getLetters());
    auto chosenTileIndices = wordsInQueue->top()->tileIndices;
    for (auto tileIndex: chosenTileIndices) {
        std::cout << tileIndex.second << " ";
    }
    std::cout << "(you may need to combine with another letter already placed on the board): " << std::endl;
}


/*
 * The code below is for the first tried algorithm using a sorted map to quickly find whether there is any word
 * that can be made given the letters. Therefore, please disregard the section below. It's for my own reference
 * in the future.
 *
 *
 *
 *
 *
 *
 * // Check whether the current index can produce AdjacentTiles that can be solvable by sortedMap
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

 *
 *         std::string updatedLetters;
        if (at->misSolvableBySortedMap) {
            updatedLetters = lettersInHand;
            // If the AdjacentTile is solvable by sortedMap, add the letter to letters
            for (auto letter : at->adjacentLetters) {
                updatedLetters += letter;
            }
            // Sort the combined letters
            std::sort(updatedLetters.begin(), updatedLetters.end());

            // Find whether the combined letters exist in sortedMap
            if (sortedMap->count(updatedLetters)) {
                std::cout << "Letters Enquired: " << updatedLetters << std::endl;
                for (auto wordBeingBuilt: *(*sortedMap)[updatedLetters]) {
                    std::cout << wordBeingBuilt << std::endl;
                }
            }
        } else {
            // Greedy Algo goes here
        }
 *
 *
 */

//void WordBuilder::updateSortedMappableAdjacentTile() {
//    // Go through the existing AdjacentTiles from the indices BOARD_LENGTH * (BOARD_LENGTH / 2 - 1)
//    // to BOARD_LENGTH * (BOARD_LENGTH / 2 - 1) + BOARD_LENGTH
//    // as well as BOARD_LENGTH * (BOARD_LENGTH / 2 + 1) to
//    // BOARD_LENGTH * (BOARD_LENGTH / 2 + 1) + BOARD_LENGTH (Scan up and down) for AdjacentTiles or
//    // placedTiles. If there are, set the misSolvableBySortedMap of the AdjacentTiles on the same vertical line
//    // to false.
//    int startIdxHorizontal = (BOARD_LENGTH / 2) * BOARD_LENGTH;
//    int at1 = 0; // AdjacentTile1
//    int at2 = 0; // AdjacentTile2
//    int baseLine = 0;
//    bool isSolvableBySortedMap;
//    while (startIdxHorizontal != (BOARD_LENGTH / 2) * BOARD_LENGTH + BOARD_LENGTH) {
//        isSolvableBySortedMap = true;
//        if (board->hasPlacedTile(startIdxHorizontal)
//            && adjacentTiles[startIdxHorizontal + TOP]
//            && adjacentTiles[startIdxHorizontal + BOTTOM]) {
//            baseLine = getCurrLine(startIdxHorizontal, VERTICAL);
//            at1 = startIdxHorizontal + TOP + TOP;
//            while (isOnBaseLine(at1, baseLine, VERTICAL)) {
//                if (adjacentTiles[at1] || board->hasPlacedTile(at1))
//                    isSolvableBySortedMap = false;
//                at1 += TOP;
//            }
//
//            at2 = startIdxHorizontal + BOTTOM + BOTTOM;
//            while (isOnBaseLine(at2, baseLine, VERTICAL)) {
//                if (adjacentTiles[at2] || board->hasPlacedTile(at2))
//                    isSolvableBySortedMap = false;
//                at2 += BOTTOM;
//            }
//            if (!isSolvableBySortedMap) {
//                adjacentTiles[startIdxHorizontal + TOP]->misSolvableBySortedMap = isSolvableBySortedMap;
//                adjacentTiles[startIdxHorizontal + BOTTOM]->misSolvableBySortedMap = isSolvableBySortedMap;
//            }
//        }
//
//        startIdxHorizontal += RIGHT;
//    }
//
//    // Repeat the same process for indices from (BOARD_LENGTH * 0 + (BOARD_LENGTH / 2 - 1))
//    // to (BOARD_LENGTH * BOARD_LENGTH + (BOARD_LENGTH / 2 - 1))
//    // as well as (BOARD_LENGTH * 0 + (BOARD_LENGTH / 2 + 1) to (BOARD_LENGTH * BOARD_LENGTH + (BOARD_LENGTH / 2 + 1))
//    // Scan left and right to see whether there are any AdjacentTiles or placedTiles.
//    for (int startIdxVertical = BOARD_LENGTH / 2;
//         startIdxVertical < BOARD_LENGTH * BOARD_LENGTH;
//         startIdxVertical += BOARD_LENGTH) {
//
//        isSolvableBySortedMap = true;
//        if (board->hasPlacedTile(startIdxVertical)
//            && adjacentTiles[startIdxVertical + LEFT]
//            && adjacentTiles[startIdxVertical + RIGHT]) {
//            baseLine = getCurrLine(startIdxVertical, HORIZONTAL);
//            at1 = startIdxVertical + LEFT + LEFT;
//            while (isOnBaseLine(at1, baseLine, HORIZONTAL)) {
//                if (adjacentTiles[at1] || board->hasPlacedTile(at1))
//                    isSolvableBySortedMap = false;
//                at1 += LEFT;
//            }
//
//            at2 = startIdxVertical + RIGHT + RIGHT;
//            while (isOnBaseLine(at2, baseLine, HORIZONTAL)) {
//                if (adjacentTiles[at2] || board->hasPlacedTile(at2))
//                    isSolvableBySortedMap = false;
//                at2 += RIGHT;
//            }
//            if (!isSolvableBySortedMap) {
//                adjacentTiles[startIdxVertical + LEFT]->misSolvableBySortedMap = isSolvableBySortedMap;
//                adjacentTiles[startIdxVertical + RIGHT]->misSolvableBySortedMap = isSolvableBySortedMap;
//            }
//        }
//        startIdxHorizontal += BOTTOM;
//    }
//}
// void WordBuilder::readFileToMap(const std::string &fileName, sortedMapPtr& mainMap) {
//    std::string line;
//    std::string mainKey;
//    std::string originalWord;
//
//    std::ifstream in(fileName);
//    // Read a file as long as there is a line to read
//    while (std::getline(in, line)) {
//        // Use istringstream to work with individual wordsInQueue on a line
//        std::istringstream iss(line);
//        iss >> mainKey;
//        originalWordsPtr originalWords = std::make_shared<originalWordsType>();
//        while (iss >> originalWord) {
//            originalWords->insert(originalWord);
//        }
//        // Build a mainMap
//        mainMap->insert(std::make_pair(mainKey, originalWords));
//    }
//    in.close();
//}