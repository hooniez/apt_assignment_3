#include "board.h"
#include <fstream>

Board::Board(unsigned int width, unsigned int height, bool misEmpty)
    : misEmpty(misEmpty)
{
    this->width = width;
    this->height = height;
    /*
     * Create a vector for each row, then for each column push a null ptr.
     * Then add that row to the board
     */
    for (unsigned int i = 0; i < height; ++i)
    {
        std::vector<TilePtr> row;
        for (unsigned int k = 0; k < width; ++k)
        {
            row.push_back(nullptr);
        }
        this->board.push_back(row);
    }
}

// Checks if the gridLoc is valid and within the bounds to place a tile
bool Board::checkIfValid(const std::string &gridLoc)
{
    bool isValid = true;
    Letter row = gridLoc[0];

    // Check if the row is in between A and O
    if (row < 'A' || row > 'O')
    {
        isValid = false;
    }
    else
    {
        // Check if the characters that make up the column are numeric
        std::string col = gridLoc.substr(1, 2);
        for (const auto &ch : col)
        {
            if (!isdigit(ch))
                isValid = false;
        }
        if (isValid)
        {
            // Check if the column value is within the range
            if (std::stoi(col) < 0 || std::stoi(col) > 14)
            {
                isValid = false;
            }
        }
    }

    return isValid;
}

// check if the specified location is within board bounds.
bool Board::checkIfInBoard(unsigned int row, unsigned int col)
{
    bool returnValue = true;
    if (row < 0)
    {
        returnValue = false;
    }
    else if (row >= this->height)
    {
        returnValue = false;
    }
    else if (col < 0)
    {
        returnValue = false;
    }
    else if (col >= this->width)
    {
        returnValue = false;
    }
    return returnValue;
}

// Checks if the board cell is empty to contain a tile
bool Board::checkIfEmpty(const std::string &gridLoc)
{
    bool isValid = true;
    auto pos = convertPosToInt(gridLoc);
    int x = pos[0];
    int y = pos[1];
    /*
     * Check if the location is occupied by another tile
     * If the cell is occupied by a tile, set isValid to false;
     */
    if (board[x][y] != nullptr)
    {
        isValid = false;
    }
    return isValid;
}

bool Board::checkAdjacentTiles()
{
    bool insert = false;

    if (this->misEmpty)
    {
        /* Make sure when the player places tiles for the first time,
         * one of the tiles is placed at the centre of the board
         */
        int midRow = BOARD_LENGTH / 2;
        int midCol = BOARD_LENGTH / 2;
        for (long unsigned int k = 0; k < currentWordCoords.size(); ++k)
        {
            int row = currentWordCoords[k][0];
            int col = currentWordCoords[k][1];
            if (row == midRow && col == midCol)
            {
                insert = true;
            }
        }
    }
    else
    {
        /*
         * Logic does the following.
         * moves through all the currently stored letters:
         * for each of the 2 wird directions
         * HORIZONTAL, and VERTICAL
         * 1. checks that a movement in each of the 4 directions
         * if their is a tile in that direction then increment the adjacent
         * tile counter
         * 2. check that the adjacent tile we found is not one of the added
         * letters.
         * if it is increment the disallowed counter
         * check to see if the adjacent tile total is equal to the adjacent
         * tile total, if this is the case then set return to false.
         * otherwise return true as tests passed
         */
        int adjacentCount = 0;
        int disallowedCount = 0;
        std::vector<std::tuple<int, int>> directions =
            {{0, -1}, {1, 0}, {0, 1}, {-1, 0}};

        for (long unsigned int k = 0; k < this->currentWordCoords.size(); ++k)
        {
            int row = this->currentWordCoords[k][0];
            int col = this->currentWordCoords[k][1];
            // for all the directions
            for (long unsigned int j = 0; j < TOTALDIRECTIONS; ++j)
            {

                int rowLooking = row + std::get<0>(directions[j]);
                int colLooking = col + std::get<1>(directions[j]);
                // find adjacent tiles
                if (checkIfInBoard(rowLooking, colLooking))
                {
                    if ((this->board[rowLooking][colLooking] != nullptr))
                    {
                        adjacentCount += 1;
                        for (long unsigned int i = 0; i <
                                                      this->currentWordCoords.size();
                             ++i)
                        {
                            if ((this->currentWordCoords[i][0] == rowLooking) &&
                                (this->currentWordCoords[i][1] == colLooking))
                            {
                                disallowedCount += 1;
                            }
                        }
                    }
                }
            }
        }
        if (adjacentCount == disallowedCount)
        {
            insert = false;
        }
        else
        {
            insert = true;
        }
    }
    return insert;
}

/*
 * Give it a list of all the letters the player just added,
 * and the length of this letter list,
 * will return all the words that can be created
 */
std::vector<std::string> Board::getAllWords()
{
    /*
     * Work out the direction that the wordBuilder is going.
     * Make sure not to check words in this direction
     * Need to check if its a one letter wordBuilder first.
     */

    unsigned int length = this->currentWordCoords.size();
    Direction dir = NONE;
    std::string wordString;
    std::vector<std::string> wordsArray;
    /* a single letter, convert it to an int array
     * then check both directions and finish
     */
    if (length == 1)
    {
        /*
         * first add all the words in a right direction,
         * then add all the words in a left direction
         */
        wordString = addWordInDirection(this->currentWordCoords[0],
                                        HORIZONTAL);
        if (wordString.length() > 1)
        {
            wordsArray.push_back(wordString);
        }
        wordString = addWordInDirection(this->currentWordCoords[0],
                                        VERTICAL);
        if (wordString.length() > 1)
        {
            wordsArray.push_back(wordString);
        }
        if (wordsArray.size() == 0)
        {
            wordsArray.push_back(wordString);
        }
    }
    else
    {
        // first two elements to get the direction
        dir = getWordDirection(this->currentWordCoords[0],
                               this->currentWordCoords[1]);

        // add all the letters from null to null;
        wordString = addWordInDirection(this->currentWordCoords[0], dir);
        wordsArray.push_back(wordString);

        /*
         * next step move through all the letters of the original
         * wordBuilder and do the same in both directions
         */

        for (unsigned int i = 0; i < length; i++)
        {
            if (dir == HORIZONTAL)
            {
                // if look at words running parallel so check the opposite
                wordString = addWordInDirection(this->currentWordCoords[i],
                                                VERTICAL);
                /*
                 * check if the wordBuilder is of length 1,
                 * if it is its just the base letter so dont add it
                 */
            }
            else
            {
                // if look at words running parallel so check the opposite
                wordString = addWordInDirection(this->currentWordCoords[i],
                                                HORIZONTAL);
            }
            //
            if (wordString.length() > 1)
            {
                wordsArray.push_back(wordString);
            }
        }
    }
    this->clearCoords();

    return wordsArray;
}

std::string Board::addWordInDirection(std::vector<int> &startingLetterCoords,
                                      Direction dir)
{
    // move as far as possible left, then move right as far as possible
    std::string wordString;

    /*
     * if the first call is false,
     * then we don't want to count words of length 1
     */

    // move left
    if (dir == HORIZONTAL)
    {
        int colIndex = startingLetterCoords[1];
        // while(checkIfInBoard(colIndex,startingLetterCoords[0]) &&
        while (checkIfInBoard(startingLetterCoords[0], colIndex) &&
               this->board[startingLetterCoords[0]][colIndex] != nullptr)
        {
            colIndex--;
        }
        // we went one too far so add one back again.
        colIndex++;
        // moving right, time to add the letters of wordBuilder to wordString
        while (checkIfInBoard(startingLetterCoords[0], colIndex) &&
               this->board[startingLetterCoords[0]][colIndex] != nullptr)
        {
            wordString.push_back(board[startingLetterCoords[0]]
                                      [colIndex]
                                          ->getLetter());
            colIndex++;
        }
    }
    else
    {
        int rowIndex = startingLetterCoords[0];
        while (checkIfInBoard(rowIndex, startingLetterCoords[1]) &&
               this->board[rowIndex][startingLetterCoords[1]])
        {
            rowIndex--;
        }
        // we went one too far so add one back again.
        rowIndex++;

        // moving right, time to add the letters of wordBuilder to wordString
        while (checkIfInBoard(rowIndex, startingLetterCoords[1]) &&
               this->board[rowIndex][startingLetterCoords[1]])
        {
            wordString.push_back(board[rowIndex]
                                      [startingLetterCoords[1]]
                                          ->getLetter());
            rowIndex++;
        }
    }
    // add the main wordBuilder.
    return wordString;
}

Direction Board::getWordDirection(std::vector<int> &location1,
                                  std::vector<int> &location2)
{
    Direction dir = NONE;
    if (location1[0] == location2[0])
    {
        // Same row;
        dir = HORIZONTAL;
    }
    else if (location1[1] == location2[1])
    {
        dir = VERTICAL;
    }
    else
    {
        // Something went wrong
    }
    return dir;
}

std::vector<TilePtr> Board::checkLetterPosValidity()
{

    // first check to determin that the tiles are adjacent to others.
    bool lettersValid = checkAdjacentTiles();

    /*
     * Create a loop that goes through all the letters entered and
     * check that at least one is next to a letter that is not part
     * of the letters entered.
     */

    unsigned int count = 0;
    if (!lettersValid)
    {
        // failed the adjacent test so dont need next test
    }
    else
    {
        /*
         * more than one letter. So we need to make
         * sure they are all on the same line.
         */
        if (this->currentWordCoords.size() == 1)
        {
            // do nothing
        }
        else
        {
            Direction dir = this->getWordDirection(this->currentWordCoords[0],
                                                   this->currentWordCoords[1]);
            /* The word has a up down direction,
             * check that all letters are on the updown
             */
            std::vector<int> firstCoord = this->currentWordCoords[0];
            if (dir == HORIZONTAL)
            {
                /*
                 * Check 2 make sure that we can get from one end to the other
                 * without a space find the smallest value in the row
                 */

                for (unsigned int k = 0;
                     k < this->currentWordCoords.size(); ++k)
                {
                    // saving the smallest element
                    if (firstCoord[1] > this->currentWordCoords[k][1])
                    {
                        firstCoord = this->currentWordCoords[k];
                    }
                }
                /*
                 * step through each item in the row starting from this one,
                 * and tick off each of the values
                 */
                int row = this->currentWordCoords[0][0];
                int col = firstCoord[1];
                while ((this->checkIfInBoard(row, col)) &&
                       (this->board[row][col] != nullptr) &&
                       count < this->currentWordCoords.size())
                {
                    for (long unsigned int j = 0;
                         j < this->currentWordCoords.size(); ++j)
                    {
                        if ((row == this->currentWordCoords[j][0]) &&
                            (col == this->currentWordCoords[j][1]))
                        {
                            count++;
                        }
                    }
                    col++;
                }
                /* The word has a up down direction, check that all letters are on
                 * the updown
                 */
            }
            else if (dir == VERTICAL)
            {
                for (unsigned int k = 0;
                     k < this->currentWordCoords.size(); ++k)
                {
                    // saving the smallest element
                    if (firstCoord[0] > this->currentWordCoords[k][0])
                    {
                        firstCoord = this->currentWordCoords[k];
                    }
                }
                /* step through each item in the row starting from this one,
                 * and tick off each of the values
                 */
                int col = this->currentWordCoords[0][1];
                int row = firstCoord[0];
                while (checkIfInBoard(row, col) &&
                       (this->board[row][col] != nullptr) &&
                       (count < this->currentWordCoords.size()))
                {
                    for (long unsigned int j = 0;
                         j < this->currentWordCoords.size(); ++j)
                    {
                        if ((row == this->currentWordCoords[j][0]) &&
                            (col == this->currentWordCoords[j][1]))
                        {
                            count++;
                        }
                    }
                    row++;
                }
                /*
                 * If it was neither HORIZONTAL nor VERTICAL then letters
                 * not alligned so letters are invalud
                 */
            }
            else
            {
                lettersValid = false;
            }
            /*
             * We exited because of a space.
             * So that means our letters are invalid
             */
            if (count != this->currentWordCoords.size())
            {
                lettersValid = false;
            }
        }
    }

    /*
     * in the board empty case need to pass a final check,
     * that the letters go through the middle.
     */
    if (this->misEmpty)
    {
        bool intersectsMid = false;
        /*
         * go through each added letter, check that one of them
         * is in the center
         */
        for (long unsigned int i = 0;
             i < this->currentWordCoords.size(); ++i)
        {
            if ((currentWordCoords[i][0] == int(this->height / 2)) &&
                (currentWordCoords[i][1] == int(this->width / 2)))
            {
                intersectsMid = true;
            }
        }
        // if meets the conditions, then proceed
        if (intersectsMid == true)
        {
            // do nothing
        }
        else
        {
            // otherwise he letters are invalid
            lettersValid = false;
        }
    }
    std::vector<TilePtr> returnVector;
    // Return a nullptr in the returnVector if the letters are valid
    if (lettersValid)
    {
        returnVector.push_back(nullptr);
        // set the board as not empty, as letters added successfully
        this->misEmpty = false;
    }
    else
    {
        /*
         * Otherwise add all the just added tiles
         * into the returnVector and
         * clear the board of these tiles.
         */
        for (long unsigned int j = 0;
             j < this->currentWordCoords.size(); ++j)
        {
            returnVector.push_back(
                this->board[this->currentWordCoords[j][0]]
                           [this->currentWordCoords[j][1]]);
            this->board[this->currentWordCoords[j][0]]
                       [this->currentWordCoords[j][1]] = nullptr;
        }
        // clear the array holding the just added letters
        currentWordCoords.clear();
    }
    return returnVector;
}

/*
 * pre-condition: the
 */
std::vector<int> Board::convertPosToInt(const std::string &position)
{
    char rowChar = position[0];
    int aInt = int('A');
    int rowInt = int(rowChar);
    int row = rowInt % aInt;

    std::string colStr = position.substr(1, 2);
    std::stringstream sstream(colStr);
    int col;
    sstream >> col;
    std::vector<int> returnArray{row, col};
    return returnArray;
}

/*
 * This is the main function used to add tiles to the board.
 * position takes the position as a string I.E. "C4"
 * tilePtr is the tile that you want to add to the board.
 */
bool Board::setTile(std::string position, TilePtr tile)
{
    bool returnVal = true;

    // Coverts string input to the required row,col index
    std::vector<int> lettersAsInt = this->convertPosToInt(position);
    /*
     * first check if the space is avaliable.
     */
    if (this->board[lettersAsInt[0]][lettersAsInt[1]] == nullptr)
    {
        /*
         * if its avaliable then check to see if its next to an existing
         * tile.
         */
        this->board[lettersAsInt[0]][lettersAsInt[1]] = tile;
        this->currentWordCoords.push_back(lettersAsInt);
    }
    else
    {
        std::cout
            << "Board: Received a already used tile space"
            << " as new tile location."
            << std::endl;
        returnVal = false;
    }

    return returnVal;
}

void Board::clearCoords()
{
    for (long unsigned int i = 0; i < this->currentWordCoords.size(); ++i)
    {
        this->currentWordCoords[i].clear();
    }
    this->currentWordCoords.clear();
}

Board::~Board()
{
    for (unsigned int i = 0; i < this->height; i++)
    {
        this->board[i].clear();
    }
    this->board.clear();
}

std::vector<std::vector<TilePtr>> Board::getBoard()
{
    return board;
}

/*
 * Returns a pointer to a tile in the cell if it exists.
 * If either x or y is outside the bounds of the board OR
 * If no tile exists in the given cell, nullptr is returned.
 */
TilePtr Board::getTile(size_t x, size_t y)
{
    TilePtr currCell = nullptr;
    // If x or y is outside the boundaries of the board
    if (x >= 0 && x < BOARD_LENGTH && y >= 0 && y < BOARD_LENGTH)
        currCell = board[x][y];
    return currCell;
}

// returns true if the board is flagged as empty.
bool Board::isEmpty()
{
    return misEmpty;
}

void Board::setNotEmpty()
{
    misEmpty = false;
}

void Board::setEmptyStatus(bool status)
{
    this->misEmpty = status;
}

unsigned int Board::getWidth()
{
    return height;
}

unsigned int Board::getLength()
{
    return width;
}

/*
 * Return the number of tiles a player herself has placed in one round
 * This is only invoked when the placed tiles are deemed valid.
 */
size_t Board::getNumPlacedTiles()
{
    return currentWordCoords.size();
}