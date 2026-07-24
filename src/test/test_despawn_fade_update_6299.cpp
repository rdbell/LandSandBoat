#include "test_despawn_fade_update_6299.h"

#include "map/ai/states/despawn_fade_update.h"

#include <cstdint>
#include <iostream>

namespace
{
auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "despawn fade update 6299 self-test failed: " << label << '\n';
    }

    return condition;
}
} // namespace

auto runDespawnFadeUpdate6299SelfTests() -> bool
{
    using despawnfadeupdate::hasBehaviorNoDespawn;
    using despawnfadeupdate::kBehaviorNoDespawn;
    using despawnfadeupdate::kStatusDisappear;
    using despawnfadeupdate::ready;
    using despawnfadeupdate::shouldComplete;
    using despawnfadeupdate::shouldProcessUpdate;
    using despawnfadeupdate::shouldPushFadeOut;

    // Constants match base_entity.h / mob_entity.h
    const bool constantsOK =
        expect(kStatusDisappear == 2, "STATUS_TYPE::DISAPPEAR == 2") &&
        expect(kBehaviorNoDespawn == 0x001, "BEHAVIOR_NO_DESPAWN == 0x001") &&
        expect(!hasBehaviorNoDespawn(0), "zero behavior has no NO_DESPAWN") &&
        expect(hasBehaviorNoDespawn(kBehaviorNoDespawn), "flag alone matches") &&
        expect(hasBehaviorNoDespawn(static_cast<uint16_t>(kBehaviorNoDespawn | 0x002)), "combined flags match");

    // Ctor FadeOut SCHEDULOR: shouldPushFadeOut
    const bool fadeOK =
        expect(shouldPushFadeOut(false, 0, 0), "normal fade pushes") &&
        expect(!shouldPushFadeOut(true, 0, 0), "instant skips FadeOut") &&
        expect(!shouldPushFadeOut(false, kStatusDisappear, 0), "already DISAPPEAR skips") &&
        expect(!shouldPushFadeOut(false, 0, kBehaviorNoDespawn), "NO_DESPAWN skips FadeOut") &&
        expect(!shouldPushFadeOut(true, kStatusDisappear, 0), "instant+DISAPPEAR skips") &&
        expect(!shouldPushFadeOut(true, kStatusDisappear, kBehaviorNoDespawn), "all block flags skip") &&
        expect(shouldPushFadeOut(false, 1, 0), "UPDATE status still fades");

    // Update outer/inner gates
    const bool processOK =
        expect(shouldProcessUpdate(false, 0), "incomplete normal processes") &&
        expect(!shouldProcessUpdate(true, 0), "completed does not process") &&
        expect(!shouldProcessUpdate(false, kBehaviorNoDespawn), "NO_DESPAWN does not process") &&
        expect(ready(true), "tick >= deadline ready") &&
        expect(!ready(false), "tick < deadline not ready");

    // Combined shouldComplete (production Update inject)
    const bool completeOK =
        expect(shouldComplete(false, 0, true), "ready incomplete completes") &&
        expect(!shouldComplete(false, 0, false), "before deadline does not complete") &&
        expect(!shouldComplete(true, 0, true), "already completed does not re-complete") &&
        expect(!shouldComplete(false, kBehaviorNoDespawn, true), "NO_DESPAWN does not complete");

    // Production inject shape: host casts status/behavior then calls pure gates
    const auto injectPush = [](const bool instant, const uint8_t status, const uint16_t behavior) -> bool {
        return shouldPushFadeOut(instant, status, behavior);
    };
    const auto injectComplete = [](const bool completed, const uint16_t behavior, const bool tickReady) -> bool {
        return shouldComplete(completed, behavior, tickReady);
    };

    const bool injectOK =
        expect(injectPush(false, 0, 0), "inject normal fade") &&
        expect(!injectPush(true, 0, 0), "inject instant no fade") &&
        expect(injectComplete(false, 0, true), "inject ready completes") &&
        expect(!injectComplete(false, kBehaviorNoDespawn, true), "inject NO_DESPAWN holds");

    return constantsOK && fadeOK && processOK && completeOK && injectOK;
}
