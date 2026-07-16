#include "test_mob_visibility_flags_2719.h"
#include "map/entities/mob_visibility_flags.h"
#include <iostream>
auto runMobVisibilityFlags2719SelfTests()->bool{uint8 m=4;auto f=mobvisibilityhelpers::SetFlag(2,m,mobvisibilityhelpers::HideHP,true);f=mobvisibilityhelpers::SetFlag(f,m,mobvisibilityhelpers::Untargetable,true);f=mobvisibilityhelpers::SetFlag(f,m,mobvisibilityhelpers::HideHP,false);bool ok=f==0x802&&m==5&&mobvisibilityhelpers::Has(f,mobvisibilityhelpers::Untargetable);if(!ok)std::cerr<<"mob visibility 2719 failed\n";return ok;}
