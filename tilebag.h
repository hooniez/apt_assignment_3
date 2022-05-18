#ifndef ASSIGN2_TILEBAG_H
#define ASSIGN2_TILEBAG_H

#include "types.h"
#include "tile.h"
#include "linkedlist.h"

#include <fstream>
#include <random>
#include <algorithm>

class TileBag
{
public:
    TileBag();
    TileBag(LinkedListPtr<TilePtr> bag);
    ~TileBag();
    std::shared_ptr<std::vector<TilePtr>> constructTiles();
    TilePtr dealOne();

    // put the tile given form the player at the end of the linked list not
    // a random location due to the requirement then return the tile at the
    // head
    TilePtr replace(TilePtr);

    // tile bag being empty is the condition to end the game
    bool isEmpty();

    // retrieve full bag for saving
    LinkedListPtr<TilePtr> getBag();

private:
    LinkedListPtr<TilePtr> bag;
};
typedef std::shared_ptr<TileBag> TileBagPtr;
typedef std::shared_ptr<std::vector<TilePtr>> VectorOfTilePtr;

#endif // ASSIGN2_TILEBAG_H
