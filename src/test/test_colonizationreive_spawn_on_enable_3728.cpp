#include "test_colonizationreive_spawn_on_enable_3728.h"

#include "map/colonization_reive_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "colonizationreive ShouldSpawnOnEnable 3728 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Lua enableReive spawn gate for dual-wire cross-check
// (dedicated 3728 expand residual 2882 / prior 3683 / prior 3638 / prior 3593 /
// prior 3499 / prior 3465 / prior 3411 / prior 3335 / prior 3163):
// if not mob:isAlive() then SpawnMob(entryId) end
auto inlineShouldSpawnOnEnable(const bool isAlive) -> bool
{
    return !isAlive;
}

// Compact dual-wire pin matching Go pinShouldSpawnOnEnable3728 / C++ capacity
// (return !isAlive):
//   !isAlive
auto pinShouldSpawnOnEnable(const bool isAlive) -> bool
{
    return !isAlive;
}

// Prior dedicated 3683 pin (independence / free==inline==pin==pin3683):
//   !isAlive
auto pinShouldSpawnOnEnable3683(const bool isAlive) -> bool
{
    return !isAlive;
}

// Prior dedicated 3638 pin (independence cross-check):
//   !isAlive
auto pinShouldSpawnOnEnable3638(const bool isAlive) -> bool
{
    return !isAlive;
}

// Prior dedicated 3593 pin (independence cross-check):
//   !isAlive
auto pinShouldSpawnOnEnable3593(const bool isAlive) -> bool
{
    return !isAlive;
}

// Prior dedicated 3499 pin (independence cross-check):
//   !isAlive
auto pinShouldSpawnOnEnable3499(const bool isAlive) -> bool
{
    return !isAlive;
}

// Prior dedicated 3465 pin (independence cross-check):
//   !isAlive
auto pinShouldSpawnOnEnable3465(const bool isAlive) -> bool
{
    return !isAlive;
}

// Prior dedicated 3411 pin (independence cross-check):
//   !isAlive
auto pinShouldSpawnOnEnable3411(const bool isAlive) -> bool
{
    return !isAlive;
}

// Prior dedicated 3335 pin (independence cross-check):
//   !isAlive
auto pinShouldSpawnOnEnable3335(const bool isAlive) -> bool
{
    return !isAlive;
}

// Prior dedicated 3163 pin (independence cross-check):
//   !isAlive
auto pinShouldSpawnOnEnable3163(const bool isAlive) -> bool
{
    return !isAlive;
}

} // namespace

// Pure dual-wire expansion for reivehelpers::ShouldSpawnOnEnable
// (Lua enableReive spawn gate for defenders and obstacles;
// OmegaXI internal/colonizationreive; dedicated slice 3728 expand residual
// 2882 / prior dedicated 3683 / prior dedicated 3638 / prior dedicated 3593 /
// prior dedicated 3499 / prior dedicated 3465 / prior dedicated 3411 /
// prior dedicated 3335 / prior dedicated 3163 — formula unchanged).
//
// Coverage:
//   - free == inline == pin == pin3683 == !isAlive
//   - residual poles: isAlive true / false
//   - dense 2^1 boolean space
//   - residual 1038 / 2882 / prior 3163 / prior 3335 / prior 3411 / prior 3465 / prior 3499 / prior 3593 / prior 3638 / prior 3683 pins still hold
//   - sibling ShouldDespawnOnDisable residual 2889 / 3189 / 3252 left alone
auto runColonizationreiveSpawnOnEnable3728SelfTests() -> bool
{
    using reivehelpers::ShouldDespawnOnDisable;
    using reivehelpers::ShouldSpawnOnEnable;

    bool ok = true;

    // Residual 1038 / 2882 / prior 3163 / prior 3335 / prior 3411 / prior 3465 / prior 3499 / prior 3593 / prior 3638 / prior 3683 pins still hold under dual-wire.
    ok = expect(ShouldSpawnOnEnable(false), "residual not alive → spawn") && ok;
    ok = expect(!ShouldSpawnOnEnable(true), "residual alive → no spawn") && ok;

    // --- Eligible / blocked paths ---
    ok = expect(ShouldSpawnOnEnable(false), "eligible not alive → spawn") && ok;
    ok = expect(!ShouldSpawnOnEnable(true), "blocked alive → no spawn") && ok;

    const struct
    {
        bool        isAlive;
        bool        want;
        const char* label;
    } cases[] = {
        // Classic dual poles / residual poles true/false.
        { false, true, "not alive → spawn" },
        { true, false, "alive → no spawn" },

        // Residual 2882 re-pins.
        { false, true, "residual 2882 false" },
        { true, false, "residual 2882 true" },

        // Prior dedicated 3163 re-pins.
        { false, true, "prior 3163 false" },
        { true, false, "prior 3163 true" },

        // Prior dedicated 3335 re-pins.
        { false, true, "prior 3335 false" },
        { true, false, "prior 3335 true" },

        // Prior dedicated 3411 re-pins.
        { false, true, "prior 3411 false" },
        { true, false, "prior 3411 true" },

        // Prior dedicated 3465 re-pins.
        { false, true, "prior 3465 false" },
        { true, false, "prior 3465 true" },

        // Prior dedicated 3499 re-pins.
        { false, true, "prior 3499 false" },
        { true, false, "prior 3499 true" },

        // Prior dedicated 3593 re-pins.
        { false, true, "prior 3593 false" },
        { true, false, "prior 3593 true" },

        // Prior dedicated 3638 re-pins.
        { false, true, "prior 3638 false" },
        { true, false, "prior 3638 true" },

        // Prior dedicated 3683 re-pins.
        { false, true, "prior 3683 false" },
        { true, false, "prior 3683 true" },

        // Residual 1038 re-pins.
        { false, true, "residual 1038 false" },
        { true, false, "residual 1038 true" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldSpawnOnEnable(c.isAlive);
        const bool inlineF = inlineShouldSpawnOnEnable(c.isAlive);
        const bool pinGot  = pinShouldSpawnOnEnable(c.isAlive);
        const bool pin3683 = pinShouldSpawnOnEnable3683(c.isAlive);
        const bool wantPin = !c.isAlive;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldSpawnOnEnable dual-wire == inline LSB formula") && ok;
        ok = expect(got == pinGot, "ShouldSpawnOnEnable dual-wire == pin") && ok;
        ok = expect(got == pin3683, "ShouldSpawnOnEnable dual-wire == pin3683") && ok;
        ok = expect(got == wantPin, "ShouldSpawnOnEnable == pin formula (!isAlive)") && ok;
    }

    // Pin composition: free function is !isAlive only.
    ok = expect(ShouldSpawnOnEnable(false), "false must spawn") && ok;
    ok = expect(!ShouldSpawnOnEnable(true), "true must not spawn") && ok;
    ok = expect(ShouldSpawnOnEnable(false) == pinShouldSpawnOnEnable(false), "free==pin false") && ok;
    ok = expect(ShouldSpawnOnEnable(true) == pinShouldSpawnOnEnable(true), "free==pin true") && ok;
    ok = expect(ShouldSpawnOnEnable(false) == pinShouldSpawnOnEnable3683(false), "free==pin3683 false") && ok;
    ok = expect(ShouldSpawnOnEnable(true) == pinShouldSpawnOnEnable3683(true), "free==pin3683 true") && ok;
    ok = expect(ShouldSpawnOnEnable(false) == inlineShouldSpawnOnEnable(false), "free==inline false") && ok;
    ok = expect(ShouldSpawnOnEnable(true) == inlineShouldSpawnOnEnable(true), "free==inline true") && ok;

    // Explicit residual poles free == inline == pin == pin3683 for true / false.
    for (const bool pole : { false, true })
    {
        const bool got     = ShouldSpawnOnEnable(pole);
        const bool inlineF = inlineShouldSpawnOnEnable(pole);
        const bool pinGot  = pinShouldSpawnOnEnable(pole);
        const bool pin3683 = pinShouldSpawnOnEnable3683(pole);
        ok                 = expect(got == !pole, "pole free == !isAlive") && ok;
        ok                 = expect(got == inlineF, "pole free == inline") && ok;
        ok                 = expect(got == pinGot, "pole free == pin") && ok;
        ok                 = expect(got == pin3683, "pole free == pin3683") && ok;
    }

    // Dense compose: full 2^1 boolean space — free == pin == inline == pin3683.
    for (const bool isAlive : { false, true })
    {
        const bool got  = ShouldSpawnOnEnable(isAlive);
        const bool want = !isAlive;
        ok              = expect(got == want, "compose free == pin formula") && ok;
        ok              = expect(got == inlineShouldSpawnOnEnable(isAlive), "compose free == inline") && ok;
        ok              = expect(got == pinShouldSpawnOnEnable(isAlive), "compose free == pin") && ok;
        ok              = expect(got == pinShouldSpawnOnEnable3683(isAlive), "compose free == pin3683") && ok;
    }

    // Prior dedicated 3683 independence: free still matches prior pin.
    for (const bool isAlive : { false, true })
    {
        const bool got      = ShouldSpawnOnEnable(isAlive);
        const bool priorPin = pinShouldSpawnOnEnable3683(isAlive);
        ok                  = expect(got == priorPin, "prior 3683 independence") && ok;
    }

    // Prior dedicated 3638 independence: free still matches prior pin.
    for (const bool isAlive : { false, true })
    {
        const bool got      = ShouldSpawnOnEnable(isAlive);
        const bool priorPin = pinShouldSpawnOnEnable3638(isAlive);
        ok                  = expect(got == priorPin, "prior 3638 independence") && ok;
    }

    // Prior dedicated 3593 independence: free still matches prior pin.
    for (const bool isAlive : { false, true })
    {
        const bool got      = ShouldSpawnOnEnable(isAlive);
        const bool priorPin = pinShouldSpawnOnEnable3593(isAlive);
        ok                  = expect(got == priorPin, "prior 3593 independence") && ok;
    }

    // Prior dedicated 3499 independence: free still matches prior pin.
    for (const bool isAlive : { false, true })
    {
        const bool got      = ShouldSpawnOnEnable(isAlive);
        const bool priorPin = pinShouldSpawnOnEnable3499(isAlive);
        ok                  = expect(got == priorPin, "prior 3499 independence") && ok;
    }

    // Prior dedicated 3465 independence: free still matches prior pin.
    for (const bool isAlive : { false, true })
    {
        const bool got      = ShouldSpawnOnEnable(isAlive);
        const bool priorPin = pinShouldSpawnOnEnable3465(isAlive);
        ok                  = expect(got == priorPin, "prior 3465 independence") && ok;
    }

    // Prior dedicated 3411 independence: free still matches prior pin.
    for (const bool isAlive : { false, true })
    {
        const bool got      = ShouldSpawnOnEnable(isAlive);
        const bool priorPin = pinShouldSpawnOnEnable3411(isAlive);
        ok                  = expect(got == priorPin, "prior 3411 independence") && ok;
    }

    // Prior dedicated 3335 independence: free still matches prior pin.
    for (const bool isAlive : { false, true })
    {
        const bool got      = ShouldSpawnOnEnable(isAlive);
        const bool priorPin = pinShouldSpawnOnEnable3335(isAlive);
        ok                  = expect(got == priorPin, "prior 3335 independence") && ok;
    }

    // Prior dedicated 3163 independence: free still matches prior pin.
    for (const bool isAlive : { false, true })
    {
        const bool got      = ShouldSpawnOnEnable(isAlive);
        const bool priorPin = pinShouldSpawnOnEnable3163(isAlive);
        ok                  = expect(got == priorPin, "prior 3163 independence") && ok;
    }

    // Host-style inject poles: mob:isAlive() → !isAlive.
    // (Live enableReive defenders / obstacles residual.)
    const struct
    {
        bool mobIsAlive;
        bool want;
    } hostPoles[] = {
        { false, true },
        { true, false },
    };
    for (const auto& h : hostPoles)
    {
        const bool isAlive = h.mobIsAlive;
        const bool inject  = !isAlive;
        ok                 = expect(ShouldSpawnOnEnable(isAlive) == inject, "host inject dual-wire identity") && ok;
        ok                 = expect(ShouldSpawnOnEnable(isAlive) == inlineShouldSpawnOnEnable(isAlive),
                    "host inject free == inline") &&
             ok;
        ok = expect(ShouldSpawnOnEnable(isAlive) == pinShouldSpawnOnEnable(isAlive),
                    "host inject free == pin") &&
             ok;
        ok = expect(ShouldSpawnOnEnable(isAlive) == pinShouldSpawnOnEnable3683(isAlive),
                    "host inject free == pin3683") &&
             ok;
        ok = expect(ShouldSpawnOnEnable(isAlive) == h.want, "host inject want pin") && ok;
    }

    // Production enableReive path semantics:
    // Eligible (not alive) → may SpawnMob(entryId).
    // Blocked (alive) → skip spawn.
    ok = expect(ShouldSpawnOnEnable(false), "enableReive eligible not alive") && ok;
    ok = expect(!ShouldSpawnOnEnable(true), "enableReive blocked alive") && ok;

    // Sibling ShouldDespawnOnDisable residual 2889 / prior 3189 / dedicated 3252
    // left alone.
    ok = expect(ShouldDespawnOnDisable(true), "sibling residual spawned → despawn") && ok;
    ok = expect(!ShouldDespawnOnDisable(false), "sibling residual not spawned → no despawn") && ok;

    return ok;
}
