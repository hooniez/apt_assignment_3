#include "tilebag.h"

TileBag::TileBag()
{
    // create tiles from the file
    VectorOfTilePtr tileSet = constructTiles();

    // Shuffle the vector of tiles
    std::random_device rd;
    auto rng = std::default_random_engine(rd());
    std::shuffle(tileSet->begin(), tileSet->end(), rng);

    // put the tiles in the linked list as the tile bag
    bag = std::make_shared<LinkedList<TilePtr>>(tileSet);
}

TileBag::TileBag(LinkedListPtr<TilePtr> bag) : bag(bag) {}

TileBag::~TileBag()
{
    bag = nullptr;
}

VectorOfTilePtr TileBag::constructTiles()
{
    VectorOfTilePtr outlist = std::make_shared<std::vector<TilePtr>>();
    std::ifstream inFile(TILES_TXT_PATH, std::ifstream::in);
    Letter letter;
    Value value;
    while (inFile >> letter >> value)
    {
        outlist->push_back(std::make_shared<Tile>(letter, value));
    }
    inFile.close();

    return outlist;
}

TilePtr TileBag::dealOne()
{
    return bag->dequeue();
}

TilePtr TileBag::replace(TilePtr discardedTile)
{
    bag->append(discardedTile);
    return bag->dequeue();
}

bool TileBag::isEmpty()
{
    return bag->getLength() == 0;
}

LinkedListPtr<TilePtr> TileBag::getBag()
{
    return bag;
}