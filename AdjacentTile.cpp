//
// Created by Myeonghoon Sun on 22/5/2022.
//

#include "AdjacentTile.h"
#include <random>

AdjacentTile::AdjacentTile(int potentialScore,
                           const std::string & lettersToStore,
                           DirectionFromPlacedTile fromPlacedTile,
                           bool isSolvableBySortedMap,
                           int idx):
                                     idx(idx),
                                     potentialScores(potentialScore),
                                     misEmpty(true),
                                     misExtendiable(isSolvableBySortedMap) {

    // Depending on the direction from the placedTile (the reference point) to AdjacentTile,
    // store the letters of the adjacent tiles.
    // (e.g. if fromPlacedTile is TOP, AdjacentTile is created above it. Therefore, the letters
    // of the placedTile should be stored in its BOTTOMIDX).
    if (fromPlacedTile == TOP) {
        adjacentLetters[BOTTOMIDX] = lettersToStore;
    } else if (fromPlacedTile == RIGHT) {
        adjacentLetters[LEFTIDX] = lettersToStore;
    } else if (fromPlacedTile == BOTTOM) {
        adjacentLetters[TOPIDX] = lettersToStore;
    } else if (fromPlacedTile == LEFT) {
        adjacentLetters[RIGHTIDX] = lettersToStore;
    }

    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist6(0,1);
    misSolvableBySortedMap = dist6(rng);
}

void AdjacentTile::update(int potentialScore,
                          const std::string & lettersToStore,
                          DirectionFromPlacedTile fromPlacedTile) {
    AdjacentTileIdx idx;
    if (fromPlacedTile == TOP) {
        idx = BOTTOMIDX;
    } else if (fromPlacedTile == RIGHT) {
        idx = LEFTIDX;
    } else if (fromPlacedTile == BOTTOM) {
        idx = TOPIDX;
    } else {
        // if fromPlacedTile == LEFT
        idx = RIGHTIDX;
    }

    // First calculate an existing letter(s)' score and then subtract it from the total potentialScores
    size_t existingScore = 0;
    for (char ch: adjacentLetters[idx]) {
        existingScore += letterScoreMap.at(ch);
    }
    potentialScores -= existingScore;

    // Update the letters and add the corresponding score
    adjacentLetters[idx] = lettersToStore;
    potentialScores += potentialScore;
}

void AdjacentTile::hasPlacedTile() {
    misSolvableBySortedMap = false;
    potentialScores = 0;
    misEmpty = false;
}