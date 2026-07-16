#include "test_mob_widescan_policy_2668.h"
#include "map/entities/mob_widescan_policy.h"
#include <iostream>
#include <tuple>
auto runMobWidescanPolicy2668SelfTests() -> bool { bool ok = true; for (const auto [base, blocked, want] : { std::tuple{false,false,false}, std::tuple{true,false,true}, std::tuple{true,true,false}, std::tuple{false,true,false} }) { if (mobwidescanhelpers::IsWideScannable(base, blocked) != want) { std::cerr << "mob widescan 2668 failed\n"; ok=false; } } return ok; }
