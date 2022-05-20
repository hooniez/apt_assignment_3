#include "tile.h"
#include "types.h"

Tile::Tile() : letter('\0'), value(0) {}
Tile::Tile(char l, int v) : letter(l), value(v) {}
Tile::Tile(char l) : letter(l), value(letterValMap.at(l)) {}
Tile::Tile(const Tile& other): letter(other.letter), value(other.value) {}

Letter Tile::getLetter() const
{
    return letter;
}

Value Tile::getValue() const
{
    return value;
}

bool Tile::isLetter(Letter checkedletter) const
{
    return letter == checkedletter;
}