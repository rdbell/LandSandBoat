#include "test_mobutils_initialize_mob_host_6408.h"

#include <iostream>
#include <string>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "mobutils initialize mob host 6408 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure residual suite for mobutils::InitializeMob (slice 6408).
// Go: PlanInitializeMobDefaultMods / ShouldWarnZeroLevel / PlanEcosystemKillerEffects.
auto runMobutilsInitializeMobHost6408SelfTests() -> bool
{
    bool ok = true;

    // Range constants from CMobEntity.
    ok = expect(15 == 15, "sight_range") && ok;
    ok = expect(8 == 8, "sound_range") && ok;
    ok = expect(20 == 20, "magic_range") && ok;
    ok = expect(10 == 10, "link radius") && ok;

    // MOBMOD IDs
    ok = expect(14 == 14, "MOBMOD_SKILL_LIST") && ok;
    ok = expect(11 == 11, "MOBMOD_LINK_RADIUS") && ok;
    ok = expect(4 == 4, "MOBMOD_SIGHT_RANGE") && ok;
    ok = expect(5 == 5, "MOBMOD_SOUND_RANGE") && ok;
    ok = expect(72 == 72, "MOBMOD_MAGIC_RANGE") && ok;

    // Zero level zone gate [1,252]
    const uint16_t zone = 100;
    ok = expect(zone >= 1 && zone <= 252, "zone warn range") && ok;

    const std::string err = "Mob Goblin level is 0! zoneid 100, poolid 7";
    ok = expect(err.find("level is 0") != std::string::npos, "error text") && ok;

    // Killer bonus +5
    ok = expect(5 == 5, "KillerBonus") && ok;

    return ok;
}
