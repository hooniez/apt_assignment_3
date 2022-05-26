//
// Created by Myeonghoon Sun on 22/5/2022.
//

#include "AdjacentTile.h"
#include <random>

AdjacentTile::AdjacentTile(int potentialScore,
                           const std::string & lettersToStore,
                           BoardDir fromPlacedTile,
                           bool isSolvableBySortedMap,
                           int idx):
                                     idx(idx),
                                     potentialScores(potentialScore),
                                     misExtendiable(true),
                                     misSolvableBySortedMap(isSolvableBySortedMap){

    // Depending on the direction from the placedTile (the reference point) to AdjacentTile,
    // store the letters of the adjacent tiles.
    // (e.g. if fromPlacedTile is TOP, AdjacentTile is created above it. Therefore, the letters
    // of the placedTile should be stored in its DOWNWARD).
    if (fromPlacedTile == TOP) {
        adjacentLetters[DOWNWARD] = lettersToStore;
    } else if (fromPlacedTile == RIGHT) {
        adjacentLetters[LEFTWARD] = lettersToStore;
    } else if (fromPlacedTile == BOTTOM) {
        adjacentLetters[UPWARD] = lettersToStore;
    } else if (fromPlacedTile == LEFT) {
        adjacentLetters[RIGHTWARD] = lettersToStore;
    }

}

void AdjacentTile::update(int potentialScore,
                          const std::string & lettersToStore,
                          BoardDir fromPlacedTile) {
    AdjacentTileDir idx;
    if (fromPlacedTile == TOP) {
        idx = DOWNWARD;
    } else if (fromPlacedTile == RIGHT) {
        idx = LEFTWARD;
    } else if (fromPlacedTile == BOTTOM) {
        idx = UPWARD;
    } else {
        // if fromPlacedTile == LEFT
        idx = RIGHTWARD;
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

    // Since the updated AdjacentTile is next to another placed tile, misSolvableBySortedMap to false
    misSolvableBySortedMap = false;
}

void AdjacentTile::hasPlacedTile() {
    misSolvableBySortedMap = false;
    potentialScores = 0;
}