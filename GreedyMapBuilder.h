//
// Created by Myeonghoon Sun on 22/5/2022.
//

#ifndef ASSIGNMENT3_GREEDYMAPBUILDER_H
#define ASSIGNMENT3_GREEDYMAPBUILDER_H

#include "MapBuilder.h"
typedef std::map<std::string, std::set<char>> greedyMapBuilder;

/*
 * GreedyMapBuilder was used to create the text files called backwardMap and forwardMap so that
 * the pre-calculated information in them can be loaded into maps quickly.
 *
 * Even though the use of an ordered_map is not ideal when dealing with 2 million elements,
 * the produced files will be in a more legible format. As the build process is only invoked once
 * to produce the texts, which then can be read into unordered_maps when a program runs as usual,
 * the initial time cost to build is well amortised in the name of storing files that
 * are easy to read in case of a bug.
 *
 * Step 1)
 * Each wordBeingBuilt in the provided dictionary is iterated in one character increments
 * and the corresponding frequencies are recorded.
 * For example, if the wordBeingBuilt "GOOD" is processed, forwardMap generates
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
 * The use of graph has been considered, but using two maps is deemed more of
 * a straightforward approach. For example, say there are two vertices whose
 * associated letters are both "O". Given the wordBeingBuilt "GOOD" has been processed,
 * it will be convoluted to determine the directionality of each of the edges
 * connecting two of the vertices. Also, the useful algorithms associated with graph
 * are not applicable in solving the problem at hand. Therefore, two hash maps for
 * forward and backward search are chosen.
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
