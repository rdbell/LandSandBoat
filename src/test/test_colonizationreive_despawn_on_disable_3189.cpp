#include "test_colonizationreive_despawn_on_disable_3189.h"

#include "map/colonization_reive_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "colonizationreive ShouldDespawnOnDisable 3189 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Lua disableReive despawn gate for dual-wire cross-check (dedicated 3189):
// if mob:isSpawned() then DespawnMob(entryId) end
auto inlineShouldDespawnOnDisable(const bool isSpawned) -> bool
{
    return isSpawned;
}

// Compact dual-wire pin matching Go pinShouldDespawnOnDisable3189 / C++ capacity:
//   isSpawned
auto pinShouldDespawnOnDisable(const bool isSpawned) -> bool
{
    return isSpawned;
}

} // namespace

// Pure dual-wire expansion for reivehelpers::ShouldDespawnOnDisable
// (Lua disableReive despawn gate for defenders and obstacles;
// OmegaXI internal/colonizationreive; dedicated slice 3189).
//
// Coverage:
//   - free == inline == pin == isSpawned
//   - residual 1038 / 2889 pins still hold
//   - sibling ShouldSpawnOnEnable left alone (3163) not re-expanded
auto runColonizationreiveDespawnOnDisable3189SelfTests() -> bool
{
    using reivehelpers::ShouldDespawnOnDisable;
    using reivehelpers::ShouldSpawnOnEnable;

    bool ok = true;

    // Residual 1038 / 2889 pins still hold under dual-wire.
    ok = expect(ShouldDespawnOnDisable(true), "residual spawned → despawn") && ok;
    ok = expect(!ShouldDespawnOnDisable(false), "residual not spawned → no despawn") && ok;

    // --- Composition table: free == inline == pin ---
    const struct
    {
        bool        isSpawned;
        bool        want;
        const char* label;
    } cases[] = {
        { true, true, "spawned → despawn" },
        { false, false, "not spawned → no despawn" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldDespawnOnDisable(c.isSpawned);
        const bool inlineF = inlineShouldDespawnOnDisable(c.isSpawned);
        const bool pin     = pinShouldDespawnOnDisable(c.isSpawned);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF && got == pin, "dual-wire free == inline == pin") && ok;
        ok = expect(got == c.isSpawned, "free == isSpawned formula") && ok;
    }

    // Compose identity: both bool injects match free == inline == pin.
    for (const bool isSpawned : { false, true })
    {
        const bool got  = ShouldDespawnOnDisable(isSpawned);
        const bool want = isSpawned;
        ok = expect(got == want, "compose free == formula") && ok;
        ok = expect(got == inlineShouldDespawnOnDisable(isSpawned), "compose free == inline") && ok;
        ok = expect(got == pinShouldDespawnOnDisable(isSpawned), "compose free == pin") && ok;
    }

    // Sibling ShouldSpawnOnEnable left alone (3163) still holds.
    ok = expect(ShouldSpawnOnEnable(false), "sibling left alone not alive → spawn") && ok;
    ok = expect(!ShouldSpawnOnEnable(true), "sibling left alone alive → no spawn") && ok;

    return ok;
}
