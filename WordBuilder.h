//
// Created by Myeonghoon Sun on 20/5/2022.
//

#ifndef ASSIGNMENT3_WORDBUILDER_H
#define ASSIGNMENT3_WORDBUILDER_H
#include <string>

/*
 * WordBuilder is initialised when --ai or --hint options are enabled.
 *
 * The first step to making the algorithm efficient is to find an efficient way
 * to differentiate hard problems from easy ones.
 *
 * A placement adjacent to only one existing tile is considered easy whereas
 * a placement adjacent to more than one existing tile is considered hard.
 *
 * For example, let's say there is one wordBeingBuilt (GOOD) placed in the board:
 *
 *                       XXXX
 *                      YGOODY
 *                       XXXX
 *
 * The cells marked with X are considered easy (as far as tiles are placed in the
 * perpendicular direction). The cells marked with Y are also considered easy
 * (A placement in the same direction is considered easy).
 *
 * Now, let's place another GOOD across where the second O is
 *
 *                         Y
 *                        XGX
 *                       XZOZ
 *                      YGOODY
 *                       XZDZ
 *                         Y
 *
 * The cells marked by Z are considered hard as more than one check is needed
 * to find out the placement results in valid wordsInQueue.
 *
 * The algorithm will keep track of available placements (all the X, Y, Z tiles)
 * in two sets, used as priority queues, namely singleWordTiles and multiWordTiles.
 * Each of the tiles in the sets will contain a data member that keeps track of
 * the surrounding letters and their total values.
 *
 * it will prioritise placement on easy tiles using sortedMap coupled with
 * the sorting of the tiles in the hand. For example, if the player
 *
 *
 * Two maps are used to make the best decision from a given tile
 * or given continuous tiles without a gap.
 *
 * Each of the tiles placeable is searched by its letter in both
 * forwardMap and backwardMap, by which one with the highest value
 * (the probability of a letter among valid letters * the score of a letter)
 * can be found.
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

#include "types.h"
#include "player.h"
#include "board.h"
#include "GreedyMap.h"
#include "AdjacentTile.h"
#include "Word.h"

class WordBuilder : public Player {
public:
    // This constructor is called from loading a saved game
    WordBuilder(const GreedyMapPtr& greedyMap,
                const DictionaryPtr& dictionary,
                const std::string& name,
                size_t score,
                LinkedListPtr<TilePtr> hand,
                BoardPtr board);

    // This constructor is called when starting a new game
    WordBuilder(const GreedyMapPtr& greedyMap,
                const DictionaryPtr& dictionary,
                const std::string& name,
                BoardPtr board);

    // WordBuilder executes the next move
    std::shared_ptr<std::map<std::string, char>> getTheBestMove();

    void findWords(const std::string &);

    // Check if the index is within the board and on the same line as the baseLine
    bool isOnBaseLine(int idx, int baseLine, Angle dir);

    void createOrUpdateEmptyAdjacencyTiles(Angle searchingDir,
                                           int currIdx,
                                           int currLine,
                                           Angle placedDirection);

    int getCurrLine(int idx, Angle dir);

    void beAwareOfTiles();

    bool isOnBoard(int idx);
    // Convert the processed tiles into a priority queue
    void prioritiseTiles();

    void setGreedyMap(GreedyMapPtr greedyMap);

    void setBoard(BoardPtr board);

    void setDictionary(DictionaryPtr dictionary);

    std::shared_ptr<std::map<std::string, char>> convert(std::map<int, char> & tileIndices);

    char convertIntToRowLetter(int);

    bool isPlacedTileOnTheSameLine(int idx, int baseLine, Angle dir);

    bool isEmptyTileOnTheSameLine(int idx, int baseLine, Angle dir);

    void scanTheBoard();

    void giveHint(const LinkedListPtr<TilePtr>& hand);

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

private:
    GreedyMapPtr greedyMap;
    BoardPtr board;
    DictionaryPtr dictionary;
    std::shared_ptr<std::priority_queue<WordPtr, std::vector<WordPtr>, CompareWord>> wordsInQueue;

    // adjacentTiles keeps track of instances of AdjacentTile
    AdjacentTiles adjacentTiles;
    // tilesToStartFrom stores AdjacentTiles sorted by its potential scores in ascending order
    TilesToStartFrom tilesToStartFrom;


};
typedef std::shared_ptr<WordBuilder> WordBuilderPtr;

#endif //ASSIGNMENT3_WORDBUILDER_H
