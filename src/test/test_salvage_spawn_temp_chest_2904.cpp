#include "test_salvage_spawn_temp_chest_2904.h"

#include "map/salvage_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "salvage spawn temp chest 2904 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Lua spawnTempChest pure status gate for dual-wire cross-check:
//   casket:getStatus() == xi.status.DISAPPEAR
auto inlineShouldSpawnOnTempChestCasket(const uint8 status) -> bool
{
    return status == salvagehelpers::kStatusDisappear;
}

} // namespace

// Pure dual-wire expansion for salvagehelpers::ShouldSpawnOnTempChestCasket
// (Lua spawnTempChest status == DISAPPEAR gate; slice 2904).
auto runSalvageSpawnTempChest2904SelfTests() -> bool
{
    using salvagehelpers::ShouldSpawnOnTempChestCasket;
    using salvagehelpers::kStatusDisappear;
    using salvagehelpers::kStatusNormal;

    bool ok = true;

    // Status catalog pins (match Go StatusNormal / StatusDisappear).
    ok = expect(kStatusNormal == 0, "StatusNormal pin") && ok;
    ok = expect(kStatusDisappear == 2, "StatusDisappear pin") && ok;

    // Residual 1083 ShouldSpawnOnTempChestCasket pins.
    ok = expect(ShouldSpawnOnTempChestCasket(kStatusDisappear), "DISAPPEAR should spawn") && ok;
    ok = expect(!ShouldSpawnOnTempChestCasket(kStatusNormal), "NORMAL should not spawn") && ok;
    ok = expect(!ShouldSpawnOnTempChestCasket(1), "status 1 should not spawn") && ok;
    ok = expect(ShouldSpawnOnTempChestCasket(2), "literal DISAPPEAR 2 should spawn") && ok;

    // --- ShouldSpawnOnTempChestCasket table ---
    const struct
    {
        uint8       status;
        bool        want;
        const char* label;
    } cases[] = {
        { kStatusDisappear, true, "StatusDisappear pin" },
        { 2, true, "literal DISAPPEAR 2" },
        { kStatusNormal, false, "StatusNormal pin" },
        { 0, false, "literal NORMAL 0" },
        { 1, false, "status 1" },
        { 3, false, "status 3" },
        { 255, false, "status max uint8" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldSpawnOnTempChestCasket(c.status);
        const bool inlineC = inlineShouldSpawnOnTempChestCasket(c.status);
        const bool compose = c.status == kStatusDisappear;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineC, "dual-wire ShouldSpawnOnTempChestCasket == inline Lua") && ok;
        ok = expect(got == compose, "ShouldSpawnOnTempChestCasket == status == StatusDisappear") && ok;
    }

    // Host compose: only status == DISAPPEAR succeeds; NORMAL and other
    // statuses skip the casket (search continues / no spawn claim).
    const struct
    {
        uint8       status;
        bool        want;
        const char* label;
    } composeCases[] = {
        { kStatusDisappear, true, "compose DISAPPEAR eligible" },
        { kStatusNormal, false, "compose NORMAL blocks" },
        { 1, false, "compose status 1 blocks" },
        { 2, true, "compose literal 2 eligible" },
        { 99, false, "compose status 99 blocks" },
    };

    for (const auto& c : composeCases)
    {
        const bool got     = ShouldSpawnOnTempChestCasket(c.status);
        const bool inlineC = inlineShouldSpawnOnTempChestCasket(c.status);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineC, "compose dual-wire free == inline") && ok;
    }

    return ok;
}
