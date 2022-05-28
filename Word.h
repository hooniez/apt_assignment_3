//
// Created by Myeonghoon Sun on 26/5/2022.
//

#ifndef ASSIGNMENT3_WORD_H
#define ASSIGNMENT3_WORD_H

#include <string>
#include <queue>
#include <map>

#include "types.h"

class Word {
public:
    std::string wordBeingBuilt;
    // Store the word made across the perpendicular direction to the direction in which the word is being built
    std::string builtWord;
    std::string lettersInHand;
    size_t score;
    std::map<int, char> tileIndices;

    Word(std::string word, std::string letttersInHand, std::map<int, char> tileIndices);

    Word(const Word& word);

    void erase(char ch);
    void prepend(char ch, int idx);
    void append(char ch, int idx);

};
typedef std::shared_ptr<Word> WordPtr;

struct CompareWord {
public:
    bool operator()(WordPtr &w1, WordPtr& w2) const {
        int w1Score = 0;
        int w2Score = 0;
        for (auto ch: w1->wordBeingBuilt)
            w1Score += letterScoreMap.at(ch);
        if (w1->lettersInHand.empty()) {
            w1Score += BINGO_ADDITIONAL_SCORE;
        }
        w1->score = w1Score;

        for (auto ch: w2->wordBeingBuilt)
            w2Score += letterScoreMap.at(ch);
        if (w2->lettersInHand.empty()) {
            w2Score += BINGO_ADDITIONAL_SCORE;
        }

        w2->score = w2Score;


        return w1Score < w2Score;
    }
};

//typedef std::priority_queue




#endif //ASSIGNMENT3_WORD_H
