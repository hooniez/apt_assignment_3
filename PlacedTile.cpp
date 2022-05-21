//
// Created by Myeonghoon Sun on 20/5/2022.
//

#include "PlacedTile.h"
PlacedTile::PlacedTile(TilePtr tile, std::tuple<size_t, size_t> gridLoc): Tile(*tile) {
    this->gridLoc = gridLoc;
}

bool PlacedTile::operator<(const PlacedTile& other) const {
    return this->getValue() < other.getValue();
}