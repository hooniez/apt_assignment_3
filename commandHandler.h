#ifndef ASSIGNMENT2_COMMANDHANDLER_H
#define ASSIGNMENT2_COMMANDHANDLER_H

#include <string>
#include <vector>
#include "utils.h"
#include "player.h"
#include "tilebag.h"
#include "Dictionary.h"

/* CommandHandler contains all the necessary and resultant data members
 * when and after reading a command
 * It also contains validation checkers that can be run on each command.
 * One instance of CommandHandler is used throughout the program's life
 * as it is more efficient not to create and destroy an instance every time
 * a command is typed.
 * A commandHandler object is shared by the players.
 */
class CommandHandler {
public:
    CommandHandler();
    ~CommandHandler();
    std::istream& getline(std::istream&, PlayerPtr);
    // Reset the re-used command object when reading the next command
    void reset();
    bool isPlaceCommandValid(BoardPtr board);
    bool isPlaceDoneCommandValid(BoardPtr board, DictionaryPtr dictionary);
    bool isReplaceCommandValid(TileBagPtr tileBag);
    bool isPassCommmandValid();
    bool isSaveCommandValid();
    bool isQuitCommandValid();
    std::vector<std::string> playerCommand;
    std::string firstWord;
    bool isValid;
    size_t numWords;
    PlayerPtr currPlayer;
};
typedef std::shared_ptr<CommandHandler> CommandHandlerPtr;

#endif //ASSIGNMENT2_COMMANDHANDLER_H
