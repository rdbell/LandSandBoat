#include "test_mob_roam_rate_2677.h"
#include "map/entities/mob_roam_rate.h"
#include <iostream>
auto runMobRoamRate2677SelfTests() -> bool { bool ok=mobroamratehelpers::GetRoamRate(15)==1.5F&&mobroamratehelpers::GetRoamRate(0)==0.0F&&mobroamratehelpers::GetRoamRate(-5)==-0.5F; if(!ok)std::cerr<<"mob roam rate 2677 failed\n"; return ok; }
