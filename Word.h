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
    std::vector<std::string> words;
    size_t score;
    std::map<int, char> tileIndices;

    Word(std::vector<std::string>& words, std::map<int, char> tileIndices);
};
typedef std::shared_ptr<Word> WordPtr;

struct CompareWord {
public:
    bool operator()(const WordPtr& w1, const WordPtr& w2) const {
        int w1Score = 0;
        int w2Score = 0;
        for (auto& word: w1->words) {
            for (auto ch: word)
                w1Score += letterScoreMap.at(ch);
        }
        w1->score = w1Score;
        for (auto& word: w2->words) {
            for (auto ch: word)
                w2Score += letterScoreMap.at(ch);
        }
        w2->score = w2Score;
        return w1Score < w2Score;
    }
};

//typedef std::priority_queue




#endif //ASSIGNMENT3_WORD_H
