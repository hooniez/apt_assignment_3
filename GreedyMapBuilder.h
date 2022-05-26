//
// Created by Myeonghoon Sun on 22/5/2022.
//

#ifndef ASSIGNMENT3_GREEDYMAPBUILDER_H
#define ASSIGNMENT3_GREEDYMAPBUILDER_H

/*
 * // TODO FIX the writing
 * GreedyMapBuilder was used to create the text files called aiForwardMap and aiBackwardMap so that
 * the pre-calculated information in them can be loaded into maps quickly.
 *
 * Even though the use of an ordered_map is not ideal when dealing with 2 million elements,
 * the produced files will be in a more legible format. As the build process is only invoked once
 * to produce the texts, which then can be read into unordered_maps when a program runs as usual,
 * the initial time cost to build is well amortised in the name of storing files that
 * are easy to read in case of a bug.
 *
 * Step 1)
 * Each word in the provided dictionary is iterated in one character increments
 * and the corresponding frequencies are recorded.
 * For example, if the word "GOOD" is processed, forwardGreedyMap generates
 * {
 *  "G":{'O':1}, "O":{'O':1}, "O":{'D':1}, "D":{'|':1},
 *  "GO":{'O':1}, "OO":{'d':1}, "OD":{'|':1}
 *  "GOO":{'D':1}, "OOD":{'|':1},
 *  "GOOD":{'|':1}
 * }
 * where the sub key '|' denotes the main key has no more letter following.
 *
 * After all the wordsInQueue are converted to such records as above,
 * Looking up "GOOD" will initially return
 * {'|': 10, 'S': 3, 'E': 4, 'H': 3, 'I': 3,
 *  'L': 6, 'M': 2, 'N': 2, 'W': 4, 'Y': 7}.
 *
 * Step 2)
 * To minimise the amount of computing as much as possible, each of the values above
 * is first converted to its probability. Thus Looking up "GOOD" after the conversion
 * will return
 * {'|': 0.22727, 'S': 0.06818, 'E': 0.09091, 'H': 0.06818, 'I': 0.06818,
 *  'L': 0.13636,'M': 0.04545, 'N': 0.04545, 'W': 0.09091, 'Y': 0.15909}
 *
 * In the same reasoning as above, each of the probabilities above is then multiplied
 * by the score of the corresponding letter (0.5 in the case of '|'). Therefore,
 * Looking up "GOOD" in the final forwardGreedyMap will return
 * {'|': 0.113635, 'S': 0.06818, 'E': 0.09091, 'H': 0.27272, 'I': 0.06818,
 *  'L': 0.13636, 'M': 0.13635, 'N': 0.04545, 'W': 0.36364, 'Y': 0.63636}
 *
 * Assuming the hand contains all the letters above, 'Y' tile will be placed after
 * "GOOD".
 *
 * The use of graph has been considered, but using two maps is deemed more of
 * a straightforward approach. For example, say there are two vertices whose
 * associated letters are both "O". Given the word "GOOD" has been processed,
 * it will be convoluted to determine the directionality of each of the edges
 * connecting two of the vertices. Also, the useful algorithms associated with graph
 * are not applicable in solving the problem at hand. Therefore, two hash maps for
 * forward and backward search are chosen.
 */


// TODO fix the error in generating files later
#include "MapBuilder.h"

typedef std::map<char, size_t> letterToScoreMapType;
typedef std::shared_ptr<letterToScoreMapType> letterToScorePtr;
typedef std::unordered_map<std::string, letterToScorePtr> greedyMapType;
typedef std::shared_ptr<greedyMapType> greedyMapPtr;

class GreedyMapBuilder: public MapBuilder {
public:
    GreedyMapBuilder(bool forward,
                     const std::string& inFile,
                     const std::string& outFile);
    void build();
    void convertToMap() override;
    void saveMap() override;

    greedyMapPtr greedyMap;
//    std::vector<std::string> wordsInQueue;
    bool forward;
    std::string inFile;
    std::string outFile;
};


#endif //ASSIGNMENT3_GREEDYMAPBUILDER_H
