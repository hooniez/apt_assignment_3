//
// Created by Myeonghoon Sun on 21/5/2022.
//

#include "filemanager.h"

#include <fstream>
#include <vector>
#include <string>
#include <iostream>
#include "utils.h"
#include "filemanager.h"
#include "game.h"
#include "player.h"
#include "linkedlist.h"
#include "tile.h"
#include "types.h"
#include "board.h"

using std::string;
using std::vector;

void files::saveGame(
        std::vector<PlayerPtr> players,
        BoardPtr board,
        TileBagPtr tileBag,
        PlayerPtr currPlayer,
        configSettingPtr configSetting,
        const std::string &fileName)
{
    std::ofstream outFile;
    outFile.open(fileName + ".save", std::ofstream::out);

    // Write configuration settings first
    utils::printConfigSetting(outFile, configSetting);

    for (const auto &player : players)
    {
        utils::printPlayer(outFile, player);
    }
    utils::printBoard(outFile, board);
    utils::printTiles(outFile, tileBag->getBag());
    outFile << currPlayer->getName();
}

std::shared_ptr<Game> files::loadGame(std::string fileName)
{
    bool invalid = false;

    std::ifstream inFile;
    // if the opening of the file fails, the load game is invalid
    try
    {
        inFile.open(fileName + ".save");
    }
    catch (const std::exception &e)
    {
        invalid = true;
    }

    // Read configuration settings first
    configSettingPtr configSetting;
    configSetting = files::parseConfigSetting(inFile);

    std::vector<PlayerPtr> players;
    PlayerPtr player;
    size_t numHumanPlayers = NUM_PLAYERS;
    if (configSetting->count("--ai")) {
        numHumanPlayers = 1;
    }

    // Since the human player will always play first if playing against AI
    // read him first.
    for (int i = 0; i < numHumanPlayers; ++i)
    {

        player = files::parsePlayer(inFile);
        if (player != nullptr)
        {
            players.push_back(player);
        }
        else
        {
            invalid = true;
        }
    }

    WordBuilderPtr wordBuilder = nullptr;
    // Read AI if --ai is configured
    if (configSetting->count("--ai")) {
        if (configSetting->count("--hint")) {
            wordBuilder = files::parseWordBuilder(inFile, true);
        } else {
            wordBuilder = files::parseWordBuilder(inFile, false);
        }

        if (wordBuilder != nullptr)
        {
            players.push_back(wordBuilder);
        }
        else
        {
            invalid = true;
        }
    } else if (configSetting->count("--hint")) {
        // If Ai is not a player, but should give a hint,
        // Create wordBuilder with board set to nullptr since board is read after
        // the players are parsed. Set board once it is parsed below.
        wordBuilder = std::make_shared<WordBuilder>("forwardGreedyMap",
                                                    "backwardGreedyMap",
                                                    "sortedMap",
                                                    nullptr,
                                                    nullptr,
                                                    false);

    }

    BoardPtr board = files::parseBoard(inFile);
    // Now that board is parsed, allow wordBuilder, if it exists, to store board as its data member
    if (wordBuilder)
        wordBuilder->setBoard(board);

    DictionaryPtr dictionary = nullptr;
    if (configSetting->count("--dictionary")) {
        dictionary = std::make_shared<Dictionary>("words", true);
    } else {
        if (wordBuilder) {
            dictionary = std::make_shared<Dictionary>("words", false);
        }
    }

    if (wordBuilder)
        wordBuilder->setDictionary(dictionary);

    std::shared_ptr<TileBag> tileBag = parseTileBag(inFile);

//    // Debugging
//    while (!tileBag->isEmpty()) {
//        auto something = tileBag->getBag()->dequeue();
//        std::cout << something->getLetter() << std::endl;
//        std::cout << something->getValue() << std::endl;
//    }


    PlayerPtr playerTurn = parsePlayerTurn(inFile, players);

    std::shared_ptr<Game> game = nullptr;

    /*
     * if any of the individual parsing methods fail, then the game is
     * unable to be loaded
     */
    if (!invalid &&
        board != nullptr &&
        tileBag != nullptr &&
        playerTurn != nullptr)
    {
        if (configSetting->empty()) {
            game = std::make_shared<Game>(configSetting, players, board, tileBag, playerTurn, nullptr, nullptr);
        } else {
            game = std::make_shared<Game>(configSetting, players, board, tileBag, playerTurn, wordBuilder, dictionary);
        }

    }

    return game;
}

/*
 * Read the first line of the file to find out the configuration setting of the game
 * If it is a regular game without any extra configuration, the file starts with an empty line
 * Otherwise, such configuration options as --ai, --dictionary, or --hint will be read
 * on the first line
 */
configSettingPtr files::parseConfigSetting(std::ifstream &in) {
    configSettingPtr configSetting = std::make_shared<configSettingType>();
    std::string line;
    std::getline(in, line);
    std::istringstream settings(line);
    std::string option;
    while (settings >> option) {
        configSetting->insert(option);
    }
    return configSetting;
}

/*
 * compares the string against the parsed players names and returns the one
 * that matches to be set as the current players turn
 */
PlayerPtr files::parsePlayerTurn(std::ifstream &in,
                                 std::vector<PlayerPtr> players)
{
    PlayerPtr playerTurn = nullptr;
    std::string playerName;
    std::getline(in, playerName);
    if (playerName.length() > 0)
    {
        for (const auto &player : players)
        {
            if (player->getName() == playerName)
            {
                playerTurn = player;
            }
        }
    }

    return playerTurn;
}

std::shared_ptr<TileBag> files::parseTileBag(std::ifstream &in)
{
    string inStr;
    std::getline(in, inStr);

    LinkedListPtr<TilePtr> tiles = nullptr;

    tiles = files::parseTiles(inStr);

    std::shared_ptr<TileBag> tb;

    // allows for a blank line to represent an empty tile bag
    if (tiles != nullptr)
    {
        tb = std::make_shared<TileBag>(tiles);
    }
    else
    {

        tb = std::make_shared<TileBag>(
                std::make_shared<LinkedList<TilePtr>>());
    }
    return tb;
}

// check if a given string is a number
bool files::isNumber(std::string string)
{
    bool isNumber = true;

    if (string.length() < 1)
    {
        isNumber = false;
    }
    for (char c : string)
    {
        if (!std::isdigit(c))
        {
            isNumber = false;
        }
    }
    return isNumber;
}

PlayerPtr files::parsePlayer(std::ifstream &in)
{
    // parse the players name
    string name;
    std::getline(in, name);

    // parse the score
    string scoreStr;
    std::getline(in, scoreStr);
    int score = 0;

    // parse the tiles
    string tileStr;
    std::getline(in, tileStr);
    LinkedListPtr<TilePtr> hand = parseTiles(tileStr);

    PlayerPtr player = nullptr;

    // check if the parsed player attributes are valid
    if (name.length() > 0 &&
        scoreStr.length() > 0 &&
        isNumber(scoreStr) &&
        hand != nullptr)
    {
        score = std::stoi(scoreStr);
        player = std::make_shared<Player>(name, score, hand);
    }

    // returns nullptr if one of the attributes is invalid
    return player;
}

WordBuilderPtr files::parseWordBuilder(std::ifstream &in, bool canGiveHints) {
    // parse the WordBuilder's name
    string name;
    std::getline(in, name);

    // parse the score
    string scoreStr;
    std::getline(in, scoreStr);
    int score = 0;

    // parse the tiles
    string tileStr;
    std::getline(in, tileStr);
    LinkedListPtr<TilePtr> hand = parseTiles(tileStr);

    WordBuilderPtr wordBuilder = nullptr;

    // check if the parsed player attributes are valid
    if (name.length() > 0 &&
        scoreStr.length() > 0 &&
        isNumber(scoreStr) &&
        hand != nullptr)
    {
        score = std::stoi(scoreStr);
        wordBuilder = std::make_shared<WordBuilder>("forwardGreedyMap",
                                                    "backwardGreedyMap",
                                                    "sortedMap",
                                                    nullptr,
                                                    name,
                                                    score,
                                                    hand,
                                                    nullptr,
                                                    canGiveHints);
    }

    // returns nullptr if one of the attributes is invalid
    return wordBuilder;
}

BoardPtr files::parseBoard(std::ifstream &in)
{
    bool invalid = false;
    string rowStr;
    std::getline(in, rowStr);

    string colCntStr = "";
    unsigned int maxSize = 0;

    for (unsigned int i = 0; i < rowStr.length(); ++i)
    {
        if (isdigit(rowStr[i]))
        {
            colCntStr = "";
            i = files::createSubString(rowStr, colCntStr, i);
        }
    }

    /*
     * validate that the parsed row returns a number, if it does not then
     * the board save is invalid
     */

    if (isNumber(colCntStr))
    {
        maxSize = std::stoi(colCntStr) + 1;
    }
    else
    {
        invalid = true;
    }

    BoardPtr board = std::make_shared<Board>(maxSize, maxSize);

    // skip the board "----" seperator
    std::getline(in, rowStr);
    bool addedTileFlag = false;
    char rowLabel;
    std::string coord = "";

    for (unsigned int i = 0; i < maxSize && !invalid; ++i)
    {
        std::getline(in, rowStr);
        rowLabel = rowStr[0];

        /*
         * if the row label is not within A-Z in sequential order then the
         * board is invalid
         */
        if ((unsigned int)rowLabel != (i + 65))
        {
            invalid = true;
        }

        /*
         * parse the board, creating tile objects from the letters found
         * increments the tiles coord position by one when it crosses the |
         * char that represents the boundary of a board space
         */
        for (unsigned int j = 3, k = 0;
             j < rowStr.length() && k < maxSize; ++j)
        {
            if (rowStr[j] == '|')
            {
                ++k;
            }
            coord = rowLabel + std::to_string(k);
            if (isalpha(rowStr[j]))
            {
                board->setTile(coord, std::make_shared<Tile>(rowStr[j]));
                addedTileFlag = true;
            }
        }
    }
    // updating board depending on if the loaded file is empty or not.
    board->clearCoords();

    if (addedTileFlag)
    {
        board->setEmptyStatus(false);
    }

    if (invalid)
    {
        board = nullptr;
    }
    return board;
}

LinkedListPtr<TilePtr> files::parseTiles(std::string tileStr)
{
    LinkedListPtr<TilePtr> tiles = std::make_shared<LinkedList<TilePtr>>();
    string letter = "";
    string val = "";

    /*
     * if the first section of the string is not a string representation of
     * a tile eg "S-4" then the tile bag can be parsed if there is a partial
     * tile representation eg"S-" that that tile will be ignored
     */
    if (
            tileStr.length() >= 3 &&
            std::isalpha(tileStr[0]) &&
            tileStr[1] == '-' &&
            std::isdigit(tileStr[2]))
    {
        std::istringstream iss(tileStr);
        std::string whitespaceDiscarded;
        while (std::getline(iss, letter, '-') && std::getline(iss, val, ',') && std::getline(iss, whitespaceDiscarded, ' ')) {
            tiles->append(std::make_shared<Tile>(
                    letter[0], std::stoi(val)));
        }
    }

    if (tiles->getLength() < 1)
    {
        tiles = nullptr;
    }

    return tiles;
}

//
/*
 * creates a substring of consecutive alphanumeric values returns the index
 * of the last checked character in the string
 */
int files::createSubString(string &parentStr,
                           string &outputStr, unsigned int i)
{

    char c = parentStr[i];

    // debugging
//    if (c == 'Z')
//        std::cout << "here";

    while (std::iswalnum(c) && i < parentStr.length())
    {
        outputStr.push_back(c);
        ++i;
        c = parentStr[i];
    }
    return i;
}