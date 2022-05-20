//
// Created by Myeonghoon Sun on 20/5/2022.
//

#include "PlacedTile.h"
PlacedTile::PlacedTile(TilePtr tile, std::tuple<size_t, size_t> gridLoc): Tile(*tile) {
    this->gridLoc = gridLoc;
}

// A std priority queue uses the < operator to determine the positions of elements
bool PlacedTile::operator<(const PlacedTile& other) {
    return this->getValue() < other.getValue();
}