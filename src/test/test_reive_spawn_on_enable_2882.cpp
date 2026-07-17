#include "test_reive_spawn_on_enable_2882.h"

#include "map/colonization_reive_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "reive spawn on enable 2882 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Lua enableReive spawn gate for dual-wire checks:
// if not mob:isAlive() then SpawnMob(entryId) end
auto inlineShouldSpawnOnEnable(const bool isAlive) -> bool
{
    return !isAlive;
}

} // namespace

// Pure dual-wire expansion for reivehelpers::ShouldSpawnOnEnable
// (Lua enableReive spawn gate for defenders and obstacles).
auto runReiveSpawnOnEnable2882SelfTests() -> bool
{
    using reivehelpers::ShouldSpawnOnEnable;

    bool ok = true;

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
        const bool pure    = !c.isAlive;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == pure, "dual-wire free == !isAlive") && ok;
        ok = expect(got == inlineF, "dual-wire free == inline Lua formula") && ok;
    }

    // Residual 1038 pins.
    ok = expect(ShouldSpawnOnEnable(false), "residual not alive → spawn") && ok;
    ok = expect(!ShouldSpawnOnEnable(true), "residual alive → no spawn") && ok;

    // Compose identity: both bool injects match formula and inline.
    for (const bool isAlive : { false, true })
    {
        const bool got  = ShouldSpawnOnEnable(isAlive);
        const bool want = !isAlive;
        ok = expect(got == want, "compose free == formula") && ok;
        ok = expect(got == inlineShouldSpawnOnEnable(isAlive), "compose free == inline") && ok;
    }

    return ok;
}
