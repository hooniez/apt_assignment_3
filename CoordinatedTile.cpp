////
//// Created by Myeonghoon Sun on 20/5/2022.
////
//
//#include "CoordinatedTile.h"
//CoordinatedTile::CoordinatedTile(TilePtr tile,
//                                 gridLocPtr gridLoc,
//                                 Angle dir):
//                                 Tile(*tile),
//                                 placedDir(dir),
//                                 gridLoc(gridLoc){}
//
////// The less than operator is overridden so that std::set<CoordinatedTilePtr>.begin()
////// returns the leftmost placed tile if the placed direction is horizontal
////// OR the uppermost placed tile if the placed direction is vertical.
////bool CoordinatedTile::operator<(const std::shared_ptr<CoordinatedTile>& a,
////                                const std::shared_ptr<CoordinatedTile>& b) const {
////    bool res;
////    if (placedDir == HORIZONTAL) {
////        // If the placed direction is horizontal, the tiles' y coordinates vary
////        res = std::get<Y>(a->gridLoc) < std::get<Y>(b->gridLoc);
////    } else {
////        // If the placed direction is VERTICAL, the tiles' x coordinates vary
////        res = std::get<X>(a->gridLoc) < std::get<X>(b->gridLoc);
////    }
////    return res;
////}