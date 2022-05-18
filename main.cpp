#include "linkedlist.h"
#include "player.h"
#include "types.h"
#include "game.h"
#include "board.h"
#include "filemanager.h"
#include "utils.h"

#include <iostream>
#include <vector>
#include <utility>
#include <fstream>

using std::pair;
using std::string;
using std::vector;

void printMenu();
void mainMenu();
void printCredits();
void startNewGame();
bool loadGame();

int main(void)
{
    std::cout << "Welcome to Scrabble!" << std::endl;
    std::cout << "-------------------" << std::endl;

    mainMenu();
    //    std::shared_ptr<Game> game = files::loadGame();
    //    files::saveGame(game);

    std::cout << "\nGoodbye" << std::endl;
    return EXIT_SUCCESS;
}

void mainMenu()
{
    bool terminate = false;
    std::string input = "";
    int selection = 0;
    while (!terminate)
    {
        printMenu();
        input = "";
        selection = 0;
        std::cout << "> ";
        if (!std::getline(std::cin, input))
        {
            terminate = true;
        }

        if (input.length() == 1 && std::isdigit(input[0]))
        {
            selection = std::stoi(input);
        }

        if (selection == NEW)
        {
            startNewGame();
            terminate = true;
        }
        else if (selection == LOAD)
        {
            terminate = loadGame();
        }
        else if (selection == CREDIT)
        {
            printCredits();
        }
        else if (selection == QUIT)
        {
            terminate = true;
        }
        else if (!terminate)
        {
            std::cout << "Invalid Input - 1" << std::endl;
        }
    }
}

void printCredits()
{
    vector<pair<string, string>> contributors =
        {
            {"Cal Lamshed", "s3706239"},
            {"Yongjie Shi", "s3759362"},
            {"Max Foord", "s3888349"},
            {"Myeonghoon Sun", "s3774430"}};

    std::cout << "\n----------------------------------" << std::endl;
    for (pair<string, string> contributor : contributors)
    {
        std::cout << "Name: " << contributor.first << std::endl
                  << "Student ID: " << contributor.second << std::endl
                  << contributor.second
                  << "@student.rmit.edu.au" << std::endl
                  << std::endl;
    }
    std::cout << "----------------------------------\n"
              << std::endl;
}

void startNewGame()
{
    std::cout << "\nStarting a New Game"
              << std::endl;
    Game game;
    // In case any of the players types in eof for the name
    if (game.isInSession())
        game.play();
}

bool loadGame()
{
    bool loaded = false;
    std::string fileName;
    std::shared_ptr<Game> game = nullptr;

    std::cout << "Enter the filename from which load a game" << std::endl;
    std::cout << "> ";
    std::getline(std::cin, fileName);

    game = files::loadGame(fileName);

    if (game != nullptr)
    {
        loaded = true;
        game->play();
    }
    else
    {
        std::cout << "unable to load the file\n"
                  << std::endl;
    }

    return loaded;
}

void printMenu()
{
    std::cout << "Menu" << std::endl
              << "----" << std::endl
              << "1. New Game" << std::endl
              << "2. Load Game" << std::endl
              << "3. Credits (Show student information)" << std::endl
              << "4. Quit\n"
              << std::endl;
}
