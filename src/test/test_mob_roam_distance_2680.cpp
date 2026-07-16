#include "test_mob_roam_distance_2680.h"
#include "map/entities/mob_roam_distance.h"
#include <iostream>
auto runMobRoamDistance2680SelfTests() -> bool { bool ok=mobroamdistancehelpers::GetRoamDistance(10)==10.0F&&mobroamdistancehelpers::GetRoamDistance(0)==0.0F&&mobroamdistancehelpers::GetRoamDistance(-3)==-3.0F; if(!ok)std::cerr<<"mob roam distance 2680 failed\n"; return ok; }
