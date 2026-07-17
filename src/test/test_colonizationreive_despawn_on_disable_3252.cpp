#include "test_colonizationreive_despawn_on_disable_3252.h"

#include "map/colonization_reive_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "colonizationreive ShouldDespawnOnDisable 3252 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Lua disableReive despawn gate for dual-wire cross-check
// (dedicated 3252 expand residual 2889 / prior 3189):
// if mob:isSpawned() then DespawnMob(entryId) end
auto inlineShouldDespawnOnDisable(const bool isSpawned) -> bool
{
    return isSpawned;
}

// Compact dual-wire pin matching Go pinShouldDespawnOnDisable3252 / C++ capacity
// (direct return):
//   isSpawned
auto pinShouldDespawnOnDisable(const bool isSpawned) -> bool
{
    return isSpawned;
}

// Prior dedicated 3189 pin (independence cross-check):
//   isSpawned
auto pinShouldDespawnOnDisable3189(const bool isSpawned) -> bool
{
    return isSpawned;
}

} // namespace

// Pure dual-wire expansion for reivehelpers::ShouldDespawnOnDisable
// (Lua disableReive despawn gate for defenders and obstacles;
// OmegaXI internal/colonizationreive; dedicated slice 3252 expand residual
// 2889 / prior dedicated 3189 — formula unchanged).
//
// Coverage:
//   - free == inline == pin == isSpawned (direct return)
//   - residual poles: isSpawned true / false
//   - dense 2^1 boolean space
//   - residual 1038 / 2889 / prior 3189 pins still hold
//   - sibling ShouldSpawnOnEnable residual 2882 / 3163 left alone
auto runColonizationreiveDespawnOnDisable3252SelfTests() -> bool
{
    using reivehelpers::ShouldDespawnOnDisable;
    using reivehelpers::ShouldSpawnOnEnable;

    bool ok = true;

    // Residual 1038 / 2889 / prior 3189 pins still hold under dual-wire.
    ok = expect(ShouldDespawnOnDisable(true), "residual spawned → despawn") && ok;
    ok = expect(!ShouldDespawnOnDisable(false), "residual not spawned → no despawn") && ok;

    // --- Eligible / blocked paths ---
    ok = expect(ShouldDespawnOnDisable(true), "eligible spawned → despawn") && ok;
    ok = expect(!ShouldDespawnOnDisable(false), "blocked not spawned → no despawn") && ok;

    const struct
    {
        bool        isSpawned;
        bool        want;
        const char* label;
    } cases[] = {
        // Classic dual poles / residual poles true/false.
        { true, true, "spawned → despawn" },
        { false, false, "not spawned → no despawn" },

        // Residual 2889 re-pins.
        { true, true, "residual 2889 true" },
        { false, false, "residual 2889 false" },

        // Prior dedicated 3189 re-pins.
        { true, true, "prior 3189 true" },
        { false, false, "prior 3189 false" },

        // Residual 1038 re-pins.
        { true, true, "residual 1038 true" },
        { false, false, "residual 1038 false" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldDespawnOnDisable(c.isSpawned);
        const bool inlineF = inlineShouldDespawnOnDisable(c.isSpawned);
        const bool pinGot  = pinShouldDespawnOnDisable(c.isSpawned);
        const bool wantPin = c.isSpawned;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldDespawnOnDisable dual-wire == inline LSB formula") && ok;
        ok = expect(got == pinGot, "ShouldDespawnOnDisable dual-wire == pin") && ok;
        ok = expect(got == wantPin, "ShouldDespawnOnDisable == pin formula (identity)") && ok;
    }

    // Pin composition: free function is identity on isSpawned only.
    ok = expect(ShouldDespawnOnDisable(true), "true must despawn") && ok;
    ok = expect(!ShouldDespawnOnDisable(false), "false must not despawn") && ok;
    ok = expect(ShouldDespawnOnDisable(true) == pinShouldDespawnOnDisable(true), "free==pin true") && ok;
    ok = expect(ShouldDespawnOnDisable(false) == pinShouldDespawnOnDisable(false), "free==pin false") && ok;
    ok = expect(ShouldDespawnOnDisable(true) == inlineShouldDespawnOnDisable(true), "free==inline true") && ok;
    ok = expect(ShouldDespawnOnDisable(false) == inlineShouldDespawnOnDisable(false), "free==inline false") && ok;

    // Explicit residual poles free == inline == pin for true / false.
    for (const bool pole : { false, true })
    {
        const bool got     = ShouldDespawnOnDisable(pole);
        const bool inlineF = inlineShouldDespawnOnDisable(pole);
        const bool pinGot  = pinShouldDespawnOnDisable(pole);
        ok                 = expect(got == pole, "pole free == identity") && ok;
        ok                 = expect(got == inlineF, "pole free == inline") && ok;
        ok                 = expect(got == pinGot, "pole free == pin") && ok;
    }

    // Dense compose: full 2^1 boolean space — free == pin == inline.
    for (const bool isSpawned : { false, true })
    {
        const bool got  = ShouldDespawnOnDisable(isSpawned);
        const bool want = isSpawned;
        ok              = expect(got == want, "compose free == pin formula") && ok;
        ok              = expect(got == inlineShouldDespawnOnDisable(isSpawned), "compose free == inline") && ok;
        ok              = expect(got == pinShouldDespawnOnDisable(isSpawned), "compose free == pin") && ok;
    }

    // Prior dedicated 3189 independence: free still matches prior pin.
    for (const bool isSpawned : { false, true })
    {
        const bool got      = ShouldDespawnOnDisable(isSpawned);
        const bool priorPin = pinShouldDespawnOnDisable3189(isSpawned);
        ok                  = expect(got == priorPin, "prior 3189 independence") && ok;
    }

    // Host-style inject poles: mob:isSpawned() → isSpawned.
    // (Live disableReive defenders / obstacles residual.)
    const struct
    {
        bool mobIsSpawned;
        bool want;
    } hostPoles[] = {
        { true, true },
        { false, false },
    };
    for (const auto& h : hostPoles)
    {
        const bool isSpawned = h.mobIsSpawned;
        const bool inject    = isSpawned;
        ok                   = expect(ShouldDespawnOnDisable(isSpawned) == inject, "host inject dual-wire identity") && ok;
        ok                   = expect(ShouldDespawnOnDisable(isSpawned) == inlineShouldDespawnOnDisable(isSpawned),
                    "host inject free == inline") &&
             ok;
        ok = expect(ShouldDespawnOnDisable(isSpawned) == pinShouldDespawnOnDisable(isSpawned),
                    "host inject free == pin") &&
             ok;
        ok = expect(ShouldDespawnOnDisable(isSpawned) == h.want, "host inject want pin") && ok;
    }

    // Production disableReive path semantics:
    // Eligible (spawned) → may DespawnMob(entryId).
    // Blocked (not spawned) → skip despawn.
    ok = expect(ShouldDespawnOnDisable(true), "disableReive eligible spawned") && ok;
    ok = expect(!ShouldDespawnOnDisable(false), "disableReive blocked not spawned") && ok;

    // Sibling ShouldSpawnOnEnable residual 2882 / dedicated 3163 left alone.
    ok = expect(ShouldSpawnOnEnable(false), "sibling residual not alive → spawn") && ok;
    ok = expect(!ShouldSpawnOnEnable(true), "sibling residual alive → no spawn") && ok;

    return ok;
}
