#include "test_salvage_spawn_temp_chest_casket_3209.h"

#include "map/salvage_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "salvage spawn temp chest casket 3209 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Lua spawnTempChest pure status gate for dual-wire cross-check:
//   casket:getStatus() == xi.status.DISAPPEAR
auto inlineShouldSpawnOnTempChestCasket(const uint8 status) -> bool
{
    return status == salvagehelpers::kStatusDisappear;
}

// Compact dual-wire pin matching Go pinShouldSpawnOnTempChestCasket3209:
//   status == kStatusDisappear
auto pinShouldSpawnOnTempChestCasket(const uint8 status) -> bool
{
    return status == salvagehelpers::kStatusDisappear;
}

} // namespace

// Pure dual-wire expansion for salvagehelpers::ShouldSpawnOnTempChestCasket
// (Lua spawnTempChest status == DISAPPEAR gate; OmegaXI internal/salvage;
// slice 3209).
//
// Coverage:
//   - DISAPPEAR (2) → spawn true (positive pin form)
//   - NORMAL / other statuses → spawn false
//   - free == inline == pin == status==kStatusDisappear
//   - dense poles: 0, 1, 2, kStatusNormal, kStatusDisappear, 99, 255
//   - residual 1083 / 2904 pins still hold
auto runSalvageSpawnTempChestCasket3209SelfTests() -> bool
{
    using salvagehelpers::ShouldSpawnOnTempChestCasket;
    using salvagehelpers::kStatusDisappear;
    using salvagehelpers::kStatusNormal;

    bool ok = true;

    // Status catalog pins (match Go StatusNormal / StatusDisappear).
    ok = expect(kStatusNormal == 0, "StatusNormal pin") && ok;
    ok = expect(kStatusDisappear == 2, "StatusDisappear pin") && ok;

    // Residual 1083 / 2904 ShouldSpawnOnTempChestCasket pins still hold under dual-wire.
    ok = expect(ShouldSpawnOnTempChestCasket(kStatusDisappear), "residual: DISAPPEAR should spawn") && ok;
    ok = expect(!ShouldSpawnOnTempChestCasket(kStatusNormal), "residual: NORMAL should not spawn") && ok;
    ok = expect(!ShouldSpawnOnTempChestCasket(1), "residual: status 1 should not spawn") && ok;
    ok = expect(ShouldSpawnOnTempChestCasket(2), "residual: literal DISAPPEAR 2 should spawn") && ok;
    ok = expect(ShouldSpawnOnTempChestCasket(kStatusDisappear), "residual 2904: StatusDisappear pin should spawn") && ok;
    ok = expect(!ShouldSpawnOnTempChestCasket(kStatusNormal), "residual 2904: StatusNormal pin should not spawn") && ok;

    // --- Eligible spawn path (positive pin form) ---
    ok = expect(ShouldSpawnOnTempChestCasket(kStatusDisappear), "eligible StatusDisappear should spawn") && ok;
    ok = expect(ShouldSpawnOnTempChestCasket(2), "eligible literal DISAPPEAR 2 should spawn") && ok;
    ok = expect(pinShouldSpawnOnTempChestCasket(kStatusDisappear), "eligible pin StatusDisappear should spawn") && ok;
    ok = expect(pinShouldSpawnOnTempChestCasket(2), "eligible pin literal 2 should spawn") && ok;

    // --- Blocked paths ---
    ok = expect(!ShouldSpawnOnTempChestCasket(kStatusNormal), "NORMAL should block spawn") && ok;
    ok = expect(!ShouldSpawnOnTempChestCasket(0), "literal NORMAL 0 should block spawn") && ok;
    ok = expect(!ShouldSpawnOnTempChestCasket(1), "status 1 should block spawn") && ok;
    ok = expect(!ShouldSpawnOnTempChestCasket(99), "status 99 should block spawn") && ok;
    ok = expect(!ShouldSpawnOnTempChestCasket(255), "status 255 should block spawn") && ok;

    // --- Composition table: free == inline == pin == compose ---
    // Dense status poles: 0, 1, 2, kStatusNormal, kStatusDisappear, 99, 255.
    const struct
    {
        uint8       status;
        bool        want;
        const char* label;
    } cases[] = {
        { 2, true, "table literal DISAPPEAR 2" },
        { kStatusDisappear, true, "table StatusDisappear pin" },
        { 0, false, "table literal NORMAL 0" },
        { kStatusNormal, false, "table StatusNormal pin" },
        { 1, false, "table status 1" },
        { 99, false, "table status 99" },
        { 255, false, "table status max uint8" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldSpawnOnTempChestCasket(c.status);
        const bool inlineC = inlineShouldSpawnOnTempChestCasket(c.status);
        const bool pinGot  = pinShouldSpawnOnTempChestCasket(c.status);
        const bool compose = c.status == kStatusDisappear;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineC, "dual-wire free==inline") && ok;
        ok = expect(got == pinGot, "dual-wire free==pin") && ok;
        ok = expect(got == compose, "formula free==status==kStatusDisappear") && ok;
    }

    // --- Production spawnTempChest path semantics ---
    // Eligible → may continue setPos / resetLocalVars / setStatus(NORMAL) / items.
    // Blocked → skip casket (search continues).
    ok = expect(ShouldSpawnOnTempChestCasket(kStatusDisappear), "spawnTempChest eligible → continue path") && ok;
    ok = expect(!ShouldSpawnOnTempChestCasket(kStatusNormal), "spawnTempChest NORMAL → blocked") && ok;
    ok = expect(!ShouldSpawnOnTempChestCasket(1), "spawnTempChest other status → blocked") && ok;

    // Dense compose identity over required poles.
    const uint8 composeStatuses[] = { 0, 1, 2, kStatusNormal, kStatusDisappear, 99, 255 };
    for (const uint8 status : composeStatuses)
    {
        const bool got  = ShouldSpawnOnTempChestCasket(status);
        const bool want = status == kStatusDisappear;
        ok = expect(got == want, "compose status==kStatusDisappear") && ok;
        ok = expect(got == inlineShouldSpawnOnTempChestCasket(status), "compose inline") && ok;
        ok = expect(got == pinShouldSpawnOnTempChestCasket(status), "compose pin") && ok;
    }

    // Compose identity: free function is the gate; host injects status only.
    ok = expect(ShouldSpawnOnTempChestCasket(kStatusDisappear) == pinShouldSpawnOnTempChestCasket(kStatusDisappear),
                "formula free == pin StatusDisappear") &&
         ok;
    ok = expect(!ShouldSpawnOnTempChestCasket(kStatusNormal), "formula NORMAL path must block") && ok;
    ok = expect(ShouldSpawnOnTempChestCasket(kStatusNormal) == pinShouldSpawnOnTempChestCasket(kStatusNormal),
                "formula NORMAL free == pin") &&
         ok;

    return ok;
}
