#include "test_s2c_talknum_runtime.h"

#include "map/packets/s2c/talknum_runtime.h"

auto runS2CTalkNumRuntimeSelfTests() -> bool
{
    const auto npc    = talknumhelpers::PlanFor({ .uniqueNo = 1, .actIndex = 2 }, 0x1234, true, 3);
    const auto pc     = talknumhelpers::PlanFor({ .uniqueNo = 1, .actIndex = 2, .playerCharacter = true }, 0x1234, true, 3);
    const auto hidden = talknumhelpers::PlanFor({ .uniqueNo = 1, .actIndex = 2 }, 0x9001, false, 3);
    return npc.UniqueNo == 1 && npc.ActIndex == 2 && npc.MesNum == 0x1234 && npc.Type == 3 && pc.MesNum == 0x9234 && hidden.MesNum == 0x1001;
}
