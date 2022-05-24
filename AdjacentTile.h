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
#include <set>

// TODO: get rid of idx after testing
class AdjacentTile {
public:
    AdjacentTile(int potentialScore,
                 const std::string & lettersToStore,
                 DirectionFromPlacedTile fromPlacedTile,
                 bool isSolvableBySortedMap,
                 int idx);
//    std::set<CoordinatedTilePtr> adjacentPlacedTiles;
    int idx;
    size_t potentialScores;
    std::string adjacentLetters[TOTALDIRECTIONS];
    bool misEmpty;
    bool misExtendiable;
    bool misSolvableBySortedMap;
    void update(int potentialScore,
                const std::string & lettersToStore,
                DirectionFromPlacedTile fromPlacedTile);
    void hasPlacedTile();



};

typedef std::shared_ptr<AdjacentTile>
        AdjacentTilePtr;

struct CompareEmptyAdjacentTile {
public:
    // Return true if eat1's potential score is less than
    // eat2's potential score
    bool operator()(const AdjacentTilePtr & eat1, const AdjacentTilePtr & eat2) const {
        bool res;
        if (eat1->misSolvableBySortedMap == eat2->misSolvableBySortedMap) {
            res = eat1->potentialScores < eat2->potentialScores;
        } else {
            res = eat1->misSolvableBySortedMap < eat2->misSolvableBySortedMap;
        }

        return res;
    }
};

typedef std::multiset<AdjacentTilePtr, CompareEmptyAdjacentTile> EmptyAdjacentTilesType;
typedef std::shared_ptr<EmptyAdjacentTilesType> AdjacentTilesPtr;

#endif //ASSIGNMENT3_ADJACENTTILE_H
