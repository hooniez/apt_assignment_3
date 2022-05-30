#include "utils.h"
#include <fstream>
#include "linkedlist.h"
#include "player.h"
#include "board.h"

void utils::printConfigSetting(std::ostream& out, configSettingPtr configSetting) {
    if (configSetting->empty())
        out << std::endl;
    else {
        for (const auto &configOption: *configSetting)
            out << configOption << " ";
        out << std::endl;
    }
}

void utils::printBoard(std::ostream& out, BoardPtr board)
{
    out << " ";
    for(unsigned int i = 0; i < board->getWidth(); i++)
    {
        if (i > 10)
        {
            out << "  " << i;
        } else
        {
            out << "   " << i;
        }

    }
    out << std::endl;
    out << "  ";
    for(unsigned int i = 0; i < board->getWidth(); i++)
    {
        out << "----";
    }
    out << "-";
    out << std::endl;

    // Utilise the indices stored in board->placedIndices for colour pritning
    // First put placedIndices in a set for membership check
    std::set<int> tempConatiner;
    auto placedIndices = board->getPlacedIndices();
    while (!placedIndices->empty()) {
        tempConatiner.insert(placedIndices->top());
        placedIndices->pop();
    }

    std::string colourCodingBeg;
    std::string colourCodingEnd;
    if (tempConatiner.size() == NUM_PLAYER_TILES) {
        colourCodingBeg = "\033[0;33m";
        colourCodingEnd = "\033[0m";
    } else {
        colourCodingBeg = "\033[0;32m";
        colourCodingEnd = "\033[0m";
    }

    // creating rows
    for(unsigned int i = 0; i < board->getLength(); i++)
    {
        char rowAsChar = (char)(i+65);
        out << rowAsChar <<" |";
        for(unsigned int k = 0; k < board->getWidth(); k++)
        {
            if(board->getBoard()[i][k] == nullptr)
            {
                out << "   |";
            } else
            {
                char tileLetter = board->getBoard()[i][k]->getLetter();

                // Colour print the letter using the single index notation
                out << " " <<
                           (tempConatiner.count((i*BOARD_LENGTH + k)) ?
                           colourCodingBeg : "") << tileLetter <<
                           (tempConatiner.count((i*BOARD_LENGTH + k)) ?
                           colourCodingEnd : "") << " |";
            }
        }
        out << std::endl;
    }

    // Put the elements in the set back in the priority queue to be processed by WordBuilder
    for (auto it = tempConatiner.begin(); it != tempConatiner.end(); ++it)
        placedIndices->push(*it);
}


void utils::printTiles(std::ostream& out, LinkedListPtr<TilePtr> tiles)
{
    TilePtr tile;
    for (int i = 0; i < tiles->getLength(); ++i)
    {
        tile = tiles->peekAt(i);
        if (i > 0)
        {
            out << ", ";
        }

        out << tile->getLetter()
            << "-"
            << tile->getValue();
    }
    out << std::endl;
}

void utils::printPlayer(std::ostream& out, PlayerPtr player)
{
    out << player->getName() << std::endl;
    out << player->getScore() << std::endl;
    utils::printTiles(out, player->getHand());
}

void utils::printTurn(std::ostream& out, PlayerPtr player) {
    out << "\n" << player->getName() << ", it's your turn" << std::endl;
}

void utils::printScores(std::ostream& out, std::vector<PlayerPtr>& players) {
    for (const auto &playerPtr : players) {
        out << "Score for " << playerPtr->getName()
            << ": " << playerPtr->getScore() << std::endl;
    }
}

void utils::printHand(std::ostream& out, LinkedListPtr<TilePtr> tiles) {
    out << "Your hand is " << std::endl;
    printTiles(out, tiles);
}

void utils::promptInput(const std::string &prompt) {
    std::cout << prompt << std::endl;
    std::cout << "> ";
}

void utils::promptInput() {
    std::cout << "> ";
}

bool utils::isAllUpper(std::string& str) {
    bool isUpper = true;

    if (str.size() == 0)
        isUpper = false;
    else {
        for (const auto &ch : str) {
            if (!isupper(ch))
                isUpper = false;
        }
    }

    return isUpper;
}