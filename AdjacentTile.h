//
// Created by Myeonghoon Sun on 22/5/2022.
//

#ifndef ASSIGNMENT3_ADJACENTTILE_H
#define ASSIGNMENT3_ADJACENTTILE_H

/*
 * AdjacentTile keeps track of all the available tiles to place letters
 * it also stores all the letters connected to it to its up, right, down, and left.
 *
 */

#include "types.h"

#include <tuple>
#include <queue>

// TODO: get rid of idx after testing
class AdjacentTile {
public:
    AdjacentTile(int potentialScore,
                 const std::string & lettersToStore,
                 BoardDir fromPlacedTile,
                 bool isSolvableBySortedMap,
                 int idx);
//    std::set<CoordinatedTilePtr> adjacentPlacedTiles;
    int idx;
    size_t potentialScores;
    std::string adjacentLetters[TOTALDIRECTIONS];
    bool misExtendiable;
    bool misSolvableBySortedMap;
    void update(int potentialScore,
                const std::string & lettersToStore,
                BoardDir fromPlacedTile);
    void hasPlacedTile();



};

typedef std::shared_ptr<AdjacentTile> AdjacentTilePtr;
typedef std::vector<AdjacentTilePtr> AdjacentTiles;

struct CompareAdjacentTile {
public:
    // Return true if at1's potential score is less than
    // at2's potential score
    bool operator()(const AdjacentTilePtr & at1, const AdjacentTilePtr & at2) const {
        bool res;
        if (at1->misSolvableBySortedMap == at2->misSolvableBySortedMap) {
            res = at1->potentialScores < at2->potentialScores;
        } else {
            res = at1->misSolvableBySortedMap < at2->misSolvableBySortedMap;
        }
        return res;
    }
};

typedef std::priority_queue<AdjacentTilePtr, std::vector<AdjacentTilePtr>, CompareAdjacentTile> TilesToStartFrom;

#endif //ASSIGNMENT3_ADJACENTTILE_H
