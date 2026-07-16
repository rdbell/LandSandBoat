#include "test_c2s_login_zone_present_runtime.h"
#include "map/packets/c2s/0x00a_login.h"
auto runC2SLoginZonePresentRuntimeSelfTests() -> bool { std::array<bool,16> s{}; s[1]=true;s[15]=true; const auto p=loginhelpers::MakeZonePresentPlan(s); return p.equipCount==2&&p.equipSlots[0]==1&&p.equipSlots[1]==15; }
