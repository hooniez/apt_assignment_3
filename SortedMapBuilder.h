////
//// Created by Myeonghoon Sun on 22/5/2022.
////
//
//#ifndef ASSIGNMENT3_SORTEDMAPBUILDER_H
//#define ASSIGNMENT3_SORTEDMAPBUILDER_H
//
//#include "MapBuilder.h"
//
//typedef std::set<std::string> originalWordsType;
//typedef std::shared_ptr<originalWordsType> originalWordsPtr;
//typedef std::unordered_map<std::string, originalWordsPtr> sortedMapType;
//typedef std::shared_ptr<sortedMapType> sortedMapPtr;
//
//class SortedMapBuilder: public MapBuilder {
//public:
//    SortedMapBuilder(const std::string& inFile, const std::string& outFile);
//    void build();
//    void convertToMap() override;
//    void saveMap() override;
//private:
//    sortedMapPtr sortedWordMap;
//    std::string inFile;
//    std::string outFile;
//};
//
//
//#endif //ASSIGNMENT3_SORTEDMAPBUILDER_H
