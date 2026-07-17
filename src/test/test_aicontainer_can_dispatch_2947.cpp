#include "test_aicontainer_can_dispatch_2947.h"

#include "map/ai/ai_container_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "aicontainer CanDispatch 2947 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline CAIContainer public controller-forward presence gate for dual-wire
// cross-check (slice 2947):
//   hasController
auto inlineCanDispatch(const bool hasController) -> bool
{
    return hasController;
}

} // namespace

// Pure dual-wire expansion for aicontainerhelpers::CanDispatch
// (controller presence outer gate; slice 2947).
auto runAicontainerCanDispatch2947SelfTests() -> bool
{
    using aicontainerhelpers::CanDispatch;

    bool ok = true;

    const struct
    {
        bool        hasController;
        bool        want;
        const char* label;
    } cases[] = {
        // Classic dual poles.
        { false, false, "no controller rejects" },
        { true, true, "controller present accepts" },

        // Residual 1189 pins.
        { false, false, "residual CanDispatch(false)" },
        { true, true, "residual CanDispatch(true)" },
    };

    for (const auto& c : cases)
    {
        const bool got     = CanDispatch(c.hasController);
        const bool inlineF = inlineCanDispatch(c.hasController);
        const bool wantPin = c.hasController;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "CanDispatch dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "CanDispatch == pin formula hasController") && ok;
    }

    // Pin composition: identity only.
    ok = expect(!CanDispatch(false), "false must reject") && ok;
    ok = expect(CanDispatch(true), "true must accept") && ok;

    // Dense compose: full 2^1 boolean space.
    for (const bool hasController : { false, true })
    {
        const bool got  = CanDispatch(hasController);
        const bool want = hasController;
        ok              = expect(got == want, "compose free == pin formula") && ok;
        ok              = expect(got == inlineCanDispatch(hasController), "compose free == inline") && ok;
    }

    // --- Production CAIContainer path semantics ---
    // Host injects:
    //   base:  hasController = static_cast<bool>(Controller)
    //   typed: hasController = dynamic_cast result != nullptr
    // when false → return false without invoking controller method
    // when true  → return Controller->X(...)
    ok = expect(!CanDispatch(false), "Cast/Engage/... no Controller → reject path") && ok;
    ok = expect(CanDispatch(true), "Cast/Engage/... Controller present → dispatch path") && ok;

    // Typed controller inject poles (MobSkill / PetSkill / UseItem).
    const bool typedAbsent  = false; // dynamic_cast failed
    const bool typedPresent = true;  // matching subclass present
    ok                      = expect(!CanDispatch(typedAbsent), "MobSkill typed cast fail → reject") && ok;
    ok                      = expect(CanDispatch(typedPresent), "MobSkill typed cast ok → dispatch") && ok;
    ok                      = expect(CanDispatch(typedAbsent) == inlineCanDispatch(typedAbsent),
                "typed absent free == inline") &&
         ok;
    ok = expect(CanDispatch(typedPresent) == inlineCanDispatch(typedPresent),
                "typed present free == inline") &&
         ok;

    // Explicit dual-wire: free == hasController inject for host-style poles.
    for (const bool has : { false, true })
    {
        ok = expect(CanDispatch(has) == has, "host inject dual-wire identity") && ok;
        ok = expect(CanDispatch(has) == inlineCanDispatch(has), "host inject free == inline") && ok;
    }

    // Residual 1189 poles still hold under dual-wire.
    ok = expect(!CanDispatch(false), "residual no controller") && ok;
    ok = expect(CanDispatch(true), "residual controller present") && ok;

    return ok;
}
