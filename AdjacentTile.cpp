//
// Created by Myeonghoon Sun on 22/5/2022.
//

#include "AdjacentTile.h"

/*
 * Depending on the direction from the placedTile (the reference point) to AdjacentTile,
 * store the letters of the placedTiles in appropriate positions.
 *
 * (e.g. if fromPlacedTile is TOP, AdjacentTile is created above the placedTile. Therefore, the letters
 *  of the placedTile should be stored in the DOWNWARD position of adjacentLetters.
 */
AdjacentTile::AdjacentTile(int potentialScore,
                           const std::string & lettersToStore,
                           BoardDir fromPlacedTile,
                           int idx):
                           idx(idx),
                           potentialScores(potentialScore) {

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

// Update the contents of a particular position in adjacentLetters depending on the value of fromPalcedTile
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

    // Also calculate an existing letter(s)' score and then subtract it from the total potentialScores
    // before updating it to a new value.
    size_t existingScore = 0;
    for (char ch: adjacentLetters[idx]) {
        existingScore += letterScoreMap.at(ch);
    }
    potentialScores -= existingScore;

    // Update the letters and add the corresponding score
    adjacentLetters[idx] = lettersToStore;
    potentialScores += potentialScore;
}