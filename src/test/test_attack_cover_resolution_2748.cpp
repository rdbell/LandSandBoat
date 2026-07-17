#include "test_attack_cover_resolution_2748.h"
#include "map/attack_capacity.h"
auto runAttackCoverResolution2748SelfTests() -> bool
{
    using namespace attackhelpers;
    const auto live = ResolveCoverCheck(true, true);
    const auto missing = ResolveCoverCheck(false, true);
    const auto dead = ResolveCoverCheck(true, false);
    return live.covered && live.replaceVictim && !missing.covered && !missing.replaceVictim && !dead.covered && !dead.replaceVictim;
}
