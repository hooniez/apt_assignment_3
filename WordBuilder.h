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
 * For example, let's say there is one word (GOOD) placed in the board:
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
 * forwardAiMap and backwardAiMap, by which one with the highest value
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
#include "SortedMapBuilder.h"
#include "GreedyMapBuilder.h"
#include "AdjacentTile.h"
#include "Word.h"

class WordBuilder : public Player {
public:
    // This constructor is called when --hint is typed by itself
    WordBuilder(const std::string& forwardSearchMapFileName,
                const std::string& backwardSearchMapFileName,
                const std::string& sortedMapFileName,
                const DictionaryPtr& dictionary,
                BoardPtr board,
                bool canGiveHints);
    // This constructor is called when --ai is typed by itself OR with --hint
    WordBuilder(const std::string& forwardSearchMapFileName,
                const std::string& backwardSearchMapFileName,
                const std::string& sortedMapFileName,
                const DictionaryPtr& dictionary,
                const std::string& name,
                size_t score,
                LinkedListPtr<TilePtr> hand,
                BoardPtr board,
                bool canGiveHints);

    WordBuilder(const std::string& forwardSearchMapFileName,
                const std::string& backwardSearchMapFileName,
                const std::string& sortedMapFileName,
                const DictionaryPtr& dictionary,
                const std::string& name,
                BoardPtr board,
                bool canGiveHints);

    void readFileToMap(const std::string &, greedyMapPtr& map);
    void readFileToMap(const std::string &, sortedMapPtr& map);
    void initialiseMaps();
    // WordBuilder executes the next move
    void execute();

    void updateAdjacentTiles();
    void suggestTiles();

    // Check if the index is within the board and on the same line as the baseLine
    bool isOnBaseLine(int idx, int baseLine, Angle dir);

    void createOrUpdateEmptyAdjacencyTiles(Angle searchingDir,
                                           int currIdx,
                                           int currLine,
                                           Angle placedDirection);

    int getCurrLine(int idx, Angle dir);


    void beAwareOfTiles();

    void placeTiles();

    bool isLineOnBoard(int line);
    // Convert the processed tiles into a priority queue
    void convertTilesIntoPQ();

    void updateSortedMappableAdjacentTile();

    void setBoard(BoardPtr board);

    void setDictionary(DictionaryPtr dictionary);




    bool canGiveHints;
    bool isPlaying;
private:
    greedyMapPtr greedyForwardMap;
    greedyMapPtr greedyBackwardMap;
    sortedMapPtr sortedMap;
    BoardPtr board;
    DictionaryPtr dictionary;
    std::shared_ptr<std::priority_queue<WordPtr, std::vector<WordPtr>, CompareWord>> wordsInQueue;
//    std::set<EmptyTilePtr> singleWordTiles;
//    std::set<EmptyTilePtr> multiWordTiles;
//    std::array<BoardDir, TOTALDIRECTIONS> verticaldDirs =
//            {TOP, BOTTOM};
//
//    std::array<BoardDir, TOTALDIRECTIONS> horizontalDirs =
//            {RIGHT, LEFT};


    // emptyAdjacnetTiles keep track of instances of EmptyAdjancetTile
    AdjacentTiles adjacentTiles;
    // tilesToStartFrom stores the same instances of AdjacentTile stored
    // in adjacentTiles sorted by its potential scores
//    AdjacentTilesPtr tilesToStartFrom;
    TilesToStartFrom tilesToStartFrom;

    const std::vector<BoardDir> boardDirections = {TOP, RIGHT, BOTTOM, LEFT};
    const std::vector<AdjacentTileDir> adjacentTileDirection =
            {DOWNWARD, LEFTWARD, UPWARD, RIGHTWARD};

    void recursivelyBuildWord(int backwardIdx,
                                 int forwardIdx,
                                 int currLine,
                                 Angle angle,
                                 std::string& tempLetters,
                                 std::string& lettersInHand,
                                 bool noLetterPlaceable,
                                 std::map<int, char>& tilesIndices);

//    bool isExtensionable(const AdjacentTilePtr&, std::string letters)


//    std::shared_ptr<std::map<int, std::tuple<int, int>>> testMap;


};
typedef std::shared_ptr<WordBuilder> WordBuilderPtr;

#endif //ASSIGNMENT3_WORDBUILDER_H
