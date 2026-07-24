#include "test_zone_mob_aggro_tap_6255.h"

#include "map/zone_mob_aggro_tap.h"

#include <iostream>

namespace
{
auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "zone mob aggro tap 6255 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

// Pins CZoneEntities::tapMobAggro's exclusion, roam-follow, and normal-aggro
// admission policy before it delegates to the mob controller.
auto runZoneMobAggroTap6255SelfTests() -> bool
{
    using namespace zoneaggrohelpers;

    bool ok = true;
    for (const bool mobDead : { false, true })
    {
        for (const bool charDead : { false, true })
        {
            for (const bool visibleGM : { false, true })
            {
                for (const bool hasMaster : { false, true })
                {
                    const bool want = mobDead || charDead || visibleGM || hasMaster;
                    ok              = expect(ShouldSkipMobAggro(mobDead, charDead, visibleGM, hasMaster) == want,
                                 "mob aggro early exclusions") &&
                         ok;
                }
            }
        }
    }

    for (const bool hasFollowFlag : { false, true })
    {
        ok = expect(ShouldUseRoamFollow(hasFollowFlag) == hasFollowFlag, "follow flag selects roam follow branch") && ok;
    }

    for (const bool difficultyAboveTooWeak : { false, true })
    {
        for (const bool charSitting : { false, true })
        {
            for (const bool alwaysAggro : { false, true })
            {
                const bool want = difficultyAboveTooWeak || charSitting || alwaysAggro;
                ok              = expect(ShouldAttemptMobAggro(difficultyAboveTooWeak, charSitting, alwaysAggro) == want,
                             "normal aggro needs difficulty, sitting, or always-aggro eligibility") &&
                     ok;
            }
        }
    }

    return ok;
}
