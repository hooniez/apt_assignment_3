#ifndef ASSIGNMENT2_GAME_H
#define ASSIGNMENT2_GAME_H

#include <vector>
#include <sstream>
#include <random>
#include <algorithm>
#include <memory>
#include <tuple>

#include "tile.h"
#include "tilebag.h"
#include "board.h"
#include "utils.h"
#include "commandHandler.h"
#include "player.h"
#include "Dictionary.h"
#include "types.h"
#include "WordBuilder.h"
#include "GreedyMap.h"

/*
 * Game's role is to initialise every object that makes up Scrabble (e.g.
 * players, board, tileBag) It also runs the game loop by taking in
 * user-typed commands with the help of commandHandler, which checks whether
 * they are in valid formats. Game itself executes the validated commands
 * and keeps track of the player scores, numRounds, etc.
 */

class Game
{
public:
    Game(configSettingPtr );
    Game(configSettingPtr configSetting,
        std::vector<PlayerPtr> players,
        BoardPtr board,
        std::shared_ptr<TileBag> tileBag,
        PlayerPtr playerTurn,
        WordBuilderPtr wordBuilder,
        DictionaryPtr dictionary);
    Game(configSettingPtr  configSetting,
         WordBuilderPtr  wordBuilder1,
         WordBuilderPtr  wordBuilder2);
    Game(configSettingPtr  configSetting,
         WordBuilderPtr  wordBuilder1,
         WordBuilderPtr  wordBuilder2,
         std::vector<PlayerPtr> players,
         BoardPtr board,
         std::shared_ptr<TileBag> tileBag,
         PlayerPtr playerTurn,
         DictionaryPtr dictionary);



    ~Game();
    // Process options read from the command-line and initialise data members
    void processConfigSetting();
    void initialiseBoard();
    void initialiseTileBag();
    /*
     * Initialise the players by reading in their names, storing them in a
     * vector, and dealing tiles to them
     */
    void initialisePlayers();
    // Read a valid player name until it is all in uppercase characters
    std::string readPlayerName(size_t playerIdx);
    /*
     * Check whether the name a player has just typed in is already typed by
     * other player(s)
     */
    bool playerNameExists(const std::string &playerName);
    // Employs the game loop
    bool play();
    /*
     * readCommand reads only once for every command except for the place
     * command, which is read multiple times until "place Done"
     */
    void readCommand();
    /*
     * Execute a regular place command (This function can only be invoked
     * once the command is validated)
     */

    void executePlaceCommand(size_t &numTilesPlaced);
    /*
     * Execute "place Done" (This function can only be invoked once the
     * command is validated)
     */
    void executePlaceDoneCommand(size_t &numTilesPlaced);
    // Calculate scores from the generated wordsInQueue after "place Done"
    int calculateScores();
    /*
     * Execute a replace command (This function can only be invoked once the
     * command is validated)
     */
    void executeReplaceCommand();
    /*
     * Execute a save command (This function can only be invoked once the
     *command is validated)
     */
    void executeSaveCommand();
    void printCurrTurn();
    /*
     * Print the result when the game ends (If more than one player shares
     * the highest score, it is a draw)
     */
    void printResult();
    bool isInSession();

    std::vector<PlayerPtr> getPlayers();
    PlayerPtr getCurrPlayer();
    BoardPtr getBoard();
    std::shared_ptr<TileBag> getTileBag();


private:
    PlayerPtr currPlayer;
    std::vector<PlayerPtr> players;
    BoardPtr board;
    TileBagPtr tileBag;
    bool inSession;
    size_t numRounds;
    CommandHandlerPtr commandHandler;
    /*                          Milestone 1 & 2 Above
     * -------------------------------------------------------------------
     * -------------------------------------------------------------------
     * -------------------------------------------------------------------
     * -------------------------------------------------------------------
     *                          Milestone 3 & 4 Below
     */
    DictionaryPtr dictionary;
    WordBuilderPtr wordBuilder;
    WordBuilderPtr wordBuilder2;
    configSettingPtr configSetting;

    void executePlaceCommand(const std::map<std::string, char>& tileIndices);

};

#endif // ASSIGNMENT2_GAME_H