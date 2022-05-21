#ifndef ASSIGNMENT2_UTILS_H
#define ASSIGNMENT2_UTILS_H

#include <fstream>
#include "linkedlist.h"
#include "board.h"
#include "player.h"
#include "board.h"

namespace utils {
    void printConfigSetting(std::ostream& out, configSettingPtr configSetting);
    void printBoard(std::ostream& out, BoardPtr board);
    void printTiles(std::ostream& out, LinkedListPtr<TilePtr> tiles);
    void printPlayer(std::ostream& out, PlayerPtr player);
    void printTurn(std::ostream& out, PlayerPtr player);
    void printScores(std::ostream& out, std::vector<PlayerPtr>& players);
    void printHand(std::ostream& out, LinkedListPtr<TilePtr> tiles);
    void promptInput();
    void promptInput(const std::string &);
    bool isAllUpper(std::string& str);
}


#endif // ASSIGNMENT2_UTILS_H