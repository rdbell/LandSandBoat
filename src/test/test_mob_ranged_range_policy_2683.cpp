#include "test_mob_ranged_range_policy_2683.h"
#include "map/entities/mob_ranged_range_policy.h"
#include <iostream>
auto runMobRangedRangePolicy2683SelfTests() -> bool { bool ok=mobrangedrangehelpers::GetRangedAttackRange(20)==20.F&&mobrangedrangehelpers::GetRangedAttackRange(0)==14.F&&mobrangedrangehelpers::GetRangedAttackRange(-1)==14.F; if(!ok)std::cerr<<"mob ranged range 2683 failed\n"; return ok; }
