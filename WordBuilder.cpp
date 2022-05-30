//
// Created by Myeonghoon Sun on 20/5/2022.
//

#include "WordBuilder.h"

#include <utility>

WordBuilder::WordBuilder(GreedyMapPtr  greedyMap,
                         DictionaryPtr  dictionary,
                         const std::string &name,
                         int score,
                         LinkedListPtr<TilePtr> hand,
                         BoardPtr board):
        Player(name, score, std::move(hand)),
        greedyMap(std::move(greedyMap)),
        board(std::move(board)),
        dictionary(std::move(dictionary)),
        completeWords(std::make_shared<CompleteWords>()) {}

WordBuilder::WordBuilder(GreedyMapPtr  greedyMap,
                         DictionaryPtr  dictionary,
                         const std::string& name,
                         BoardPtr board):
                         Player(name),
                         greedyMap(std::move(greedyMap)),
                         board(std::move(board)),
                         dictionary(std::move(dictionary)),
                         completeWords(std::make_shared<CompleteWords>()) {}

void WordBuilder::scanTheBoard() {
    if (board->isEmpty()) {
        // Place an empty AdjacentTile at the centre
        int centreIdx = BOARD_LENGTH * BOARD_LENGTH / 2;
        AdjacentTilePtr at = std::make_shared<AdjacentTile>(
                0, "", NA, centreIdx);
        (*adjacentTiles)[centreIdx] = at;
    } else {
        beAwareOfTiles();
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
    createOrUpdateAdjacentTiles(singleSearchDir,
                                currIdx,
                                currLine,
                                placedDir);

    // Next, build letters in the perpendicular direction as placedDir
    while (!placedIndices->empty()) {
        // Access the leftmost or uppermost index of those of the placed tiles
        currIdx = placedIndices->top();

        currLine = getCurrLine(currIdx, multipleSearchDir);


        createOrUpdateAdjacentTiles(multipleSearchDir,
                                    currIdx,
                                    currLine,
                                    placedDir);
        // Destroy the AdjacentTile if found on the placed indices
        (*adjacentTiles)[currIdx] = nullptr;
        placedIndices->pop();
    }
}

// The leftmost or uppermost index among the tiles placed is passed as currIdx
void WordBuilder::createOrUpdateAdjacentTiles(Angle searchingDir,
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

    /*
     * Since the tile could have been placed next to an existing tile,
     * Traverse backwards once to check whether another placed tile
     * is connected to the one just placed
     */
    mostBackwardIdx += backwardDir;

    /*
     * Find out the leftmost or uppermost tile that is placed on the board on
     * the same row or column
     */
    while (isPlacedTileOnTheSameLine(mostBackwardIdx,
                                     currLine,
                                     searchingDir)) {
        mostBackwardIdx += backwardDir;
    }

    /*
     * After the while loop above, mostBackwardIdx lands on a spot one index
     * further back. Pull it back with the line below so that a string can be
     * built by traversing to the bottommost or rightmost index of a placed
     * tile and then create or update emptyAdjacentTile
     */
    currIdx = mostBackwardIdx + forwardDir;
    int totalLetterScores = 0;
    while (isPlacedTileOnTheSameLine(currIdx,
                                     currLine,
                                     searchingDir)) {
        totalLetterScores += board->getValue(currIdx);
        oss << (board->getLetter(currIdx));
        currIdx += forwardDir;
    }

    /*
     * currIdx now refers to the index on which bottommost or rightmost
     * placed tile is found or off the line.
     * Assign mostForwardIdx with currIdx for readability.
     */
    int mostForwardIdx = currIdx;
    // Create or update the bottommost or rightmost AdjacentTile
    if (isOnCurrLine(mostForwardIdx, currLine, searchingDir)) {
        if (!(*adjacentTiles)[mostForwardIdx]) {
            AdjacentTilePtr currAt = std::make_shared<AdjacentTile>(
                    totalLetterScores, oss.str(),
                    forwardDir, mostForwardIdx);
            (*adjacentTiles)[mostForwardIdx] = currAt;
        } else {
            (*adjacentTiles)[mostForwardIdx]->update(
                    totalLetterScores,
                    oss.str(),
                    forwardDir);
        }
    }

    // Create or update the uppermost or leftmost emptyAdjacencyTile
    if (isOnCurrLine(mostBackwardIdx, currLine, searchingDir)) {
        if (!(*adjacentTiles)[mostBackwardIdx]) {
            AdjacentTilePtr currAt = std::make_shared<AdjacentTile>(
                    totalLetterScores, oss.str(),
                    backwardDir, mostBackwardIdx);
            (*adjacentTiles)[mostBackwardIdx] = currAt;
        } else {
            (*adjacentTiles)[mostBackwardIdx]->update(
                    totalLetterScores,
                    oss.str(),
                    backwardDir);
        }
    }
}

indicesToLettersPtr WordBuilder::getTheBestMove() {
    completeWords = std::make_shared<CompleteWords>();
    prioritiseTiles();
    std::string lettersInHand = getHand()->getLetters();
    findWords(lettersInHand);
    std::map<int, char> chosenTileIndices;
    if (!completeWords->empty())
        chosenTileIndices = completeWords->top()->tileIndices;
    /*
     * Convert chosenTileIndices into the format
     * Board's pre-existing function understands
     */
    return convert(chosenTileIndices);
}

void WordBuilder::prioritiseTiles() {
    // Clear adjacentTilesToProcess before putting
    adjacentTilesToProcess = AdjacentTilesToProcess();
    for (auto adjacentTile : *adjacentTiles) {
        if (adjacentTile)
            adjacentTilesToProcess.push(adjacentTile);
    }
}

void WordBuilder::findWords(const std::string& lettersInHand) {
    while(!adjacentTilesToProcess.empty()) {
        auto at = adjacentTilesToProcess.top();
        Angle angles[] = {VERTICAL, HORIZONTAL};

        std::string verticalLetters;
        std::string horizontalLetters;
        std::map<WordPtr, std::tuple<bool, bool>> verticalLettersToExtend;
        std::map<WordPtr, std::tuple<bool, bool>> horizontalLettersToExtend;
        std::map<int, char> tileIndices;
        bool forwardWordExists;
        bool backwardWordExists;

        for (auto letter : lettersInHand) {
            tileIndices.clear();

            // Process the connected lettersInHand
            verticalLetters = std::string(1, letter);
            horizontalLetters = std::string(1, letter);
            if (!at->adjacentLetters[UPWARD].empty())
                verticalLetters =
                        at->adjacentLetters[UPWARD] + verticalLetters;
            if (!at->adjacentLetters[DOWNWARD].empty())
                verticalLetters += at->adjacentLetters[DOWNWARD];
            if (!at->adjacentLetters[LEFTWARD].empty())
                horizontalLetters=
                        at->adjacentLetters[LEFTWARD] + horizontalLetters;
            if (!at->adjacentLetters[RIGHTWARD].empty())
                horizontalLetters += at->adjacentLetters[RIGHTWARD];

            /* Iterate VERTICAL and HORIZONTAL angles to see whether the
             * AdjacentTile can go forwards or backwards or both.
             */
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

                // If letters can be extended
                if (forwardWordExists || backwardWordExists) {
                    std::string lettersCopy(lettersInHand);
                    auto charPos = std::find(
                            lettersCopy.begin(),
                            lettersCopy.end(),
                            letter);
                    lettersCopy.erase(charPos);
                    tileIndices.insert(std::make_pair(at->idx, letter));

                    std::tuple<bool, bool> wordAvailability =
                            std::make_tuple(0, 0);
                    if (forwardWordExists)
                        std::get<FORWARDWORDEXISTS>(wordAvailability) = true;
                    if (backwardWordExists)
                        std::get<BACKWARDWORDEXISTS>(wordAvailability) = true;


                    if (angle == VERTICAL) {
                        WordPtr newWord = std::make_shared<Word>(
                                verticalLetters,
                                lettersCopy,
                                tileIndices);
                        /*
                         * If the word in the perpendicular direction was
                         * more than one letter, add it to builtWords
                         */
                        if (horizontalLetters.size() > 1) {
                            newWord->builtWords.push_back(horizontalLetters);
                        }
                        verticalLettersToExtend.insert(std::make_pair(
                                newWord,
                                wordAvailability));
                    } else {
                        WordPtr newWord = std::make_shared<Word>(
                                horizontalLetters,
                                lettersCopy,
                                tileIndices);
                        if (verticalLetters.size() > 1) {
                            newWord->builtWords.push_back(verticalLetters);
                        }
                        horizontalLettersToExtend.insert(
                                std::make_pair(newWord,
                                               wordAvailability));
                    }
                }
            }
        }

        /*
         * Find out the most backward index and forward index connected to the
         * AdjacentIdx and then build completeWords
         */

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

        if (!verticalLettersToExtend.empty()) {
            currIdx = at->idx;
            currVerticalLine = getCurrLine(currIdx, VERTICAL);
            mostBackwardIdx = currIdx;
            mostForwardIdx = currIdx;
            backwardDir = TOP;
            forwardDir = BOTTOM;

            /*
             * currIdx is occupied by one of the letters in
             * verticalLettersToExtend. Therefore, move backward to find the
             * next empty cell
             */
            mostBackwardIdx = currIdx + backwardDir;
            // As long as on the board and there are already placed tiles
            while (isPlacedTileOnTheSameLine(mostBackwardIdx,
                                             currVerticalLine,
                                             VERTICAL)) {
                mostBackwardIdx += backwardDir;
            }

            // Find out mostForwardIdx
            mostForwardIdx = currIdx + forwardDir;
            // As long as on the board and there are already placed tiles
            while (isPlacedTileOnTheSameLine(mostForwardIdx,
                                             currVerticalLine,
                                             VERTICAL)) {
                mostForwardIdx += forwardDir;
            }

            if (isEmptyTileOnTheSameLine(mostBackwardIdx,
                                         currVerticalLine,
                                         VERTICAL)) {
                isBackwardable = true;
            } else {
                isBackwardable = false;
            }

            if (isEmptyTileOnTheSameLine(mostForwardIdx,
                                         currVerticalLine,
                                         VERTICAL)) {
                isForwardable = true;
            } else {
                isForwardable = false;
            }

            // Build words
            for (auto & verticalValidLetter : verticalLettersToExtend) {
                if (isForwardable &&
                    std::get<FORWARDWORDEXISTS>(verticalValidLetter.second)) {
                    buildWordForwards(mostForwardIdx,
                                      mostBackwardIdx,
                                      currVerticalLine,
                                      VERTICAL,
                                      verticalValidLetter.first);
                }
                if (isBackwardable &&
                    std::get<BACKWARDWORDEXISTS>(verticalValidLetter.second)) {
                    buildWordBackwards(mostForwardIdx,
                                       mostBackwardIdx,
                                       currVerticalLine,
                                       VERTICAL,
                                       verticalValidLetter.first);
                }
            }
        }

        if (!horizontalLettersToExtend.empty()) {
            currIdx = at->idx;
            currHorizontalLine = getCurrLine(currIdx, HORIZONTAL);
            mostBackwardIdx = currIdx;
            mostForwardIdx = currIdx;
            backwardDir = LEFT;
            forwardDir = RIGHT;

            // Find out mostBackwardIdx first
            mostBackwardIdx = currIdx + backwardDir;
            // As long as on the board and there are already placed tiles
            while (isPlacedTileOnTheSameLine(mostBackwardIdx,
                                             currHorizontalLine,
                                             HORIZONTAL)) {
                mostBackwardIdx += backwardDir;
            }

            // Find out mostForwardIdx
            mostForwardIdx = currIdx + forwardDir;
            // As long as on the board and there are already placed tiles
            while (isPlacedTileOnTheSameLine(mostForwardIdx,
                                             currHorizontalLine,
                                             HORIZONTAL)) {
                mostForwardIdx += forwardDir;
            }

            if (isEmptyTileOnTheSameLine(mostBackwardIdx,
                                         currHorizontalLine,
                                         HORIZONTAL)) {
                isBackwardable = true;
            } else {
                isBackwardable = false;
            }

            if (isEmptyTileOnTheSameLine(mostForwardIdx,
                                         currHorizontalLine,
                                         HORIZONTAL)) {
                isForwardable = true;
            } else {
                isForwardable = false;
            }

            // Build words
            for (auto & horizontalValidLetter : horizontalLettersToExtend) {
                if (isForwardable &&
                    std::get<FORWARDWORDEXISTS>(horizontalValidLetter.second))
                    buildWordForwards(mostForwardIdx,
                                      mostBackwardIdx,
                                      currHorizontalLine,
                                      HORIZONTAL,
                                      horizontalValidLetter.first);
                if (isBackwardable &&
                    std::get<BACKWARDWORDEXISTS>(horizontalValidLetter.second))
                    buildWordBackwards(mostForwardIdx,
                                       mostBackwardIdx,
                                       currHorizontalLine,
                                       HORIZONTAL,
                                       horizontalValidLetter.first);
            }
        }

        // Move over to the next adjacent tile
        adjacentTilesToProcess.pop();
    }
}

indicesToLettersPtr WordBuilder::convert(std::map<int, char>& tileIndices) {
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

/*
 * This method converts a 2D row index back to the alphabetical representation.
 * Even though it is a roundabout way to convert it only to be converted back
 * to 2D integer indices again by board, it is considered a good solution as
 * the conversion allows for the reuse of the pre-existing methods in other
 * classes, namely executePlaceCommand() in Game.
 */
char WordBuilder::convertIntToRowLetter(int rowIdx) {
    char a = 'A';
    int baseInt = int(a);
    return rowIdx + baseInt;
}

void WordBuilder::buildWordForwards(int forwardIdx,
                                    int backwardIdx,
                                    int currLine,
                                    Angle angle,
                                    const WordPtr& word) {
    // Base case: no more letter to place in the hand
    if (word->lettersInHand.empty())  {
    } else {
        // Recursive case: extend letters and create a Word if found in dic
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
                        if (!at->adjacentLetters[DOWNWARD].empty())
                            verticalLetters += at->adjacentLetters[DOWNWARD];
                        if (!at->adjacentLetters[LEFTWARD].empty())
                            horizontalLetters =
                                    at->adjacentLetters[LEFTWARD]
                                    + horizontalLetters;
                        if (!at->adjacentLetters[RIGHTWARD].empty())
                            horizontalLetters += at->adjacentLetters[RIGHTWARD];
                    } else {
                        verticalLetters = std::string(1, letter);
                        horizontalLetters = word->wordBeingBuilt + letter;
                        if (!at->adjacentLetters[UPWARD].empty())
                            verticalLetters =
                                    at->adjacentLetters[UPWARD]
                                    + verticalLetters;
                        if (!at->adjacentLetters[DOWNWARD].empty())
                            verticalLetters += at->adjacentLetters[DOWNWARD];
                        if (!at->adjacentLetters[RIGHTWARD].empty())
                            horizontalLetters += at->adjacentLetters[RIGHTWARD];
                    }

                    if (angle == VERTICAL &&
                    dictionary->isInDict(horizontalLetters)) {
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

                    if (angle == HORIZONTAL &&
                    dictionary->isInDict(verticalLetters)) {
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
                    newWord->wordBeingBuilt = newWord->wordBeingBuilt + letter;
                    if (greedyMap->forward->count(newWord->wordBeingBuilt))
                        isForwardable = true;
                    if (greedyMap->backward->count(newWord->wordBeingBuilt))
                        isBackwardable = true;
                }
            }

            // If newWord has been assigned
            if (newWord) {
                newWord->tileIndices.insert(
                        std::make_pair(forwardIdx, letter
                        ));
                // Erase the letter from the lettersInHand
                newWord->erase(letter);

                    /*
                     * Traverse forwards until either an empty cell is
                     * encountered or out of the current line
                     */
                    forwardIdxToSearch += forwardDir;
                    while (isPlacedTileOnTheSameLine(
                            forwardIdxToSearch,
                            currLine,
                            angle))
                        forwardIdxToSearch += forwardDir;
                    if (isBackwardable && isEmptyTileOnTheSameLine(
                            backwardIdx,
                            currLine,
                            angle)) {
                        buildWordBackwards(forwardIdxToSearch,
                                           backwardIdx,
                                           currLine,
                                           angle,
                                           newWord);
                    }
                    if (isForwardable &&
                    isEmptyTileOnTheSameLine(forwardIdxToSearch,
                                             currLine,
                                             angle)) {
                        buildWordForwards(forwardIdxToSearch,
                                          backwardIdx,
                                          currLine,
                                          angle,
                                          newWord);
                    }

                // Add a word if it exists in a dictionary
                if (dictionary->isInDict(newWord->wordBeingBuilt))
                    completeWords->push(newWord);
            }
        }
    }
}

void WordBuilder::buildWordBackwards(int forwardIdx,
                                     int backwardIdx,
                                     int currLine,
                                     Angle angle,
                                     const WordPtr& word) {
    // Base case: no more letter to place in the hand
    if (word->lettersInHand.empty()) {
    } else {
        // Recursive case: extend letters and create a Word if found in dic
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
                            verticalLetters =
                                    at->adjacentLetters[UPWARD]
                                    + verticalLetters;
                        if (!at->adjacentLetters[LEFTWARD].empty())
                            horizontalLetters =
                                    at->adjacentLetters[LEFTWARD]
                                    + horizontalLetters;
                        if (!at->adjacentLetters[RIGHTWARD].empty())
                            horizontalLetters += at->adjacentLetters[RIGHTWARD];
                    } else { // If HORIZONTAL
                        verticalLetters = std::string(1, letter);
                        horizontalLetters = letter + word->wordBeingBuilt;
                        // If going horizontal, the backward direction is left
                        if (!at->adjacentLetters[UPWARD].empty())
                            verticalLetters =
                                    at->adjacentLetters[UPWARD]
                                    + verticalLetters;
                        if (!at->adjacentLetters[DOWNWARD].empty())
                            verticalLetters += at->adjacentLetters[DOWNWARD];
                        if (!at->adjacentLetters[LEFTWARD].empty())
                            horizontalLetters =
                                    at->adjacentLetters[LEFTWARD]
                                    + horizontalLetters;
                    }

                    if (angle == VERTICAL &&
                    dictionary->isInDict(horizontalLetters)) {
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

                    if (angle == HORIZONTAL &&
                    dictionary->isInDict(verticalLetters)) {
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
                newWord->tileIndices.insert(
                        std::make_pair(backwardIdx, letter
                        ));
                // Erase the letter from the lettersInHand
                newWord->erase(letter);

                /*
                 * Traverse forwards until either an empty cell is
                 * encountered or out of the current line
                 */
                backwardIdxToSearch += backwardDir;
                while (isPlacedTileOnTheSameLine(backwardIdxToSearch,
                                                 currLine,
                                                 angle))
                    backwardIdxToSearch += backwardDir;
                if (isBackwardable &&
                isEmptyTileOnTheSameLine(backwardIdxToSearch,
                                         currLine,
                                         angle)) {
                    buildWordBackwards(forwardIdx,
                                       backwardIdxToSearch,
                                       currLine,
                                       angle,
                                       newWord);
                }
                if (isForwardable &&
                isEmptyTileOnTheSameLine(forwardIdx,
                                         currLine,
                                         angle)) {
                    buildWordForwards(forwardIdx,
                                      backwardIdxToSearch,
                                      currLine,
                                      angle,
                                      newWord);
                }

                // Add a word if it exists in a dictionary
                if (dictionary->isInDict(newWord->wordBeingBuilt))
                    completeWords->push(newWord);
            }
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

bool WordBuilder::isOnCurrLine(int idx, int baseLine , Angle dir) {
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
    return isOnBoard(idx) && isOnCurrLine(idx, baseLine, dir) && board->hasPlacedTile(idx);
}

bool WordBuilder::isEmptyTileOnTheSameLine(int idx, int baseLine, Angle dir) {
    return isOnBoard(idx) && isOnCurrLine(idx, baseLine, dir) && !board->hasPlacedTile(idx);
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
    completeWords = std::make_shared<CompleteWords>();
    prioritiseTiles();
    findWords(hand->getLetters());
    auto chosenTileIndices = completeWords->top()->tileIndices;
    for (auto tileIndex: chosenTileIndices) {
        std::cout << tileIndex.second << " ";
    }
    std::cout <<
    "(you may need to combine with another letter already placed): "
    << std::endl;
}


/*
 * The code below is for the first tried algorithm using a sorted map to quickly find whether there is any word
 * that can be made given the letters. Therefore, please disregard the section below. It's for my own reference
 * in the future.
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
        while (isOnCurrLine(idxToSearch, lineToSearch, searchingDir)) {
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
//            while (isOnCurrLine(at1, baseLine, VERTICAL)) {
//                if (adjacentTiles[at1] || board->hasPlacedTile(at1))
//                    isSolvableBySortedMap = false;
//                at1 += TOP;
//            }
//
//            at2 = startIdxHorizontal + BOTTOM + BOTTOM;
//            while (isOnCurrLine(at2, baseLine, VERTICAL)) {
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
//            while (isOnCurrLine(at1, baseLine, HORIZONTAL)) {
//                if (adjacentTiles[at1] || board->hasPlacedTile(at1))
//                    isSolvableBySortedMap = false;
//                at1 += LEFT;
//            }
//
//            at2 = startIdxVertical + RIGHT + RIGHT;
//            while (isOnCurrLine(at2, baseLine, HORIZONTAL)) {
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
//        // Use istringstream to work with individual completeWords on a line
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