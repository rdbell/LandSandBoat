#include "test_status_bust_effect_2817.h"

#include "map/status_effect_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "status bust effect 2817 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runStatusBustEffect2817SelfTests() -> bool
{
    using statuseffecthelpers::IsBustEffectForAbility;
    using statuseffecthelpers::StatusIDBust;

    bool ok = true;

    // 1) match: Bust status + subPower equals ability/roll id
    ok = expect(IsBustEffectForAbility(StatusIDBust, 97, StatusIDBust, 97), "match fighters roll") && ok;
    ok = expect(IsBustEffectForAbility(309, 0, 309, 0), "match zero ability") && ok;

    // 2) reject wrong status ID
    ok = expect(!IsBustEffectForAbility(310, 97, StatusIDBust, 97), "reject fighters_roll status") && ok;
    ok = expect(!IsBustEffectForAbility(0, 97, StatusIDBust, 97), "reject status 0") && ok;
    ok = expect(!IsBustEffectForAbility(StatusIDBust + 1, 97, StatusIDBust, 97), "reject status+1") && ok;

    // 3) reject wrong subPower / ability id
    ok = expect(!IsBustEffectForAbility(StatusIDBust, 97, StatusIDBust, 98), "reject wrong ability") && ok;
    ok = expect(!IsBustEffectForAbility(StatusIDBust, 0, StatusIDBust, 1), "reject zero vs one") && ok;

    // 4) both wrong
    ok = expect(!IsBustEffectForAbility(1, 2, StatusIDBust, 97), "reject both wrong") && ok;

    // 5) injected bustStatusID (host may pass enum value explicitly)
    ok = expect(IsBustEffectForAbility(42, 7, 42, 7), "custom bust id match") && ok;
    ok = expect(!IsBustEffectForAbility(42, 7, 43, 7), "custom bust id miss") && ok;

    // 6) StatusIDBust constant pins yaml id 309
    ok = expect(StatusIDBust == 309, "StatusIDBust == 309") && ok;

    // 7) compose table: pure AND of equality
    const struct
    {
        uint16      statusID;
        uint16      subPower;
        uint16      bustStatusID;
        uint16      abilityOrRollID;
        bool        want;
        const char* label;
    } cases[] = {
        { 309, 97, 309, 97, true, "compose match" },
        { 309, 97, 309, 96, false, "compose ability" },
        { 308, 97, 309, 97, false, "compose status" },
        { 0, 0, 0, 0, true, "compose zeros" },
        { 309, 65535, 309, 65535, true, "compose max subPower" },
        { 309, 65535, 309, 0, false, "compose max vs zero" },
    };
    for (const auto& c : cases)
    {
        const bool got      = IsBustEffectForAbility(c.statusID, c.subPower, c.bustStatusID, c.abilityOrRollID);
        const bool composed = c.statusID == c.bustStatusID && c.subPower == c.abilityOrRollID;
        ok                  = expect(got == c.want && got == composed, c.label) && ok;
    }

    return ok;
}
