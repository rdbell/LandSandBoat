#include "test_reive_despawn_on_disable_2889.h"

#include "map/colonization_reive_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "reive despawn on disable 2889 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Lua disableReive despawn gate for dual-wire checks:
// if mob:isSpawned() then DespawnMob(entryId) end
auto inlineShouldDespawnOnDisable(const bool isSpawned) -> bool
{
    return isSpawned;
}

} // namespace

// Pure dual-wire expansion for reivehelpers::ShouldDespawnOnDisable
// (Lua disableReive despawn gate for defenders and obstacles).
auto runReiveDespawnOnDisable2889SelfTests() -> bool
{
    using reivehelpers::ShouldDespawnOnDisable;

    bool ok = true;

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
        const bool pure    = c.isSpawned;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == pure, "dual-wire free == isSpawned") && ok;
        ok = expect(got == inlineF, "dual-wire free == inline Lua formula") && ok;
    }

    // Residual 1038 pins.
    ok = expect(ShouldDespawnOnDisable(true), "residual spawned → despawn") && ok;
    ok = expect(!ShouldDespawnOnDisable(false), "residual not spawned → no despawn") && ok;

    // Compose identity: both bool injects match formula and inline.
    for (const bool isSpawned : { false, true })
    {
        const bool got  = ShouldDespawnOnDisable(isSpawned);
        const bool want = isSpawned;
        ok = expect(got == want, "compose free == formula") && ok;
        ok = expect(got == inlineShouldDespawnOnDisable(isSpawned), "compose free == inline") && ok;
    }

    return ok;
}
