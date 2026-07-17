#include "test_recast_reset_ability_2800.h"

#include "map/recast_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "recast reset ability 2800 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runRecastResetAbility2800SelfTests() -> bool
{
    using namespace recasthelpers;
    bool ok = true;

    // IsOneHourSpecialRecast: Special, Special2, or both
    ok = expect(IsOneHourSpecialRecast(true, false), "special only") && ok;
    ok = expect(IsOneHourSpecialRecast(false, true), "special2 only") && ok;
    ok = expect(IsOneHourSpecialRecast(true, true), "both specials") && ok;
    ok = expect(!IsOneHourSpecialRecast(false, false), "neither special") && ok;

    // ShouldResetAbilityRecast: inverse of one-hour special
    ok = expect(!ShouldResetAbilityRecast(true, false), "skip special") && ok;
    ok = expect(!ShouldResetAbilityRecast(false, true), "skip special2") && ok;
    ok = expect(!ShouldResetAbilityRecast(true, true), "skip both") && ok;
    ok = expect(ShouldResetAbilityRecast(false, false), "reset normal ability") && ok;

    // Composition: reset iff not one-hour
    const struct
    {
        bool        isSpecial;
        bool        isSpecial2;
        bool        wantReset;
        const char* label;
    } cases[] = {
        { true, false, false, "compose special" },
        { false, true, false, "compose special2" },
        { true, true, false, "compose both" },
        { false, false, true, "compose normal" },
    };
    for (const auto& c : cases)
    {
        const bool oneHour = IsOneHourSpecialRecast(c.isSpecial, c.isSpecial2);
        const bool reset   = ShouldResetAbilityRecast(c.isSpecial, c.isSpecial2);
        ok                 = expect(reset == c.wantReset, c.label) && ok;
        ok                 = expect(reset == !oneHour, "compose inverse") && ok;
    }

    return ok;
}
