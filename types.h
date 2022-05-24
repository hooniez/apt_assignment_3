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

typedef std::set<std::string> configSettingType;
typedef std::shared_ptr<configSettingType> configSettingPtr;
const std::set<std::string> CONFIGOPTIONS = {"--ai", "--dictionary", "--hint"};

#define BOARD_LENGTH 15
#define NUM_PLAYER_TILES 7
#define NUM_PLAYERS 2
#define BINGO_ADDITIONAL_SCORE 50
#define TOTAL_WORDS_IN_DIC 228000
#define NUM_ELEMENTS_IN_GREEDY_MAP 2010000
#define NUM_ELEMENTS_IN_SORTED_MAP 228000

const char TILES_TXT_PATH[] = "ScrabbleTiles.txt";

const std::map<char, float> letterScoreMap = {
        {'A', 1}, {'B', 3}, {'C', 3},
        {'D', 2}, {'E', 1}, {'F', 4},
        {'G', 2}, {'H', 4}, {'I', 1},
        {'J', 8}, {'K', 5}, {'L', 1},
        {'M', 3}, {'N', 1}, {'O', 1},
        {'P', 3}, {'Q', 10},{'R', 1},
        {'S', 1}, {'T', 1}, {'U', 1},
        {'V', 4}, {'W', 4}, {'X', 8},
        {'Y', 4}, {'Z', 10}, {'|', 0.5}};

enum Coordinate {
    X,
    Y
};

enum DirectionFromPlacedTile {
    UP = -BOARD_LENGTH,
    LEFT = -1,
    RIGHT = 1,
    BOTTOM = BOARD_LENGTH
};

enum AdjacentTileIdx {
    TOPIDX = 0,
    RIGHTIDX = 1,
    BOTTOMIDX = 2,
    LEFTIDX = 3,
};

//const std::map<DirectionFromPlacedTile, AdjacentTileIdx> = {
//        {'A', 1}, {'B', 3}, {'C', 3},
//        {'D', 2}, {'E', 1}, {'F', 4},
//        {'G', 2}, {'H', 4}, {'I', 1},
//        {'J', 8}, {'K', 5}, {'L', 1},
//        {'M', 3}, {'N', 1}, {'O', 1},
//        {'P', 3}, {'Q', 10},{'R', 1},
//        {'S', 1}, {'T', 1}, {'U', 1},
//        {'V', 4}, {'W', 4}, {'X', 8},
//        {'Y', 4}, {'Z', 10}, {'|', 0.5}};

#define TOTALDIRECTIONS 4
enum Direction
{   
    NONE = -1,
    VERTICAL = 0,
    HORIZONTAL = 1
};

//const int Directions[TOTALDIRECTIONS] = {UP, RIGHT, BOTTOM,LEFT};

#endif // ASSIGN2_TYPES_H
