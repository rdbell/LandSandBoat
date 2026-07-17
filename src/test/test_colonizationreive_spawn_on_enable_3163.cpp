#include "test_colonizationreive_spawn_on_enable_3163.h"

#include "map/colonization_reive_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "colonizationreive ShouldSpawnOnEnable 3163 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Lua enableReive spawn gate for dual-wire cross-check (dedicated 3163):
// if not mob:isAlive() then SpawnMob(entryId) end
auto inlineShouldSpawnOnEnable(const bool isAlive) -> bool
{
    return !isAlive;
}

// Compact dual-wire pin matching Go pinShouldSpawnOnEnable3163 / C++ capacity:
//   !isAlive
auto pinShouldSpawnOnEnable(const bool isAlive) -> bool
{
    return !isAlive;
}

} // namespace

// Pure dual-wire expansion for reivehelpers::ShouldSpawnOnEnable
// (Lua enableReive spawn gate for defenders and obstacles;
// OmegaXI internal/colonizationreive; dedicated slice 3163).
//
// Coverage:
//   - free == inline == pin == !isAlive
//   - residual 1038 / 2882 pins still hold
//   - sibling ShouldDespawnOnDisable residual (2889) not re-expanded
auto runColonizationreiveSpawnOnEnable3163SelfTests() -> bool
{
    using reivehelpers::ShouldDespawnOnDisable;
    using reivehelpers::ShouldSpawnOnEnable;

    bool ok = true;

    // Residual 1038 / 2882 pins still hold under dual-wire.
    ok = expect(ShouldSpawnOnEnable(false), "residual not alive → spawn") && ok;
    ok = expect(!ShouldSpawnOnEnable(true), "residual alive → no spawn") && ok;

    // --- Composition table: free == inline == pin ---
    const struct
    {
        bool        isAlive;
        bool        want;
        const char* label;
    } cases[] = {
        { false, true, "not alive → spawn" },
        { true, false, "alive → no spawn" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldSpawnOnEnable(c.isAlive);
        const bool inlineF = inlineShouldSpawnOnEnable(c.isAlive);
        const bool pin     = pinShouldSpawnOnEnable(c.isAlive);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF && got == pin, "dual-wire free == inline == pin") && ok;
        ok = expect(got == !c.isAlive, "free == !isAlive formula") && ok;
    }

    // Compose identity: both bool injects match free == inline == pin.
    for (const bool isAlive : { false, true })
    {
        const bool got  = ShouldSpawnOnEnable(isAlive);
        const bool want = !isAlive;
        ok = expect(got == want, "compose free == formula") && ok;
        ok = expect(got == inlineShouldSpawnOnEnable(isAlive), "compose free == inline") && ok;
        ok = expect(got == pinShouldSpawnOnEnable(isAlive), "compose free == pin") && ok;
    }

    // Sibling residual 2889 still holds (not re-expanded under 3163).
    ok = expect(ShouldDespawnOnDisable(true), "sibling residual spawned → despawn") && ok;
    ok = expect(!ShouldDespawnOnDisable(false), "sibling residual not spawned → no despawn") && ok;

    return ok;
}
