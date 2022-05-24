////
//// Created by Myeonghoon Sun on 20/5/2022.
////
//
//#ifndef ASSIGNMENT3_COORDINATEDTILE_H
//#define ASSIGNMENT3_COORDINATEDTILE_H
//
///*
// * Tiles placed will be stored in a max heap in Board class
// * to quickly find out the most lucrative places for AI to place tiles
// *
// * Coordinated stores x and y coordinates in addition to what's already
// * tracked by Tile
// */
//#include <tuple>
//
//#include "types.h"
//#include "tile.h"
//
//typedef std::tuple<size_t, size_t> gridLocType;
//typedef std::shared_ptr<gridLocType> gridLocPtr;
//
//class CoordinatedTile: public Tile {
//public:
//    CoordinatedTile(TilePtr, gridLocPtr gridLoc, PlacedDirection);
//    virtual ~CoordinatedTile() = default;
//    PlacedDirection placedDir;
//    gridLocPtr gridLoc;
//};
//typedef std::shared_ptr<CoordinatedTile> CoordinatedTilePtr;
//
//struct CompareCoordinatedTile {
//public:
//    // Return true if ct1's y coordinate is less than ct2's y coordinate if
//    // the direction is HORIZONTAL
//    // return true if ct1's x coordinate is less than ct2's x coordinate if
//    // the direction is VERTICAL
//    bool operator()(const CoordinatedTilePtr& ct1, const CoordinatedTilePtr& ct2) const {
//        bool res;
//        if (ct1->placedDir == HORIZONTAL) {
//            res = std::get<Y>(*ct1->gridLoc) < std::get<Y>(*ct2->gridLoc);
//        } else {
//            res = std::get<X>(*ct1->gridLoc) < std::get<X>(*ct2->gridLoc);
//        }
//        return res;
//    }
//};
//
//typedef std::set<CoordinatedTilePtr, CompareCoordinatedTile> PlacedTilesType;
//typedef std::shared_ptr<PlacedTilesType> PlacedTilesPtr;



#endif //ASSIGNMENT3_COORDINATEDTILE_H
