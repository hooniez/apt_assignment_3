#ifndef ASSIGNMENT2_BOARD_H
#define ASSIGNMENT2_BOARD_H
#include <vector>
#include <sstream>
#include <random>
#include <algorithm>
#include <memory>
#include <tuple>

#include "types.h"
#include "tile.h"
#include "player.h"

class Board {


public:

    /*
     * creates a board with a specified width, height and if it is empty
     * if misEmpty set to false will not do Empty board error checking
     */
    Board(unsigned int width, unsigned int height, bool misEmpty=true);
    ~Board();

    /* 
     * returns true if any tiles are adjacent, modifies tiles
     * array to pointers to the tiles. order is above, right, below, left
     * Checks if the gridLoc is valid and within the bounds to place a tile
     * pre-condition: gridLoc is a valid location
     */
    bool checkIfValid(const std::string &gridLoc);

    /* 
     * Checks if the gridLoc is empty to place a tile
     * pre-condition: gridLoc is a valid location
     */
    bool checkIfEmpty(const std::string &gridLoc);

    /* 
     * Setter method for tile
     * pre-condition: is a valid position format, [A-O1-14]
     */
    bool setTile(std::string position, TilePtr tile);

    // Gets all the words created by the letters put on the board that turn
    std::vector<std::string> getAllWords();

    /*
     * Checks that the letters placed on the board this
     * turn meet scrabble rules
     */
    std::vector<TilePtr> checkLetterPosValidity();

    // Ends the turn clears the list of letters added this turn
    void clearCoords();

    // Getter method used by save/load
    unsigned int getWidth();

    // Getter method used by save/load
    unsigned int getLength();

    /*
     * Getter method used by save/load
     */
    std::vector<std::vector<TilePtr>> getBoard();

    // Getter method used by save/load
    TilePtr getTile(size_t x, size_t y);

    // gets if the board is empty
    bool isEmpty();

    // sets the board to be not empty.
    void setNotEmpty();

    // sets the boards empty status
    void setEmptyStatus(bool status);

    // Return the number of tiles a player herself has placed in one round
    size_t getNumPlacedTiles();

    /*
     * Check if a tile is within the board
     * pre-condition: the values are within the range of the board width,height
     */
    bool checkIfInBoard(unsigned int x, unsigned int y);

    // adjacency checking function checks that tile meets adjacency rules
    bool checkAdjacentTiles();

    /*
     * Adds a single word, starting from a single coord,
     * in a specified direction
     * pre-condition: the starting coordinations are valid board coordinates
     */
    std::string addWordInDirection(std::vector<int>
                                   &startingLetterCoords, Direction dir);

    /*
     * Converts a specified string to a coordinate
     * pre-condition: The string be in the form "letterintegerinteger"
     * in order to work. For example: "A13"
     */
    std::vector<int> convertPosToInt(const std::string &position);

    /*
     * gets the direction of a word, returns none direction
     * if they are not on same row or column
     * pre-condition: the location vectors are valid board coordinates
     */
    Direction getWordDirection(std::vector<int> &location1,
                               std::vector<int> &location2);

// Board Attributes:
private:
    std::vector <std::vector<TilePtr>> board;
    unsigned int width;
    unsigned int height;
    bool misEmpty;
    std::vector<std::vector<int>> currentWordCoords;
};
typedef std::shared_ptr<Board> BoardPtr;

#endif // ASSIGNMENT2_BOARD_H