//
// Created by Myeonghoon Sun on 20/5/2022.
//

#ifndef ASSIGNMENT3_PLACEDTILE_H
#define ASSIGNMENT3_PLACEDTILE_H

/*
 * Tiles placed will be stored in a max heap in Board class
 * to quickly find out the most lucrative places for AI to place tiles
 *
 * placedTile stores x and y coordinates in addition to what's already
 * tracked by Tile
 */
#include <tuple>

#include "tile.h"

class PlacedTile: public Tile {
public:
    PlacedTile(TilePtr, std::tuple<size_t, size_t> gridLoc);
    bool operator<(const PlacedTile& other) const;
private:
    std::tuple<size_t, size_t> gridLoc;
};
typedef std::shared_ptr<PlacedTile>
        placedTilePtr;


#endif //ASSIGNMENT3_PLACEDTILE_H
