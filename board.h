#ifndef ASSIGNMENT2_BOARD_H
#define ASSIGNMENT2_BOARD_H
#include <vector>
#include <sstream>
#include <random>
#include <algorithm>
#include <memory>
#include <tuple>
#include <string>
#include <queue>


#include "types.h"
#include "tile.h"
#include "player.h"
#include "Dictionary.h"

// The min-heap syntax
typedef std::priority_queue<int, std::vector<int>, std::greater<>> placedIndicesType;
typedef std::shared_ptr<placedIndicesType> placedIndicesPtr;

class Board {


public:
    /*
     * Create a board with a specified width, height and if it is empty
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

    // Ends the turn clears the list of letters added this turn
    void clearCoords();
    // Getter method used by save/load
    unsigned int getWidth();
    // Getter method used by save/load
    unsigned int getLength();
    /*
     * Getter method used by save/load
     */
    std::vector<std::vector<TilePtr> > getBoard();
    // Getter method used by save/load
    TilePtr getTile(size_t x, size_t y);
    // gets if the board is empty
    bool isEmpty();
    // sets the board to be not empty.
    void setNotEmpty();
    // sets the boards empty status
    void setEmptyStatus(bool status);
    /*
     * Check if a tile is within the board
     * pre-condition: the values are within the range of the board width,height
     */
    bool checkIfInBoard(unsigned int x, unsigned int y);
    // adjacency checking function checks that tile meets adjacency rules
    bool checkAdjacentTiles();
    // Checks whether all placed tiles are on the same line
    bool checkAllOnTheSameLine();
    // Checks whether the first placement was made through the center cell
    bool checkTheFirstPlacement();
    /*
     * Adds a single word, starting from a single coord,
     * in a specified direction
     * pre-condition: the starting coordinations are valid board coordinates
     */
    std::string addWordInDirection(std::vector<int>
                                   &startingLetterCoords, Angle dir);
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
    Angle getWordDirection(std::vector<int> &location1,
                           std::vector<int> &location2);

    // Get all the wordsInQueue created by the letters put on the board in the current turn &
    // store them in the data member currWords if all are valid.
    void makeCurrWords();

    // Return the vector currWords that contains all the valid wordsInQueue
    std::vector<std::string>& getCurrWords();

    // Check whether the placement is legal according to the game rules
    bool isPlacementValid();

    /*                          Milestone 1 & 2 Above
     * -------------------------------------------------------------------
     * -------------------------------------------------------------------
     * -------------------------------------------------------------------
     * -------------------------------------------------------------------
     *                          Milestone 3 & 4 Below
     */

    // Check whether the placed tiles make up wordsInQueue found in a dict stored in wordBuilder
    bool isWordValid(DictionaryPtr dict);
    std::vector<TilePtr>& getTilesToReturn();
    // Clear the board of invalid tiles and put them in tilesToReturn
    void setTilesToReturn();
    placedIndicesPtr getPlacedIndices();
    Angle getPlacedDir();
    // Reduce the dimensionality
    void trackPlacedTiles();
    Value getValue(int idx);
    std::string getLetters(int idx);
    Letter getLetter(int idx);
    bool hasPlacedTile(int idx);

private:
    std::vector<std::vector<TilePtr> > board;
    unsigned int width;
    unsigned int height;
    bool misEmpty;
    std::vector<std::vector<int> > gridLocs;

    /*                          Milestone 1 & 2 Above
     * -------------------------------------------------------------------
     * -------------------------------------------------------------------
     * -------------------------------------------------------------------
     * -------------------------------------------------------------------
     *                          Milestone 3 & 4 Below
     */

    // If tiles placed are deemed invalid, they are put back in tilesToReturn.
    std::vector<TilePtr> tilesToReturn;
    std::vector<std::string> currWords;
    Angle placedDir;
    // placedIndices is cleared at the beginning of the turn and filled with the
    // placedIndices converted from gridLocs belonging to the tiles placed
    placedIndicesPtr placedIndices;
    bool placedBoard[BOARD_LENGTH * BOARD_LENGTH];


};
typedef std::shared_ptr<Board> BoardPtr;

#endif // ASSIGNMENT2_BOARD_H