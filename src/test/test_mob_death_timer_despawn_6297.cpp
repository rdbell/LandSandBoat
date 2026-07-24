#include "test_mob_death_timer_despawn_6297.h"

#include "map/ai/states/mob_death_timer_despawn.h"

#include <cstdint>
#include <iostream>

namespace
{
auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "mob death timer despawn 6297 self-test failed: " << label << '\n';
    }

    return condition;
}
} // namespace

auto runMobDeathTimerDespawn6297SelfTests() -> bool
{
    using mobdeathtimer::shouldDespawn;

    // Pure gate: !raisable
    const bool pureOK =
        expect(shouldDespawn(false), "non-raisable despawns") &&
        expect(!shouldDespawn(true), "raisable does not despawn");

    // Host inject from BEHAVIOR_RAISABLE = 0x004 (mob_entity.h)
    constexpr uint16_t kBehaviorRaisable  = 0x004;
    constexpr uint16_t kBehaviorNoDespawn = 0x001;

    const auto injectDespawn = [](const uint16_t behavior) -> bool {
        const bool raisable = (behavior & static_cast<uint16_t>(0x004)) != 0;
        return shouldDespawn(raisable);
    };

    const bool injectOK =
        expect(injectDespawn(0), "zero behavior despawns") &&
        expect(injectDespawn(kBehaviorNoDespawn), "NO_DESPAWN alone despawns") &&
        expect(!injectDespawn(kBehaviorRaisable), "RAISABLE alone does not despawn") &&
        expect(!injectDespawn(static_cast<uint16_t>(kBehaviorRaisable | kBehaviorNoDespawn)),
               "RAISABLE|NO_DESPAWN does not despawn") &&
        expect(injectDespawn(static_cast<uint16_t>(0x002 | 0x008)), "other bits without RAISABLE despawn");

    return pureOK && injectOK;
}
