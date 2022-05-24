//
// Created by Myeonghoon Sun on 22/5/2022.
//

#ifndef ASSIGNMENT3_EMPTYADJACENTTILE_H
#define ASSIGNMENT3_EMPTYADJACENTTILE_H

/*
 * EmptyAdjacentTile keeps track of all the available tiles to place letters
 *
 *
 */

#include "types.h"

#include <tuple>
#include <set>

// TODO: get rid of idx after testing
class EmptyAdjacentTile {
public:
    EmptyAdjacentTile(int potentialScore,
                      const std::string & lettersToStore,
                      DirectionFromPlacedTile fromPlacedTile,
                      int idx);
//    std::set<CoordinatedTilePtr> adjacentPlacedTiles;
    int idx;
    size_t potentialScores;
    std::string adjacentLetters[TOTALDIRECTIONS];
    void update(int potentialScore,
                const std::string & lettersToStore,
                DirectionFromPlacedTile fromPlacedTile);



};

typedef std::shared_ptr<EmptyAdjacentTile>
        EmptyAdjacentTilePtr;

struct CompareEmptyAdjacentTile {
public:
    // Return true if eat1's potential score is less than
    // eat2's potential score
    bool operator()(const EmptyAdjacentTilePtr & eat1, const EmptyAdjacentTilePtr & eat2) const {
//        bool res;
//        if (ct1->placedDir == HORIZONTAL) {
//            res = std::get<Y>(*ct1->gridLoc) < std::get<Y>(*ct2->gridLoc);
//        } else {
//            res = std::get<X>(*ct1->gridLoc) < std::get<X>(*ct2->gridLoc);
//        }
        return eat1->potentialScores < eat2->potentialScores;
    }
};

typedef std::multiset<EmptyAdjacentTilePtr, CompareEmptyAdjacentTile> EmptyAdjacentTilesType;
typedef std::shared_ptr<EmptyAdjacentTilesType> EmptyAdjacentTilesPtr;

#endif //ASSIGNMENT3_EMPTYADJACENTTILE_H
