//
// Created by Myeonghoon Sun on 22/5/2022.
//

#ifndef ASSIGNMENT3_GREEDYMAPBUILDER_H
#define ASSIGNMENT3_GREEDYMAPBUILDER_H

#include "MapBuilder.h"
typedef std::map<std::string, std::set<char>> greedyMapBuilder;

/*
 * GreedyMapBuilder was used to create the text files called backwardMap and
 * forwardMap so that the pre-calculated information in them can be loaded
 * into maps and membership checks can be performed in constant time at
 * runtime. Even though the produced maps is about 50MB in size, an extra 10
 * seconds to load it is well amortized by the stunningly fast speed at which
 * the best scoring word is found; if the size is still concerning, it may be
 * reduced by the Huffman encoding algorithm.
 *
 * Even though the use of an ordered_map is not ideal when dealing with 2
 * million elements, the produced files will be in a more legible format. As
 * the build process is invoked only once to produce the texts, which then
 * can be read into unordered_maps when a program runs as usual,
 * the extra time to build it is bearable so that the resultant files are
 * easy to read in case of a bug.
 *
 * Each word in the provided dictionary is iterated in one character increments
 * For example, if the word "GOOD" is processed, forwardMap generates
 *
 *  "G":'O'
 *  "O":'O'
 *  "O":'D'
 *  "D":'|'
 *  "GO":'O'
 *  "OO":'D'
 *  "OD":'|'
 *  "GOO":'D'
 *  "OOD":'|'
 *  "GOOD":'|'
 *
 *  Each word is iterated in the same manner to generate backwardMap. If the
 *  word "GOOD" is used again for an example:
 *
 * "D":'O'
 * "O":'O'
 * "O":'|'
 * "G":'|'
 * "OD":'O'
 * "OO":'G'
 * "GO":'|'
 * "OOD":'G'
 * "GOO":'|'
 * "GOOD":'|'
 *
 * where the sub key '|' denotes the main key has no more letter following.
 *
 * After all the words are converted to such records as above,
 * Looking up "GOOD" in forwardMap will return
 *
 * GOOD E H I L M N S W Y |
 *
 * backwardMap gives:
 *
 * GOOD A L N R S Y |
 *
 * If the algorithm started with a tile already placed with the letter 'G'
 * and the letters in the hand are 'O', 'O', 'D', 'U' 'N' 'L' and 'Y', it
 * can check whether G can be extended backwards and forwards using each of
 * the letters. When a word is built up to "GOOD", the algorithm
 * keeps checking backwards and forwards to finally arrive at "UNGOODLY" and
 * hits a bingo.
 *
 * The use of graph has been considered, but there is to be no gain in
 * wrapping the same data in the graph as recursion can be used to go
 * backwards and forwards from a given word.
 * forward and backward search are chosen.
 *
 */

class GreedyMapBuilder: public MapBuilder {
public:
    GreedyMapBuilder(bool forward,
                     std::string  inFile,
                     std::string  outFile);
    void build();
    void convertToMap() override;
    void saveMap() override;

    greedyMapBuilder greedyMap;
    bool forward;
    std::string inFile;
    std::string outFile;
};


#endif //ASSIGNMENT3_GREEDYMAPBUILDER_H
