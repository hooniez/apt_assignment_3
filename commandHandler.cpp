#include <sstream>
#include "commandHandler.h"

/*
 * Assume every command handled by CommandHandler is invalid and check
 * whether it is valid
 */
CommandHandler::CommandHandler() : isValid(false), numWords(0)
{
}

CommandHandler::~CommandHandler()
{
}

/*
 * CommandHandler's getline reads a line using: std::getline(std::cin,
 * std::string).
 */
std::istream &CommandHandler::getline(std::istream &in,
                                      PlayerPtr currPlayer)
{
    /*
     * Make sure to reset isValid and userWord in case of re-reading a
     *  command
     */
    utils::promptInput();
    reset();
    this->currPlayer = currPlayer;

    std::string userLine;
    std::getline(in, userLine);
    std::istringstream iss(userLine);
    std::string userWord;

    // Store all the words delimited by whitespace in userLine
    while (iss >> userWord)
    {
        playerCommand.push_back(userWord);
    }
    // Store the first word in a variable called commandWord
    if (playerCommand.size() > 0)
        firstWord = playerCommand[0];
    numWords = playerCommand.size();
    return in;
}

// Reset the re-used command object when reading the next command
void CommandHandler::reset()
{
    isValid = false;
    playerCommand.clear();
    numWords = 0;
    firstWord = "";
}

bool CommandHandler::isPlaceCommandValid(const BoardPtr &board)
{
    /*
     * Checking whether the first word is "place" is necessary as multiple
     * place commands are read
     */
    if (firstWord != "place")
    {
        std::cout << "Invalid Input: To end your turn use: place Done"
                  << std::endl;
    }
    else if (numWords == 4)
    {
        std::string secondWord = playerCommand[1];
        std::string thirdWord = playerCommand[2];
        std::string fourthWord = playerCommand[3];
        LinkedListPtr<TilePtr> currPlayerHand = currPlayer->getHand();

        /*
         * Checking different error conditions, and associating error
         * Messages.
         */

        if (secondWord < "A" || secondWord > "Z")
        {
            std::cout << "Invalid Input: place [A-Z] at <grid location>"
                      << std::endl;
            // If the tile the player is placing does not exist in his hand.
        }
        else if (!currPlayerHand->hasLetter(secondWord[0]))
        {
            std::cout
                << "Invalid Input: You can only place a tile from your hand"
                << std::endl;
        }
        else if (thirdWord != "at")
        {
            std::cout << "Invalid Input: place <tile> [at] <grid location>"
                      << std::endl;
        }
        else if (fourthWord.size() > 3 || fourthWord.size() < 2)
        {
            std::cout << "Invalid Input: place <tile> at [<grid location>]"
                      << std::endl;
            std::cout << "Grid location's range for the row [A-O]"
                      << std::endl;
            std::cout << "Grid location's range for the column [0-14]"
                      << std::endl;
            std::cout << "e.g. place A at C13"
                      << std::endl;
        }
        else if (!board->checkIfValid(fourthWord))
        {
            /* Checks if the gridLoc is valid and within the bounds to place
             * a tile
             */
            std::cout << "Invalid Input: Place <tile> at [A-P0-14]"
                      << std::endl;
        }
        else if (!board->checkIfEmpty(fourthWord))
        {
            std::cout << "Invalid Input: That grid location is already taken."
                      << std::endl;
        }
        else
        {
            isValid = true;
        }
    }
    else
    {
        std::cout << "Invalid Input: Place <tile> [at] <grid location>"
                  << std::endl;
    }

    return isValid;
}

bool CommandHandler::isPlaceDoneCommandValid(const BoardPtr &board)
{
    if (numWords == 2)
    {
        std::string secondWord = playerCommand[1];
        // Check whether the words are "place Done"
        if (secondWord != "Done")
        {
            std::cout << "Invalid Input: To end your turn use: place Done"
                      << std::endl;
        }
        else
        {
            /*
             * board checks whether placed letters are valid and returns a
             * vector of pointers to placed tiles back if invalid. If the
             * placed tiles are legal, the first element of the returned
             * vector should contain nullptr, implying there are no tiles
             * are illegal
             */
            auto returnedTiles = board->checkLetterPosValidity();
            if (returnedTiles[0] != nullptr)
            {
                if (board->isEmpty())
                {
                    std::cout
                        << "Invalid Input: Make sure your word includes the"
                        << std::endl;
                    std::cout
                        << "center of the board (H7),"
                        << " and that all letters are connected."
                        << std::endl;
                }
                else
                {
                    std::cout
                        << "Invalid Input:"
                        << " Make sure that your word connects to"
                        << std::endl;
                    std::cout
                        << "an existing one, and that"
                        << " all the letters form a single"
                        << std::endl;
                    std::cout
                        << "unbroken line."
                        << std::endl;
                }
                // Put the returned tiles back in the player's hand
                for (const auto &placedTile : returnedTiles)
                {
                    currPlayer->getHand()->append(placedTile);
                }
            }
            else
            {
                isValid = true;
            }
        }
    }
    return isValid;
}

bool CommandHandler::isReplaceCommandValid(const TileBagPtr &tileBag)
{
    if (numWords != 2)
    {
        std::cout << "Invalid Input: replace [A-Z] (A letter in your hand)."
                  << std::endl;
    }
    else
    {
        std::string secondWord = playerCommand[1];
        LinkedListPtr<TilePtr> currPlayerHand = currPlayer->getHand();

        // Check if the second word is not a letter within the range
        if (secondWord < "A" || secondWord > "Z")
        {
            std::cout << "Invalid Input: place [A-Z] at <grid location>"
                      << std::endl;
        }
        else if (!currPlayerHand->hasLetter(secondWord[0]))
        {
            /*
             * If the tile that the player is placing does not exist in the
             * hand.
             */
            std::cout << "Invalid Input: place [A-Z] at <grid location>"
                      << std::endl;
            std::cout << "You can only place a tile from your hand."
                      << std::endl;
        }
        else if (tileBag->isEmpty())
        {
            std::cout
                << "Invalid Input: There are no more tiles in the tile bag. :("
                << std::endl;
        }
        else
        {
            isValid = true;
        }
    }
    return isValid;
}

bool CommandHandler::isPassCommmandValid()
{
    if (numWords != 1)
    {
        std::cout << "Invalid Input: pass" << std::endl;
    }
    else
    {
        isValid = true;
    }
    return isValid;
}

bool CommandHandler::isSaveCommandValid()
{
    if (numWords != 2)
    {
        std::cout << "Invalid Input: save <filename>" << std::endl;
    }
    else
    {
        isValid = true;
    }
    return isValid;
}

bool CommandHandler::isQuitCommandValid()
{
    if (numWords != 1)
    {
        std::cout << "Invalid Input: pass" << std::endl;
    }
    else
    {
        isValid = true;
    }
    return isValid;
}