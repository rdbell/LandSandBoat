#include "test_battle_animation_1658.h"

#include "map/battle_animation_capacity.h"

#include <iostream>

namespace
{
using namespace battleanimationhelpers;

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "battle animation 1658 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

auto runBattleAnimation1658SelfTests() -> bool
{
    bool ok = true;

    // --- animation pins ---
    ok = expect(AnimationChocobo == 5 && AnimationHealing == 33 && AnimationSit == 47, "core pins") && ok;
    ok = expect(AnimationSitchair0 == 63 && AnimationSitchair10 == 73 && AnimationMount == 85, "range pins") && ok;
    ok = expect(AnimationSitchair1 == 64 && AnimationSitchair5 == 68 && AnimationSitchair9 == 72, "mid sitchair pins") && ok;

    // --- IsMounted pins ---
    ok = expect(IsMounted(AnimationChocobo), "chocobo mounted") && ok;
    ok = expect(IsMounted(AnimationMount), "mount mounted") && ok;

    // --- IsMounted neighbors outside ---
    ok = expect(!IsMounted(0) && !IsMounted(4) && !IsMounted(6), "chocobo neighbors") && ok;
    ok = expect(!IsMounted(AnimationHealing) && !IsMounted(AnimationSit), "sit not mounted") && ok;
    ok = expect(!IsMounted(AnimationSitchair0) && !IsMounted(AnimationSitchair10), "sitchair not mounted") && ok;
    ok = expect(!IsMounted(62) && !IsMounted(74), "sitchair neighbors not mounted") && ok;
    ok = expect(!IsMounted(84) && !IsMounted(86), "mount neighbors") && ok;
    ok = expect(!IsMounted(255), "mount high") && ok;

    // --- IsSitting healing / sit ---
    ok = expect(IsSitting(AnimationHealing), "healing sitting") && ok;
    ok = expect(IsSitting(AnimationSit), "sit sitting") && ok;

    // --- IsSitting sitchair range (each pin + full span) ---
    for (std::uint8_t anim = AnimationSitchair0; anim <= AnimationSitchair10; ++anim)
    {
        ok = expect(IsSitting(anim), "sitchair range member") && ok;
    }
    ok = expect(IsSitting(AnimationSitchair0) && IsSitting(AnimationSitchair5) && IsSitting(AnimationSitchair10),
                "sitchair explicit pins") &&
         ok;

    // --- IsSitting neighbors outside ---
    ok = expect(!IsSitting(0) && !IsSitting(AnimationChocobo), "none/chocobo not sitting") && ok;
    ok = expect(!IsSitting(32) && !IsSitting(34), "healing neighbors") && ok;
    ok = expect(!IsSitting(46) && !IsSitting(48), "sit neighbors") && ok;
    ok = expect(!IsSitting(62) && !IsSitting(74), "sitchair neighbors 62/74") && ok;
    ok = expect(!IsSitting(84) && !IsSitting(86), "mount neighbors not sitting") && ok;
    ok = expect(!IsSitting(AnimationMount), "mount not sitting") && ok;
    ok = expect(!IsSitting(255), "sitting high") && ok;

    // --- mounted and sitting are disjoint for known pins ---
    ok = expect(!IsSitting(AnimationChocobo) && !IsSitting(AnimationMount), "mounted not sitting") && ok;
    ok = expect(!IsMounted(AnimationHealing) && !IsMounted(AnimationSit), "sitting not mounted") && ok;

    return ok;
}
