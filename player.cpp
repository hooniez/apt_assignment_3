#include "player.h"
#include "commandHandler.h"

/*
 * Most of the methods here are self-explanatory, so no comment needed.
 */

Player::Player(std::string name)
    : name(name),
      score(0),
      hand(std::make_shared<LinkedList<TilePtr>>()),
      numPasses(0)
{
}

Player::Player(std::string name, int score,
               LinkedListPtr<TilePtr> hand)
    : name(name),
      score(score),
      hand(hand),
      numPasses(0)
{
}

Player::~Player()
{
    hand = nullptr;
}

std::string &Player::getName()
{
    return name;
}

void Player::drawOne(TilePtr newTile)
{
    hand->append(newTile);
}

TilePtr Player::discardTile(Letter discardedLetter)
{
    // get the index of the tile letter to be move
    TilePtr outTile = nullptr;
    int index = findLetterInHand(discardedLetter);
    if (index != -1)
    {
        // remove the tile from hand
        outTile = hand->popAt(index);
    }
    return outTile;
}

int Player::findLetterInHand(Letter letter)
{
    bool tileFound = false;
    int index = -1;
    // iterate hand until the letter is found
    for (int i = 0; !tileFound && i < hand->getLength(); ++i)
    {
        TilePtr currentTile = hand->peekAt(i);
        if (currentTile->getLetter() == letter)
        {
            index = i;
            tileFound = true;
        }
    }
    return index;
}

bool Player::checkLetterInHand(Letter letter)
{
    return findLetterInHand(letter) == -1;
}

LinkedListPtr<TilePtr> Player::getHand()
{
    return hand;
}

void Player::addScore(int addedScore)
{
    score += addedScore;
}

int Player::getScore()
{
    return score;
}

bool Player::hasPassedTwie()
{
    return numPasses == 2;
}

void Player::incrementNumPasses()
{
    ++numPasses;
}
void Player::resetNumPasses()
{
    numPasses = 0;
}