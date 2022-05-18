//
// Created by Myeonghoon Sun on 11/4/2022.
//

#ifndef ASSIGN2_PLAYER_H
#define ASSIGN2_PLAYER_H

#include "linkedlist.h"
#include "types.h"
#include <iostream>

class CommandHandler;

/*
 * The player class consist of player name, score and hand which is a linked
 * list.
 */
class Player
{

public:
    Player() = default;
    Player(std::string);
    Player(std::string name, int score, LinkedListPtr<TilePtr> hand);
    // Player(std::string, LinkedListPtr<TilePtr>);
    ~Player();
    std::string &getName();

    // add one tile to player's hand
    void drawOne(TilePtr);

    /*
     * remove and return the first tile with the given letter form player's
     * hand return nullptr if no such tile
     */
    TilePtr discardTile(Letter discardedLetter);

    // check if a tile with the given letter is in player's hand
    bool checkLetterInHand(Letter letter);

    LinkedListPtr<TilePtr> getHand();

    // add the given score to the player
    void addScore(int);

    // get the current score of the player
    int getScore();

    bool hasPassedTwie();
    void incrementNumPasses();
    void resetNumPasses();

private:
    std::string name;
    int score;
    LinkedListPtr<TilePtr> hand;
    size_t numPasses;

    /*
     * helper function return the index of the first tile with the given
     * letter in player's hand return -1 if no such tile
     */
    int findLetterInHand(Letter letter);
};
typedef std::shared_ptr<Player> PlayerPtr;
#endif // ASSIGN2_PLAYER_H