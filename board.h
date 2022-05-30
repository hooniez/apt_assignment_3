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
typedef std::priority_queue<int, std::vector<int>,
        std::greater<>> placedIndicesType;
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
    bool setTile(std::tuple<int, int> coords, TilePtr tile);

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
     * Adds a single wordBeingBuilt, starting from a single coord,
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
     * gets the direction of a wordBeingBuilt, returns none direction
     * if they are not on same row or column
     * pre-condition: the location vectors are valid board coordinates
     */
    Angle getWordDirection(std::vector<int> &location1,
                           std::vector<int> &location2);

    // Get all the words created by the letters put on the board
    // in the current turn & store them in the data member currWords if all
    // are valid.
    void makeCurrWords();

    // Return the vector currWords that contains all the valid completeWords
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

    // Check whether the placed tiles make a valid word found in a dictionary
    bool isWordValid(const DictionaryPtr& dict);

    /* This method was created as the previous implementation lacked the easy
     * access of the tiles to be returned.
     */
    std::vector<TilePtr>& getTilesToReturn();
    // Clear the board of invalid tiles and put them in tilesToReturn
    void setTilesToReturn();

    /*
     * Guided by the good people on stackoverflow
     * (https://stackoverflow.com/questions/17259877/1d-or-2d-array-whats-faster),
     * The 2 dimensional representation of the board is reduced to one dimension
     * as the speed of the AI algorithm is assessed.
     */
    void trackPlacedTiles();

    /*
     * Getters and setters for working with the 1 dimensional array
     * to be used in WordBuilder
     */
    std::string getLetters(int idx);
    bool hasPlacedTile(int idx);
    Value getValue(int idx);
    Letter getLetter(int idx);

    /*
     * Get the indices of the tiles placed in the previous turn
     * to be processed by WordBuilder
     */
    placedIndicesPtr getPlacedIndices();
    // Get the angle at which tiles were placed in the previous turn.
    Angle getPlacedDir();
    // The method used to update placedIndices after loading a saved file
    void setPlacedIndices();

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
    /*
     * currWords as a data member was absent in the previous milestone,
     * which stores currWords built in the previous turn
     */
    std::vector<std::string> currWords;
    // placedDir denotes whether words are placed horizontally or vertically
    Angle placedDir;
    /*
     * placedIndices stores 1 dimensional indices at which tiles are placed
     * in the previous turn so that AdjacentTiles can be created or updated
     * accordingly.
     */
    placedIndicesPtr placedIndices;
    /*
     * the placedBoard array is used to work with the 1 dimensional array in
     * WordBuilder and checks whether a letter has been placed at a given index.
     */
    bool placedBoard[BOARD_LENGTH * BOARD_LENGTH];



};
typedef std::shared_ptr<Board> BoardPtr;

#endif // ASSIGNMENT2_BOARD_H