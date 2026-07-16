#include "test_mob_force_link_policy_2665.h"
#include "map/entities/mob_force_link_policy.h"
#include <iostream>
#include <tuple>
auto runMobForceLinkPolicy2665SelfTests() -> bool { bool ok = true; for (const auto [d,b,s,w] : { std::tuple{false,false,false,false}, std::tuple{true,false,false,true}, std::tuple{false,true,false,true}, std::tuple{false,false,true,true} }) { if (mobforcelinkhelpers::ShouldForceLink(d,b,s) != w) { std::cerr << "mob force-link 2665 failed\n"; ok=false; } } return ok; }
