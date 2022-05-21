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
void mainMenu(const configSettingPtr&);
void printCredits();
void startNewGame(const configSettingPtr&);
bool loadGame();

/*                          Milestone 1 & 2 Above
 * -------------------------------------------------------------------
 * -------------------------------------------------------------------
 * -------------------------------------------------------------------
 * -------------------------------------------------------------------
 *                          Milestone 3 & 4 Below
 */

configSettingPtr readConfig(int, const char *[]);
configSettingPtr readConfigUntilValid();
void printConfigOptions();

int main(int argc, const char *argv[])
{
    configSettingPtr optionsValid = readConfig(argc, argv);

    // If the entered options are invalid, make the user type again until they are valid
    if (!optionsValid)
        optionsValid = readConfigUntilValid();

    std::cout << "\nWelcome to Scrabble!" << std::endl;
    std::cout << "-------------------" << std::endl;

    mainMenu(optionsValid);

    std::cout << "\nGoodbye" << std::endl;


    return EXIT_SUCCESS;
}

void mainMenu(const configSettingPtr &options)
{
    bool terminate = false;
    std::string input;
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
            startNewGame(options);
            terminate = true;
        }
        else if (selection == LOAD)
        {
            // TODO: implement options
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

void startNewGame(const configSettingPtr &options)
{
    std::cout << "\nStarting a New Game"
              << std::endl;
    Game game(options);
    // In case any of the players types in eof for the name
    if (game.isInSession())
        game.play();
}

bool loadGame()
{
    bool loaded = false;
    std::string fileName;
    std::shared_ptr<Game> game = nullptr;

    std::cout << "Enter the filename from which to load a game" << std::endl;
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

/*                          Milestone 1 & 2 Above
 * -------------------------------------------------------------------
 * -------------------------------------------------------------------
 * -------------------------------------------------------------------
 * -------------------------------------------------------------------
 *                          Milestone 3 & 4 Below
 */

// Read an option(s) and return a set of correctly formatted options
// Otherwise, return nullptr.
configSettingPtr readConfig(int argc, const char *argv[]) {
    configSettingPtr options = nullptr;

    // If no options are typed
    if (argc == 1)
        // Return an empty set rather than nullptr
        options = std::make_shared<configSettingType>();

    // Check if options specified are fewer than the total number of available options
    // argc - 1 excludes the program name
    if (argc - 1 <= CONFIGOPTIONS.size()) {
        for (int i = 1; i < argc; ++i) {
            if (CONFIGOPTIONS.count(argv[i]) == 1) {
                if (!options)
                    options = std::make_shared<configSettingType>();
                options->insert(std::string(argv[i]));
            } else { // If any one of the options is invalid
                options = nullptr;
            }
        }
    }
    return options;
}

configSettingPtr readConfigUntilValid() {
    configSettingPtr optionsValid = nullptr;
    printConfigOptions();

    std::string userOption;
    bool done = false;
    // While options are not valid OR the user is not done
    while (!optionsValid || !done) {
        std::cout << std::endl;
        std::cout << "Please type in an option to enable (type --done when finished)" << std::endl;
        utils::promptInput();
        std::getline(std::cin, userOption);
        // If the user correctly types in an option
        if (CONFIGOPTIONS.count(userOption) == 1) {
            // If no valid option has been entered
            if (!optionsValid)
                optionsValid = std::make_shared<configSettingType>();
            optionsValid->insert(userOption);

            // Only if optionsValid has a valid option entered, enter the condition below
            // If the user has typed "--done"", exit the loop
        } else if (optionsValid && (userOption == "--done")) {
            done = true;
        } else {
            printConfigOptions();
        }
    }
    return optionsValid;
}

// Give information on what command-line options are available
void printConfigOptions() {
    std::cout << "Invalid Option(s)\n" << std::endl;
    std::cout << "Available options are:" << std::endl;
    std::cout << "To play against a computer: --ai" << std::endl;
    std::cout << "To check whether the entered words are valid: --dictionary" << std::endl;
    std::cout << "To get hints on where to place tiles: --hint" << std::endl;
}