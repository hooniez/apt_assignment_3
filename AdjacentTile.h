//
// Created by Myeonghoon Sun on 22/5/2022.
//

#ifndef ASSIGNMENT3_ADJACENTTILE_H
#define ASSIGNMENT3_ADJACENTTILE_H

/*
 * AdjacentTiles surround the placed tiles. They can be created or updated based on the placed tiles.
 *
 * Each AdjacentTile keeps track of all the letters of placed tiles connected to it in all four directions.
 * For example, if there is an AdjacencyTile with 5 letter-long word in each direction, it has a record of
 * each 5 letters in its array of fixed size 4 with the first index (0) representing the connected 5 letters above
 * the AdjacentTile; the index 1 the right; the index 2 the bottom, the index 3 the left in a clockwise manner.
 *
 * A potential score tapped into the just placed tiles is used to initialise AdjacentTile coupled with
 * their letters to be stored in a particular index of the mentioned array
 * based on fromPlacedTile variable denoting the direction where the tiles are placed relative to the AdjacentTile.
 * If tiles are placed from below where an AdjacentTile will be created, their letters are stored at the index 2.
 *
 * Every time tiles are placed in each turn, if the placed tiles are next to AdjacentTiles, all the
 * adjacent tiles' data members including potentialScore and lettersToScore are updated.
 * If tiles are placed directly where an AdjacentTile is, it creates or updates all the other four
 * AdjacencyTiles connected to them by the placed tiles, however many there are.
 */

#include "types.h"

#include <tuple>
#include <queue>

class AdjacentTile {
public:
    AdjacentTile(int potentialScore,
                 const std::string & lettersToStore,
                 BoardDir fromPlacedTile,
                 int idx);

    int idx;
    size_t potentialScores;
    // An array of fixed size 4 to store the letters connected to the AdjacentTile in each direction.
    std::string adjacentLetters[TOTALDIRECTIONS];
    void update(int potentialScore,
                const std::string & lettersToStore,
                BoardDir fromPlacedTile);
};

typedef std::shared_ptr<AdjacentTile> AdjacentTilePtr;
typedef std::vector<AdjacentTilePtr> AdjacentTiles;

/*
 * AdjacentTiles are stored in two containers—one in an array of the size BOARD_LENGTH * BOARD_LENGTH (for fast
 * access and, hence update) and the other in a priority queue sorted by potentialScore;
 * initially the priority queue was chosen because of the uncertainty involved with how fast the algorithm
 * to be implemented could process all the AdjacentTiles.
 *
 * In an attempt to reduce the number of AdjacentTiles to process if the algorithm turned out to be slow,
 * the priority queue has been selected so that the most valuable tiles (high potential scores) get processed first.
 *
 * However, the algorithm turned out to be extremely fast and the time complexity of building a priority queue
 * (O(nlogn) is not as concerning compared to a vector as there are only few tiles.
 * If there were a lot more tiles to turn into a priority queue each turn,
 * it may still be better to go with a priority queue to select a few worth tiles to build words from.
 * rather than go through each of them linearly and run the algorithm from semi-optimal starting points.
 */

struct CompareAdjacentTile {
public:
    // Return true if at1's potential score is less than at2's potential score
    bool operator()(const AdjacentTilePtr & at1, const AdjacentTilePtr & at2) const {
        return at1->potentialScores < at2->potentialScores;;
    }
};

typedef std::priority_queue<AdjacentTilePtr, std::vector<AdjacentTilePtr>, CompareAdjacentTile> TilesToStartFrom;

#endif //ASSIGNMENT3_ADJACENTTILE_H
