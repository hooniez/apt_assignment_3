//
// Created by Myeonghoon Sun on 22/5/2022.
//

#include "EmptyAdjacentTile.h"

EmptyAdjacentTile::EmptyAdjacentTile(int potentialScore,
                                     const std::string & lettersToStore,
                                     DirectionFromPlacedTile fromPlacedTile,
                                     int idx):
                                     idx(idx), potentialScores(potentialScore) {

    // Depending on the direction from the placedTile (the reference point) to EmptyAdjacentTile,
    // store the letters of the adjacent tiles.
    // (e.g. if fromPlacedTile is UP, EmptyAdjacentTile is created above it. Therefore, the letters
    // of the placedTile should be stored in its BOTTOMIDX).
    if (fromPlacedTile == UP) {
        adjacentLetters[BOTTOMIDX] = lettersToStore;
    } else if (fromPlacedTile == RIGHT) {
        adjacentLetters[LEFTIDX] = lettersToStore;
    } else if (fromPlacedTile == BOTTOM) {
        adjacentLetters[TOPIDX] = lettersToStore;
    } else if (fromPlacedTile == LEFT) {
        adjacentLetters[RIGHTIDX] = lettersToStore;
    }
}

void EmptyAdjacentTile::update(int potentialScore,
                               const std::string & lettersToStore,
                               DirectionFromPlacedTile fromPlacedTile) {
    AdjacentTileIdx idx;
    if (fromPlacedTile == UP) {
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