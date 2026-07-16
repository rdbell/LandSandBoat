#include "test_c2s_gameok_runtime.h"
#include "map/packets/c2s/0x00c_gameok.h"
#include <iostream>

auto runC2SGameOKRuntimeSelfTests() -> bool
{
    using Action          = gameokhelpers::Action;
    const auto full       = gameokhelpers::BuildActionPlan({ true, true, false, true });
    const auto noOptional = gameokhelpers::BuildActionPlan({ false, true, true, true });
    if (full.count != 29 || full.actions[0] != Action::EnterZone || full.actions[25] != Action::UpdateTreasurePool || full.actions[26] != Action::SpawnTransport || full.actions[27] != Action::SpawnPersistedPet || full.actions[28] != Action::ResetPetZoningInfo || noOptional.count != 26 || noOptional.actions[25] != Action::SpawnTransport)
    {
        std::cerr << "c2s GAMEOK runtime self-test failed\n";
        return false;
    }
    return true;
}
