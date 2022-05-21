#ifndef ASSIGN2_TYPES_H
#define ASSIGN2_TYPES_H

#include <map>
#include <set>
#include <string>

enum Input
{
    NEW = 1,
    LOAD,
    CREDIT,
    QUIT
};

typedef std::set<std::string> validOptionType;
typedef std::shared_ptr<validOptionType> validOptions;
const std::set<std::string> OPTIONS = {"--ai", "--dictionary", "--hint"};

#define BOARD_LENGTH 15
#define NUM_PLAYER_TILES 7
#define NUM_PLAYERS 2
#define BIGO_ADDITIONAL_SCORE 50
#define TOTAL_WORDS_IN_DIC 227568

const char TILES_TXT_PATH[] = "ScrabbleTiles.txt";

const std::map<char, int> letterValMap = {
    {'A', 1}, {'B', 3}, {'C', 3},
    {'D', 2}, {'E', 1}, {'F', 4},
    {'G', 2}, {'H', 4}, {'I', 1},
    {'J', 8}, {'K', 5}, {'L', 1},
    {'M', 3}, {'N', 1}, {'O', 1},
    {'O', 1}, {'P', 3}, {'Q', 10},
    {'R', 1}, {'S', 1}, {'T', 1},
    {'U', 1}, {'V', 4}, {'W', 4},
    {'X', 8}, {'Y', 4}, {'Z', 10}
};

enum Coordinate {
    X,
    Y
};

#define TOTALDIRECTIONS 4
enum Direction
{   
    NONE = -1,
    VERTICAL = 0,
    HORIZONTAL = 1
};

#endif // ASSIGN2_TYPES_H
