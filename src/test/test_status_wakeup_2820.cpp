#include "test_status_wakeup_2820.h"

#include "map/status_effect_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "status wakeup 2820 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runStatusWakeup2820SelfTests() -> bool
{
    using statuseffecthelpers::IsAsleepEffectID;
    using statuseffecthelpers::PlanWakeUp;
    using statuseffecthelpers::StatusIDLullaby;
    using statuseffecthelpers::StatusIDSleepI;
    using statuseffecthelpers::StatusIDSleepIi;
    using statuseffecthelpers::WakeUpStatusIDCount;
    using statuseffecthelpers::WakeUpStatusIDs;

    bool ok = true;

    // 1) fixed length
    const auto ids = WakeUpStatusIDs();
    ok             = expect(ids.size() == WakeUpStatusIDCount, "count constant") && ok;
    ok             = expect(ids.size() == 3, "count literal 3") && ok;

    // 2) production order: SleepI, SleepIi, Lullaby
    ok = expect(ids[0] == StatusIDSleepI, "order[0] SleepI") && ok;
    ok = expect(ids[1] == StatusIDSleepIi, "order[1] SleepIi") && ok;
    ok = expect(ids[2] == StatusIDLullaby, "order[2] Lullaby") && ok;

    // 3) numeric pins (data/status_effects.yaml)
    ok = expect(ids[0] == 2, "SleepI == 2") && ok;
    ok = expect(ids[1] == 19, "SleepIi == 19") && ok;
    ok = expect(ids[2] == 193, "Lullaby == 193") && ok;
    ok = expect(StatusIDSleepI == 2 && StatusIDSleepIi == 19 && StatusIDLullaby == 193, "const pins") && ok;

    // 4) PlanWakeUp alias matches WakeUpStatusIDs
    const auto plan = PlanWakeUp();
    ok              = expect(plan.size() == ids.size(), "plan size") && ok;
    ok              = expect(plan.data() == ids.data() || (plan.size() == 3 && plan[0] == ids[0] && plan[1] == ids[1] && plan[2] == ids[2]),
                "plan contents") &&
         ok;
    ok = expect(plan[0] == 2 && plan[1] == 19 && plan[2] == 193, "plan pins") && ok;

    // 5) every plan id is an asleep-family member; non-sleep is not in plan
    for (const auto id : ids)
    {
        ok = expect(IsAsleepEffectID(id), "plan id is asleep") && ok;
    }
    ok = expect(!IsAsleepEffectID(10), "stun not asleep") && ok;
    bool hasStun = false;
    for (const auto id : ids)
    {
        if (id == 10)
        {
            hasStun = true;
        }
    }
    ok = expect(!hasStun, "stun not in plan") && ok;

    // 6) order is strict: SleepI before SleepIi before Lullaby
    ok = expect(ids[0] < ids[1] && ids[1] < ids[2], "increasing id order (coincidental)") && ok;
    ok = expect(ids[0] != ids[1] && ids[1] != ids[2] && ids[0] != ids[2], "distinct ids") && ok;

    // 7) second call returns same ordered sequence (stable plan)
    const auto again = WakeUpStatusIDs();
    ok               = expect(again.size() == 3 && again[0] == 2 && again[1] == 19 && again[2] == 193, "stable second call") && ok;

    return ok;
}
