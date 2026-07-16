#include "test_mob_home_distance_policy_2671.h"
#include "map/entities/mob_home_distance_policy.h"
#include <iostream>
auto runMobHomeDistancePolicy2671SelfTests() -> bool { const bool ok = !mobhomedistancehelpers::IsFarFromHome(4.9F, 5.0F) && !mobhomedistancehelpers::IsFarFromHome(5.0F, 5.0F) && mobhomedistancehelpers::IsFarFromHome(5.1F, 5.0F); if (!ok) std::cerr << "mob home-distance 2671 failed\n"; return ok; }
