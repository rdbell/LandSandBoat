#include "test_mob_geode_pool_2713.h"
#include "map/entities/mob_geode_pool.h"
#include <iostream>
auto runMobGeodePool2713SelfTests() -> bool { using mobgeodepoolhelpers::EligibleGeodes; const bool ok=EligibleGeodes(false,99,1).empty()&&EligibleGeodes(true,49,1).empty()&&EligibleGeodes(true,50,1)==std::vector<uint16>{FLAME_GEODE}&&EligibleGeodes(true,80,8)==std::vector<uint16>{SHADOW_GEODE,FENRITE}&&EligibleGeodes(true,80,0)==std::vector<uint16>{0,0}; if(!ok)std::cerr<<"mob geode pool 2713 failed\n"; return ok; }
