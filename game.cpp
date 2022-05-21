#include "game.h"
#include "filemanager.h"

Game::Game(const configSettingPtr& configSetting) : inSession(true), numRounds(0),
                                                    commandHandler(std::make_shared<CommandHandler>()),
                                                    dictionary(nullptr), wordBuilder(nullptr),
                                                    configSetting(configSetting)

{
    processConfigSetting(configSetting);
    initialiseBoard();
    initialiseTileBag();
    initialisePlayers();
    currPlayer = players[0];
}

Game::Game(
        configSettingPtr configSetting,
    std::vector<PlayerPtr> players,
    BoardPtr board,
    std::shared_ptr<TileBag> tileBag,
    PlayerPtr playerTurn):
                            currPlayer(playerTurn),
                            players(players),
                            board(board),
                            tileBag(tileBag),
                            inSession(true),
                            numRounds(0),
                            commandHandler(std::make_shared<CommandHandler>()),
                            configSetting(configSetting)

{
    for (const auto &option : *configSetting) {
        std::cout << option << std::endl;
    }
}

Game::~Game() = default;

void Game::processConfigSetting(const configSettingPtr& options) {
    // If options is not empty
    if (!options->empty()) {
        // Create a dictionary as all the options require it.
        dictionary = std::make_shared<Dictionary>("words");
        // If options includes --ai, create wordBuilder as a player
        if (options->count("--ai")) {
            // If options includes --hint, allow wordBuilder to give hints
            if (options->count("--hint")) {
                wordBuilder = std::make_shared<WordBuilder>("forwardAiMap",
                                                            "backwardAiMap",
                                                            "AI", true);
            } else
                // If options doesnt include --hint, do not allow wordBuilder to give hints
                wordBuilder = std::make_shared<WordBuilder>("forwardAiMap",
                                                            "backwardAiMap",
                                                            "AI", false);
        }
        // If options includes just --hint, do not create wordBuilder as a player
        // but allow it to give hints
    } else if (options->count("--hint")) {
        wordBuilder = std::make_shared<WordBuilder>("forwardAiMap",
                                                    "backwardAiMap", true);
    }
}

void Game::initialiseBoard()
{
    board = std::make_shared<Board>(BOARD_LENGTH, BOARD_LENGTH);
}

void Game::initialiseTileBag()
{
    tileBag = std::make_shared<TileBag>();
}

// Initialise the players by reading in their names, storing them in a
// vector, and dealing tiles to them
void Game::initialisePlayers()
{

    size_t numHumanPlayers = NUM_PLAYERS;
    // If the player plays against wordBuilder
    if (wordBuilder && wordBuilder->isPlaying)
        numHumanPlayers = 1;

    // Read in the players' names
    for (size_t i = 0; i < numHumanPlayers; ++i)
    {
        std::string playerName = readPlayerName(i);
        // Check whether playerName is identical to the previous
        // playerName(s).
        while (playerNameExists(playerName) && inSession)
        {
            std::cout << "\nPlease enter a non-existing player name."
                      << std::endl;
            playerName = readPlayerName(i);
        }

        players.push_back(std::make_shared<Player>(playerName));
        // If the player plays against wordBuilder
        if (wordBuilder && wordBuilder->isPlaying)
            players.push_back(wordBuilder);

        // Draw tiles from tileBag for each player's hand
        for (size_t j = 0; j < NUM_PLAYER_TILES; j++)
        {
            players[i]->drawOne(tileBag->dealOne());
        }
    }

//    std::cout << (*(*wordBuilder->forwardMap)["A"])['C'] << std::endl;



}

// Read a valid player name until it is all in uppercase characters
std::string Game::readPlayerName(size_t playerIdx)
{
    std::string playerName;
    bool readValid = false;
    while (!readValid && inSession)
    {
        // Prompt the player for a name
        std::ostringstream oss;
        oss << "\nEnter a name for player "
            << (playerIdx + 1) << " (uppercase character only)";
        utils::promptInput(oss.str());

        // Read the line the player has entered
        if (!std::getline(std::cin, playerName))
        {
            // If eof is read, terminate the game3.
            inSession = false;
        }
        else
        { // Otherwise, check whether the entered name is all uppercase
            readValid = utils::isAllUpper(playerName);
            // If the player's name contains a lowercase
            if (!readValid)
                std::cout << "Please use uppercase characters only"
                          << std::endl;
        }
    }
    return playerName;
}

// Check whether the name a player has just typed in is already typed by
// other player(s)
bool Game::playerNameExists(const std::string &playerName)
{
    bool nameExists = false;

    for (const auto &playerPtr : players)
    {
        std::string currPlayerName = playerPtr->getName();
        if (currPlayerName == playerName)
            nameExists = true;
    }
    return nameExists;
}

// Employs the game loop
void Game::play()
{
    std::cout << "\nLet's play!"
              << std::endl;

    PlayerPtr nextPlayer;
    bool isGameOver = false;
    while (inSession && !isGameOver)
    {
        printCurrTurn();
        readCommand();
        // The game ends when the tile bag is empty AND One player has no
        // more tiles in his/her hand OR passes his turn twice
        if (tileBag->isEmpty())
        {
            // Check if the player has passed this turn
            if (commandHandler->firstWord == "pass")
            {
                currPlayer->incrementNumPasses();
            }
            else
            {
                currPlayer->resetNumPasses();
            }
            // One player has no more tiles in their hands OR passes his
            // turn twice
            if (currPlayer->getHand()->getLength() == 0 ||
                currPlayer->hasPassedTwie())
            {
                isGameOver = true;
                printResult();
            }
        }
        ++numRounds;
        nextPlayer = players[numRounds % NUM_PLAYERS];

        if (nextPlayer->getName() == currPlayer->getName())
        {
            ++numRounds;
            nextPlayer = players[numRounds % NUM_PLAYERS];
        }
        currPlayer = nextPlayer;
    }
}

// readCommand reads only once for every command except for the place
// command, which is read multiple times until "place Done"
void Game::readCommand()
{
    // Assume each command is invalid at the entry point and validate it.
    commandHandler->isValid = false;
    // If isSaved is set to True within the loop when the player saves the
    // game, the loop is entered again so that he can type in a game-related
    // command (e.g. place, replace, etc)
    bool isSaved = false;
    // Every time commandHandler->getline(std::cin, currPlayer) is invoked,
    // commandHandler's content gets reset. if a command is deemed valid via
    // is<CommandType>CommandValid() for each type, commandHandler->isValid
    // is set to true within the function in CommandHandler class and the
    // command gets executed in Game class.
    while (!commandHandler->isValid || isSaved)
    {
        isSaved = false;
        if (commandHandler->getline(std::cin, currPlayer))
        {
            // numTiles the player has placed on the tile is tracked by the
            // sue of numTilesPlaced
            size_t numTilesPlaced = 0;
            // If firstWord is "place" and it is deemed valid
            if (commandHandler->firstWord == "place")
            {
                if (commandHandler->isPlaceCommandValid(board))
                {
                    executePlaceCommand(numTilesPlaced);
                    // Read lines until eof is read or the player types
                    // "place Done"
                    bool placeDoneTyped = false;
                    while (!placeDoneTyped &&
                           commandHandler->getline(std::cin, currPlayer))
                    {
                        auto firstWord = commandHandler->playerCommand[0];
                        auto secondWord = commandHandler->playerCommand[1];
                        if (firstWord == "place" && secondWord == "Done")
                        {
                            if (commandHandler->isPlaceDoneCommandValid(board, dictionary))
                                executePlaceDoneCommand(numTilesPlaced);
                            placeDoneTyped = true;
                        }
                        else
                        {
                            if (commandHandler->isPlaceCommandValid(board))
                                executePlaceCommand(numTilesPlaced);
                        }
                    }
                    // If the loop was exited due to the user typing eof
                    if (std::cin.eof())
                    {
                        inSession = false;
                    }
                }
            }
            else if (commandHandler->firstWord == "replace")
            {
                if (commandHandler->isReplaceCommandValid(tileBag))
                    executeReplaceCommand();
            }
            else if (commandHandler->firstWord == "pass")
            {
                if (commandHandler->isPassCommmandValid())
                    std::cout << "The player has passed the turn" << std::endl;
            }
            else if (commandHandler->firstWord == "save")
            {
                if (commandHandler->isSaveCommandValid())
                {
                    executeSaveCommand();
                    isSaved = true;
                }
            }
            else if (commandHandler->firstWord == "quit")
            {
                if (commandHandler->isQuitCommandValid())
                    inSession = false;
            }
            else
            {
                std::cout << "Invalid Input" << std::endl;
            }
        }
        else // eof read from command.getline(std::cin)
        {
            commandHandler->isValid = true;
            inSession = false;
        }
    }
}

// Execute a regular place command (This function can only be invoked once
// the command is validated)
void Game::executePlaceCommand(size_t &numTilesPlaced)
{
    std::string gridLoc = commandHandler->playerCommand[3];
    Letter letterSelected = commandHandler->playerCommand[1][0];
    // Set a tile on the board if legal and return the result
    board->setTile(gridLoc, currPlayer->getHand()->getTile(letterSelected));
    currPlayer->discardTile(letterSelected);
    ++numTilesPlaced;
}

// Execute "place Done" (This function can only be invoked once the command
// is validated)
void Game::executePlaceDoneCommand(size_t &numTilesPlaced)
{
    currPlayer->addScore(calculateScores());

    if (numTilesPlaced == NUM_PLAYER_TILES)
    {
        std::cout << "\nBINGO!!!" << std::endl;
        currPlayer->addScore(BINGO_ADDITIONAL_SCORE);
        std::cout << "Congrats!, have another " << BINGO_ADDITIONAL_SCORE
                  << " points!" << std::endl;
    }
    // while the tile bag is not empty and the player's hand is not full
    while (!tileBag->isEmpty() &&
           currPlayer->getHand()->getLength() < NUM_PLAYER_TILES)
    {
        currPlayer->drawOne(tileBag->dealOne());
    }
}

// Calculate scores from the generated words after "place Done"
int Game::calculateScores()
{
    auto words = board->getCurrWords();
    int score = 0;
    std::cout << currPlayer->getName() << " made the words: " << std::endl;
    for (auto it = words.cbegin(); it != words.cend(); ++it)
    {
        // Iterate characters in a word
        int currWordScore = 0;
        for (auto const &ch : *it)
        {
            currWordScore += letterScoreMap.at(ch);
            score += letterScoreMap.at(ch);
        }

        std::cout << " - " << *it << ": " << currWordScore << " points."
                  << std::endl;
    }
    std::cout << "For a score of " << score << ", Good job!"
              << std::endl;
    return score;
}

// Execute a replace command (This function can only be invoked once the
// command is validated)
void Game::executeReplaceCommand()
{
    // 1. Remove the tile from the players hand and place it in the tile
    //    bag.
    // 2. Draw a new tile from the tile bag and add it to the player’s hand
    Letter letterSelected = commandHandler->playerCommand[1][0];
    // currPlayer discards a tile from his hand and tileBag exchanges it
    // with a new tile, which currPlayer draws
    currPlayer->drawOne(tileBag->replace(
        currPlayer->discardTile(letterSelected)));
    std::cout << "Successfully Replaced" << std::endl;
}

// Execute a save command (This function can only be invoked once the
// command is validated)
void Game::executeSaveCommand()
{
    std::string fileName = commandHandler->playerCommand[1];

    try
    {
        files::saveGame(players, board, tileBag, currPlayer, configSetting, fileName);
        std::cout << "\nGame successfully saved\n"
                  << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cout << "unable to save game\n"
                  << std::endl;
    }
}

void Game::printCurrTurn()
{
    utils::printTurn(std::cout, currPlayer);
    utils::printScores(std::cout, players);
    utils::printBoard(std::cout, board);
    std::cout << std::endl;
    utils::printHand(std::cout, currPlayer->getHand());
}

// Print the result when the game ends (If more than one player shares the
// highest score, it is a draw)
void Game::printResult()
{
    // Print the command message regardless of whether it is a draw or a win
    std::cout << "Game over" << std::endl;
    for (const auto &currPlayer : players)
    {
        std::cout << "Score for " << currPlayer->getName()
                  << ": " << currPlayer->getScore() << std::endl;
    }

    // Sort the players in descending order by score
    std::sort(players.begin(), players.end(),
              [](const auto &player1, const auto &player2)
              { return player1->getScore() > player2->getScore(); });
    int topScore = players[0]->getScore();

    // Find the first player whose score is lower than topScore and return
    // the iterator pointing to it
    auto ret = std::find_if(players.begin(), players.end(),
                            [topScore](const auto &player)
                            { return player->getScore() < topScore; });

    auto beg = players.cbegin();
    // If there are more than one player who has the same top score
    if (ret - beg != 1)
    {
        std::cout << "It is a draw!" << std::endl;
        std::cout << "Player ";
        while (beg != ret)
        {
            std::cout << (*beg)->getName() << ((beg == ret - 1) ? " " : ", ");
            ++beg;
        }
        std::cout << "won!" << std::endl;
    }
    else
    { // If there is only one player who has won
        std::cout << "Player " << (*beg)->getName() << " won!" << std::endl;
    }
    std::cout << "Final Board State:" << std::endl;
    utils::printBoard(std::cout, board);
}

bool Game::isInSession()
{
    return inSession;
}

std::vector<PlayerPtr> Game::getPlayers()
{
    return players;
}

PlayerPtr Game::getCurrPlayer()
{
    return currPlayer;
}

BoardPtr Game::getBoard()
{
    return board;
}

std::shared_ptr<TileBag> Game::getTileBag()
{
    return tileBag;
}