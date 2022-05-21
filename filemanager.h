#ifndef ASSIGNMENT2_FILES_H
#define ASSIGNMENT2_FILES_H

#include <fstream>
#include <string>
#include "game.h"
#include "linkedlist.h"
#include "board.h"
#include "strstream"

namespace files
{
    void saveGame(std::vector<PlayerPtr> players,
                  BoardPtr board, TileBagPtr tileBag, PlayerPtr currPlayer,
                  configSettingPtr configSetting, const std::string &fileName);
    std::shared_ptr<Game> loadGame(std::string fileName);
    configSettingPtr parseConfigSetting(std::ifstream &in);
    PlayerPtr parsePlayerTurn(std::ifstream &in,
                              std::vector<PlayerPtr> players);
    LinkedListPtr<TilePtr> parseTiles(std::string tileStr);
    PlayerPtr parsePlayer(std::ifstream &in);
    BoardPtr parseBoard(std::ifstream &in);
    std::shared_ptr<TileBag> parseTileBag(std::ifstream &in);
    int createSubString(std::string &parentStr,
                        std::string &outputStr, unsigned int i);
    bool isNumber(std::string string);
}

#endif // ASSIGNMENT2_FILES_H