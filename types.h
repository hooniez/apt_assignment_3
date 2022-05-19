//
// Created by Myeonghoon Sun on 19/5/2022.
//

#ifndef ASSIGNMENT3_TYPES_H
#define ASSIGNMENT3_TYPES_H

#include <map>

enum Input
{
    NEW = 1,
    LOAD,
    CREDIT,
    QUIT
};

#define BOARD_LENGTH 15
#define NUM_PLAYER_TILES 7
#define NUM_PLAYERS 2
#define BIGO_ADDITIONAL_SCORE 50
#define NUM_ELEMENTS_IN_AI_MAP 2009525

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

#define TOTALDIRECTIONS 4
enum Direction
{
    NONE = -1,
    VERTICAL = 0,
    HORIZONTAL = 1
};

#endif //ASSIGNMENT3_TYPES_H
