
#ifndef ASSIGN2_TILE_H
#define ASSIGN2_TILE_H

#include <memory>

// Define a Letter type
typedef char Letter;

// Define a Value type
typedef int Value;

class Tile
{

public:
    Tile();
    Tile(Letter, Value);
    Tile(Letter);
    Tile(const Tile&);
    Letter getLetter() const;
    Value getValue() const;
    // check if the letter in the tile matches the letter given
    bool isLetter(Letter) const;

private:
    const Letter letter;
    const Value value;
};
typedef std::shared_ptr<Tile> TilePtr;
#endif // ASSIGN2_TILE_H
