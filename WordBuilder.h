//
// Created by Myeonghoon Sun on 20/5/2022.
//

#ifndef ASSIGNMENT3_WORDBUILDER_H
#define ASSIGNMENT3_WORDBUILDER_H

#include <string>

/*
 * WordBuilder is initialised when --ai or --hint options are enabled
 *
 *
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
#include <unordered_map>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <sstream>

#include "types.h"
#include "player.h"
#include "board.h"
typedef std::map<char, float> letterToScoreMapType;
typedef std::shared_ptr<letterToScoreMapType> letterToScorePtr;
typedef std::unordered_map<std::string, letterToScorePtr> aiMapType;
typedef std::shared_ptr<aiMapType> aiMapPtr;

class WordBuilder : public Player {
public:
    // This constructor is called when --hint is typed by itself
    WordBuilder(const std::string& forwardSearchMapFileName,
                const std::string& backwardSearchMapFileName,
                BoardPtr board,
                bool canGiveHints);
    // This constructor is called when --ai is typed by itself OR with --hint
    WordBuilder(const std::string& forwardSearchMapFileName,
                const std::string& backwardSearchMapFileName,
                const std::string& name,
                BoardPtr board,
                bool canGiveHints);
    void readFileToMap(const std::string &, const aiMapPtr& map);
    void initialiseMaps();
    void execute();


    bool canGiveHints;
    bool isPlaying;

private:

    aiMapPtr forwardMap;
    aiMapPtr backwardMap;
    BoardPtr board;
};
typedef std::shared_ptr<WordBuilder> WordBuilderPtr;

#endif //ASSIGNMENT3_WORDBUILDER_H
