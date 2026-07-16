#include "test_mob_call_for_help_policy_2686.h"
#include "map/entities/mob_call_for_help_policy.h"
#include <iostream>
auto runMobCallForHelpPolicy2686SelfTests() -> bool { bool ok=!mobcallforhelphelpers::HasCallForHelp(0,0x20)&&mobcallforhelphelpers::HasCallForHelp(0x20,0x20)&&!mobcallforhelphelpers::HasCallForHelp(0x100,0x20); if(!ok)std::cerr<<"mob call for help 2686 failed\n"; return ok; }
