//
// Created by Myeonghoon Sun on 20/5/2022.
//

#ifndef ASSIGNMENT3_WORDBUILDER_H
#define ASSIGNMENT3_WORDBUILDER_H

/*
 * WordBuilder is initialised when --ai or --hint options are enabled.
 *
 * WordBuilder's functions make up the algorithm that drives tile placements.
 * It extends the Player class so that it can be polymorphic, able to utilise
 * the methods of Player.
 *
 * The algorithm first fetches the tiles placed in the previous turn; If it
 * is the first round, it has no placed tiles to process and, hence, inserts
 * an AdjacentTile at the centre.
 *
 * Once the placed tiles are fetched in the 1 dimensional notation (e.g. H7
 * equals BOARD_LENGTH * (BOARD_LENGTH / 2) + (BOARD_LENGTH / 2)), they are
 * then iterated from the leftmost or uppermost, depending on the angle at
 * which letters are placed by the other player, to rightmost or bottommost.
 * For example, if letters are placed as below across the center line,
 *
 * | | | | | | | | | | | | | | | |
 * | | | | | |A|P|T| | | | | | | |
 * | | | | | | | | | | | | | | | |
 * FIG-1
 *
 * The letters A, P, and T are processed in the order twice vertically and
 * horizontally.
 *
 * Since the word is placed horizontally, beAwareOfTiles() calls
 * createOrUpdateAdjacentTiles() horizontally once and vertically three
 * times to create AdjacentTiles around the word as seen below as noted by
 * the asterisks.
 *
 * | | | | | |*|*|*| | | | | | | |
 * | | | | |*|A|P|T|*| | | | | | |
 * | | | | | |*|*|*| | | | | | | |
 * FIG-2
 *
 * createOrUpdateAdjacentTiles(), when first called in the horizontal
 * direction, moves mostBackwardIdx first backward from the placed tile with
 * the smallest index, in this case A until it finds an empty tile or goes
 * off the current line. Once it lands on an empty cell (the left most * in
 * FIG-2), currIdx is assigned to its previous index, in this case where A is.
 * currIdx then traverses forwards, while keeping track of each of the
 * letters' values and the letters themselves, until it finds an empty tile
 * (the rightmost * in FIG-2) or goes off the current line. It is then
 * an AdjacentTile is created where each of the asterisks is (leftmost and
 * rightmost); that is as long as it is on the same line as the root index,
 * that of A. When an AdjacentTile is created, the letters collected are
 * stored in a manner mentioned in AdjacentTile.h; so is the total score.
 *
 * After the previous steps are complete, the same procedure is repeated in the
 * vertical direction; this time uppermost and bottommost. By the time
 * beAwareOfTiles() comes to an end, 8 AdjacentTiles will have been created
 * and stored in the data member called adjacentTiles, which is a vector of
 * size BOARD_LENGTH * BOARD_LENGTH initialised with nullptr; an Array could
 * have been used here as a container does not need to grow, but an Array
 * needs to be converted to a vector when the container is turned into a
 * priority queue later on. Therefore, it is reasonable to settle on a vector
 * to begin with and utilise its convenient data methods.
 *
 * Another example of what happens when letters are placed:
 *
 * | | | | |*| | | | | | | | | | |
 * | | | |*|S|*| | | | | | | | | |
 * | | | |*|C|*|*|*| | | | | | | |
 * | | | |*|R|A|P|T|*| | | | | | |
 * | | | |*|A|*|*|*| | | | | | | |
 * | | | |*|B|*| | | | | | | | | |
 * | | | |*|B|*| | | | | | | | | |
 * | | | |*|L|*| | | | | | | | | |
 * | | | |*|E|*| | | | | | | | | |
 * | | | | |*| | | | | | | | | | |
 * FIG-3 (illegal bingo)
 *
 * In the case above, the uppermost and bottommost AdjacentTiles are created
 * first and then more are created horizontally. Here, the edge cases worth
 * noting are the asterisk whose left-hand side is C of SCRABBLE and bottom
 * side is A of RAPT. For what happens in this case, please refer again to
 * AdjacentTile.h. However, what is a more interesting case is the right most
 * AdjacentTile of RAPT. Its value denoting what letters are to its left is
 * updated when R of SCRABBLE is processed horizontally. Additionally, the
 * AdjacentTile left to APT is now destroyed, squashed by the big R.
 *
 * Once all the AdjacentTiles are created or updated, each of them are then put
 * into a max-heap based on potential scores from its surrounding placed
 * tiles. The resultant adjacentTilesToProcess is then processed until empty
 * (originally it was hard to gauge the efficiency of the algorithm; as such
 * it was important to have a way of processing a certain number of
 * adjacentTiles depending on how slow the algorithm runs). Each of
 * the letters in the hand is then concatenated to verticalLetters and
 * horizontalLetters if any.
 *
 * After concatenation, whether the letter placed on the AdjacentTile can be
 * extended forwards or backwards or both in both the vertical and horizontal
 * directions. For example, For an adjacentTile to be extended forwards from
 * the given letter in the vertical direction, the horizontalLetters must be a
 * complete word. Once the horizontalLetters is confirmed to exist in the
 * dictionary, forwardMap is consulted to see whether the verticalLetters
 * exist as a key. If it does, forwardWordExists is set to true. The same
 * step is repeated for the backwards direction. If two of the conditions are
 * met, the combination of the particular letter and the AdjacentTile and
 * other information mentioned in Word.h is turned into an instance of Word.
 * The previous steps are repeated for every single letter in the hand before
 * moving onto the next phase.
 *
 * Now, the most backward index and forward index connected to the
 * AdjacentIdx need to be found out before being able to extend further. As
 * seen above in createOrUpdateAdjacentTiles(), mostBackwardIdx and
 * mostForwardIdx will land on the next closest empty tile. There is a
 * possibility that it may land on a different line in the single dimensional
 * representation of the board, but isEmptyTileOnTheSameLine() provides for
 * such an edge case. These steps will be repeated for both directions before
 * recursively further extending what are still potentially valid letters.
 *
 * At this point, verticalLettersToExtend and horizontalLettersToExtend
 * contain all the concatenated letters to extend, and their corresponding
 * remaining letters in the hand, and the letters to actually place as
 * instances of Word as well as two boolean values for each Word indicating
 * whether its stored letters can be extended forwards or backwards or both
 * based on the results found by forwardMap and backwardMap. It is then with
 * this information, recursive functions are, buildWordForwards and
 * buildWordBackwards, accordingly called.
 *
 * Each recursive case iterates the remaining tiles in the hand associated
 * with the Word object on which a recursive call was made. Using the forward
 * and backward greedy maps, it can be determined that whether the
 * wordBeingBuilt is extendable in the time complexity of O(1). If it is,
 * whether there is an AdjacentTile at the current index is checked. If so,
 * similar steps mentioned three paragraphs above are executed and new Words
 * objects are created and their forwardability and backwardability are also
 * checked (the same results for the index where there is no AdjacentTile
 * with fewer checks). After that, depending on whether a new Word can be
 * extended forwards or backwards, recursive functions are called on it.
 * After a new Word comes back from the recursion, its wordBeingBuilt is
 * checked in the dictionary. If so, it is added to completeWords (a
 * max-heap) for finding the best possible placement.
 *
 */

#include <map>
#include <unordered_map>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <array>
#include <string>

#include "types.h"
#include "player.h"
#include "board.h"
#include "GreedyMap.h"
#include "AdjacentTile.h"
#include "Word.h"

typedef std::map<std::string, char> indicesToLetters;
typedef std::shared_ptr<indicesToLetters> indicesToLettersPtr;

class WordBuilder : public Player {
public:
    // This constructor is called from loading a saved game
    WordBuilder(GreedyMapPtr  greedyMap,
                DictionaryPtr  dictionary,
                const std::string& name,
                int score,
                LinkedListPtr<TilePtr> hand,
                BoardPtr board);

    // This constructor is called when starting a new game
    WordBuilder(GreedyMapPtr  greedyMap,
                DictionaryPtr  dictionary,
                const std::string& name,
                BoardPtr board);

    void scanTheBoard();

    void beAwareOfTiles();

    void createOrUpdateAdjacentTiles(Angle searchingDir,
                                     int currIdx,
                                     int currLine,
                                     Angle placedDirection);

    // WordBuilder executes the next move
    std::shared_ptr<std::map<std::string, char>> getTheBestMove();

    // Convert the processed tiles into a priority queue
    void prioritiseTiles();

    void findWords(const std::string &);

    void buildWordForwards(int forwardIdx,
                           int backwardIdx,
                           int currLine,
                           Angle angle,
                           const WordPtr& word);

    void buildWordBackwards(int forwardIdx,
                            int backwardIdx,
                            int currLine,
                            Angle angle,
                            const WordPtr& word);

    indicesToLettersPtr convert(std::map<int, char> &);

    // Check if the index is within the board and on the same line as the baseLine
    bool isOnCurrLine(int idx, int baseLine, Angle dir);

    int getCurrLine(int idx, Angle dir);

    bool isOnBoard(int idx);

    void setGreedyMap(GreedyMapPtr greedyMap);

    void setBoard(BoardPtr board);

    void setDictionary(DictionaryPtr dictionary);

    void setAdjacentTiles(AdjacentTilesPtr adjacentTiles);

    char convertIntToRowLetter(int);

    bool isPlacedTileOnTheSameLine(int idx, int baseLine, Angle dir);

    bool isEmptyTileOnTheSameLine(int idx, int baseLine, Angle dir);

    void giveHint(const LinkedListPtr<TilePtr>& hand);

private:
    GreedyMapPtr greedyMap;

    BoardPtr board;

    DictionaryPtr dictionary;

    CompleteWordsPtr completeWords;

    AdjacentTilesPtr adjacentTiles;

    AdjacentTilesToProcess adjacentTilesToProcess;
};
typedef std::shared_ptr<WordBuilder> WordBuilderPtr;

#endif //ASSIGNMENT3_WORDBUILDER_H
