//
// Created by Myeonghoon Sun on 19/5/2022.
//

#include "AiMap.h"

AiMap::AiMap(): aiMap(std::make_shared<aiMapType>()){
    aiMap->reserve(NUM_ELEMENTS_IN_AI_MAP);
}



