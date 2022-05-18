//
// Created by Myeonghoon Sun on 18/5/2022.
//

#ifndef ASSIGNMENT3_AIMAPBUILDER_H
#define ASSIGNMENT3_AIMAPBUILDER_H

/*
 * Two maps are used to make the best decision from a given tile
 * or given continuous tiles without a gap.
 *
 * Each of the tiles placeable is searched by its letter in both
 * forwardAiMap and backwardAiMap, by which one with the highest value
 * (the probability of a letter among valid letters * the score of a letter)
 * can be found.
 *
 */

#include <map>

struct AiMapBuilder {
    // Build a forwardAiMap, a map used to find the best tile after a given tile(s)
    static void buildForwardAiMap();
    // Build a backwardAiMap, a map used to find the best tile before a given tile(s)
    static void buildBackwardAiMap();

    /*
     * Each word in the given dictionary is iterated in one character increments
     * and the corresponding frequencies are recorded.
     * For example, if the word "good" is given, forwardAiMap generates
     * {
     *  "g":{'o':1,'\0':0}, "o":{'o':1,'\0':0}, "o":{'d':1,'\0':0}, "d":{'':1,'\0':0},
     *  "go":{'o':1,'\0':0}, "oo":{'d':1,'\0':0}, "od":{'':1,'\0':0}
     *  "goo":{'d':1,'\0':0}, "ood":{'':1,'\0':0},
     *  "good":{'':1,'\0':1}
     * }
     * where the sub key '\0' is used to mark the main key as a valid word
     * and the sub key '' denotes the main key has no more letter following.
     *
     *
     */
    static std::map<std::string, std::map<char, int>> forwardAiMap;
    static std::map<std::string, std::map<char, int>> backwardAiMap;

};


#endif //ASSIGNMENT3_AIMAPBUILDER_H
