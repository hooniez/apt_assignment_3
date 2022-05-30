//
// Created by Myeonghoon Sun on 26/5/2022.
//

#ifndef ASSIGNMENT3_WORD_H
#define ASSIGNMENT3_WORD_H

#include <string>
#include <queue>
#include <map>

#include "types.h"

/*
 * Word objects can be created when a placed tile's letter can be
 * extended by one of the letters in the hand (by looking up greedy maps).
 * However, say the angle at which a word is being built is horizontal, if the
 * vertically placed pre-existing letters cannot be turned into a complete word
 * by placing the letter in the hand placed in between, a Word object cannot
 * be created even though the placed tile of reference is extendable by itself.
 * If there are no letters in the perpendicular direction, only the
 * extendability of the placed tile or the wordBeingBuilt of another
 * Word object needs to be checked before a new Word object can be created.
 * The same logic applies when extending a letter in the vertical direction.
 *
 * When an instance is created, the index of the single dimensional board at
 * which to place one of the letters in the hand is recorded in sorted_map
 * named tileIndices as a key and the letter itself as its value. In
 * addition, the tiles in the hand minus the one to extend the existing
 * placed tile are stored together in lettersInHand as a string to further
 * extract from when building a subsequent word. The logic for assigning the
 * data member score can be found below above the comparator used with a
 * priority queue, which stores all the complete instances (beingBuiltWord
 * found in the dictionary) of Word generated each turn.
 */

class Word {
public:
    Word(std::string word,
         std::string letttersInHand,
         std::map<int, char> tileIndices);

    Word(const Word& word);

    std::string wordBeingBuilt;
    std::vector<std::string> builtWords;
    std::string lettersInHand;
    size_t score;
    std::map<int, char> tileIndices;
    void erase(char ch);
};
typedef std::shared_ptr<Word> WordPtr;

/*
 * The comparator below is the metric by which a priority queue will insert
 * complete Word objects. Only the ones found in a dictionary can make it in
 * the queue.
 *
 * all the values of the letters collected are summed and if the Word's
 * letterInHand is empty(), an additional 50 points are added. The max-heap
 * structure then lifts and shifts down accordingly.
 */
struct CompareWord {
public:
    bool operator()(WordPtr &w1, WordPtr& w2) const {
        int w1Score = 0;
        int w2Score = 0;
        for (auto ch: w1->wordBeingBuilt)
            w1Score += letterScoreMap.at(ch);
        for (auto &word: w1->builtWords)
            for (auto ch: word)
            w1Score += letterScoreMap.at(ch);
        if (w1->lettersInHand.empty()) {
            w1Score += BINGO_ADDITIONAL_SCORE;
        }
        w1->score = w1Score;

        for (auto ch: w2->wordBeingBuilt)
            w2Score += letterScoreMap.at(ch);
        for (auto &word: w2->builtWords)
            for (auto ch: word)
                w2Score += letterScoreMap.at(ch);
        if (w2->lettersInHand.empty()) {
            w2Score += BINGO_ADDITIONAL_SCORE;
        }
        w2->score = w2Score;

        return w1Score < w2Score;
    }
};

typedef std::priority_queue<WordPtr,
std::vector<WordPtr>, CompareWord> CompleteWords;
typedef std::shared_ptr<CompleteWords> CompleteWordsPtr;




#endif //ASSIGNMENT3_WORD_H
