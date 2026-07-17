#include "test_zone_mob_away_2848.h"

#include "map/zone_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "zone mob away 2848 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline production formula for dual-wire cross-check:
// (alive && !fullyHealed) || pathingHome
// where alive := !isDead.
auto inlineAway(const bool isDead, const bool isFullyHealed, const bool isPathingHome) -> bool
{
    return (!isDead && !isFullyHealed) || isPathingHome;
}

} // namespace

auto runZoneMobAway2848SelfTests() -> bool
{
    using zonehelpers::IsMobAwayFromHome;
    using zonehelpers::ShouldReportAllMobsHomeAndHealed;

    bool ok = true;

    // Full 3-bool truth table (8 cases) for IsMobAwayFromHome.
    // Formula: (!isDead && !isFullyHealed) || isPathingHome
    const struct
    {
        bool        isDead;
        bool        isFullyHealed;
        bool        isPathingHome;
        bool        want;
        const char* label;
    } cases[] = {
        // alive, unhealed, not pathing → away
        { false, false, false, true, "alive unhealed not-pathing" },
        // alive, unhealed, pathing → away
        { false, false, true, true, "alive unhealed pathing" },
        // alive, fully healed, not pathing → home
        { false, true, false, false, "alive healed not-pathing" },
        // alive, fully healed, pathing → away (pathing wins)
        { false, true, true, true, "alive healed pathing" },
        // dead, unhealed, not pathing → home (dead is not "alive")
        { true, false, false, false, "dead unhealed not-pathing" },
        // dead, unhealed, pathing → away (pathing wins)
        { true, false, true, true, "dead unhealed pathing" },
        // dead, fully healed, not pathing → home
        { true, true, false, false, "dead healed not-pathing" },
        // dead, fully healed, pathing → away (pathing wins)
        { true, true, true, true, "dead healed pathing" },
    };

    for (const auto& c : cases)
    {
        const bool got    = IsMobAwayFromHome(c.isDead, c.isFullyHealed, c.isPathingHome);
        const bool pure   = inlineAway(c.isDead, c.isFullyHealed, c.isPathingHome);
        ok                = expect(got == c.want, c.label) && ok;
        ok                = expect(got == pure, "dual-wire matches inline formula") && ok;
        ok                = expect(pure == c.want, "inline formula matches want") && ok;
    }

    // Spot-check labels from slice 1363 residual coverage remain stable.
    ok = expect(IsMobAwayFromHome(false, false, false), "legacy away unhealed") && ok;
    ok = expect(IsMobAwayFromHome(false, true, true), "legacy away pathing") && ok;
    ok = expect(!IsMobAwayFromHome(true, false, false), "legacy dead home") && ok;
    ok = expect(!IsMobAwayFromHome(false, true, false), "legacy healed home") && ok;

    // Host CheckMobsPathedBack dual-wires anyAway through IsMobAwayFromHome
    // then ShouldReportAllMobsHomeAndHealed(hasMobList, anyAway).
    // Empty list always reports all home; non-empty reports !anyAway.
    ok = expect(ShouldReportAllMobsHomeAndHealed(false, false), "empty list home") && ok;
    ok = expect(ShouldReportAllMobsHomeAndHealed(false, true), "empty list ignores anyAway") && ok;
    ok = expect(ShouldReportAllMobsHomeAndHealed(true, false), "list all home") && ok;
    ok = expect(!ShouldReportAllMobsHomeAndHealed(true, true), "list some away") && ok;

    // Compose: inject per-mob away flags into anyAway aggregate.
    {
        const bool mobs[] = {
            IsMobAwayFromHome(false, true, false),  // home
            IsMobAwayFromHome(false, false, false), // away unhealed
            IsMobAwayFromHome(true, true, false),   // dead home
        };
        bool anyAway = false;
        for (const bool away : mobs)
        {
            if (away)
            {
                anyAway = true;
                break;
            }
        }
        ok = expect(anyAway, "compose anyAway from pure flags") && ok;
        ok = expect(!ShouldReportAllMobsHomeAndHealed(true, anyAway), "compose report some away") && ok;
    }
    {
        const bool mobs[] = {
            IsMobAwayFromHome(false, true, false), // home
            IsMobAwayFromHome(true, false, false), // dead home
            IsMobAwayFromHome(true, true, false),  // dead home
        };
        bool anyAway = false;
        for (const bool away : mobs)
        {
            if (away)
            {
                anyAway = true;
                break;
            }
        }
        ok = expect(!anyAway, "compose all home") && ok;
        ok = expect(ShouldReportAllMobsHomeAndHealed(true, anyAway), "compose report all home") && ok;
    }

    return ok;
}
