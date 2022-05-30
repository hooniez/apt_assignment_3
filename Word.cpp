//
// Created by Myeonghoon Sun on 26/5/2022.
//

#include "Word.h"
Word::Word(std::string word,
           std::string lettersInHand,
           std::map<int, char> tileIndices):
           wordBeingBuilt(word),
           lettersInHand(lettersInHand),
           score(0),
           tileIndices(tileIndices) {

}

Word::Word(const Word& other): wordBeingBuilt(other.wordBeingBuilt),
                               builtWords(other.builtWords),
                               lettersInHand(other.lettersInHand),
                               score(other.score),
                               tileIndices(other.tileIndices) {

}

void Word::erase(char ch) {
    auto it = std::find(
            lettersInHand.begin(),lettersInHand.end(), ch);
    lettersInHand.erase(it);
}
