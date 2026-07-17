#include "test_salvage_reset_temp_box_3442.h"

#include "map/salvage_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "salvage reset temp box 3442 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Lua resetTempBoxes pure status gate for dual-wire cross-check:
//   casket:getStatus() == xi.status.NORMAL
auto inlineShouldResetTempBox(const uint8 status) -> bool
{
    return status == salvagehelpers::kStatusNormal;
}

// Compact dual-wire pin matching Go pinShouldResetTempBox3442:
//   status == kStatusNormal
auto pinShouldResetTempBox(const uint8 status) -> bool
{
    return status == salvagehelpers::kStatusNormal;
}

// Prior dedicated 3398 pin (same formula; residual suite identity).
auto pinShouldResetTempBox3398(const uint8 status) -> bool
{
    return status == salvagehelpers::kStatusNormal;
}

// Prior dedicated 3146 pin (same formula; residual suite identity).
auto pinShouldResetTempBox3146(const uint8 status) -> bool
{
    return status == salvagehelpers::kStatusNormal;
}

} // namespace

// Pure dual-wire expansion for salvagehelpers::ShouldResetTempBox
// (Lua resetTempBoxes status == NORMAL gate; OmegaXI internal/salvage;
// slice 3442 dedicated dual-wire expand residual 2898).
//
// Coverage:
//   - NORMAL (0) → reset true (positive pin form)
//   - DISAPPEAR / other statuses → reset false
//   - free == inline == pin == status==kStatusNormal residual pins
//   - dense poles: 0, 1, 2, kStatusNormal, kStatusDisappear, 99, 255
//   - residual 1083 / 2898 / 3146 / 3398 pins still hold
auto runSalvageResetTempBox3442SelfTests() -> bool
{
    using salvagehelpers::ShouldResetTempBox;
    using salvagehelpers::kStatusDisappear;
    using salvagehelpers::kStatusNormal;

    bool ok = true;

    // Status catalog pins (match Go StatusNormal / StatusDisappear).
    ok = expect(kStatusNormal == 0, "StatusNormal pin") && ok;
    ok = expect(kStatusDisappear == 2, "StatusDisappear pin") && ok;

    // Residual 1083 / 2898 / 3146 / 3398 ShouldResetTempBox pins still hold under dual-wire.
    ok = expect(ShouldResetTempBox(kStatusNormal), "residual: NORMAL should reset") && ok;
    ok = expect(!ShouldResetTempBox(kStatusDisappear), "residual: DISAPPEAR should not reset") && ok;
    ok = expect(!ShouldResetTempBox(1), "residual: status 1 should not reset") && ok;
    ok = expect(ShouldResetTempBox(0), "residual: literal NORMAL 0 should reset") && ok;
    ok = expect(ShouldResetTempBox(kStatusNormal), "residual 2898: StatusNormal pin should reset") && ok;
    ok = expect(!ShouldResetTempBox(kStatusDisappear), "residual 2898: StatusDisappear pin should not reset") && ok;
    ok = expect(ShouldResetTempBox(kStatusNormal) == pinShouldResetTempBox3146(kStatusNormal),
                "residual 3146: free == pin for StatusNormal") &&
         ok;
    ok = expect(ShouldResetTempBox(kStatusDisappear) == pinShouldResetTempBox3146(kStatusDisappear),
                "residual 3146: free == pin for StatusDisappear") &&
         ok;
    ok = expect(ShouldResetTempBox(kStatusNormal) == pinShouldResetTempBox3398(kStatusNormal),
                "residual 3398: free == pin for StatusNormal") &&
         ok;
    ok = expect(ShouldResetTempBox(kStatusDisappear) == pinShouldResetTempBox3398(kStatusDisappear),
                "residual 3398: free == pin for StatusDisappear") &&
         ok;

    // --- Eligible reset path (positive pin form) ---
    ok = expect(ShouldResetTempBox(kStatusNormal), "eligible StatusNormal should reset") && ok;
    ok = expect(ShouldResetTempBox(0), "eligible literal NORMAL 0 should reset") && ok;
    ok = expect(pinShouldResetTempBox(kStatusNormal), "eligible pin StatusNormal should reset") && ok;
    ok = expect(pinShouldResetTempBox(0), "eligible pin literal 0 should reset") && ok;

    // --- Blocked paths ---
    ok = expect(!ShouldResetTempBox(kStatusDisappear), "DISAPPEAR should block reset") && ok;
    ok = expect(!ShouldResetTempBox(1), "status 1 should block reset") && ok;
    ok = expect(!ShouldResetTempBox(2), "literal DISAPPEAR 2 should block reset") && ok;
    ok = expect(!ShouldResetTempBox(99), "status 99 should block reset") && ok;
    ok = expect(!ShouldResetTempBox(255), "status 255 should block reset") && ok;

    // --- Composition table: free == inline == pin residual pins == compose ---
    // Dense status poles: 0, 1, 2, kStatusNormal, kStatusDisappear, 99, 255.
    const struct
    {
        uint8       status;
        bool        want;
        const char* label;
    } cases[] = {
        { 0, true, "table literal NORMAL 0" },
        { kStatusNormal, true, "table StatusNormal pin" },
        { 1, false, "table status 1" },
        { 2, false, "table literal DISAPPEAR 2" },
        { kStatusDisappear, false, "table StatusDisappear pin" },
        { 99, false, "table status 99" },
        { 255, false, "table status max uint8" },
    };

    for (const auto& c : cases)
    {
        const bool got           = ShouldResetTempBox(c.status);
        const bool inlineC       = inlineShouldResetTempBox(c.status);
        const bool pinGot        = pinShouldResetTempBox(c.status);
        const bool pinPrior3398  = pinShouldResetTempBox3398(c.status);
        const bool pinPrior3146  = pinShouldResetTempBox3146(c.status);
        const bool compose       = c.status == kStatusNormal;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineC, "dual-wire free==inline") && ok;
        ok = expect(got == pinGot, "dual-wire free==pin") && ok;
        ok = expect(got == pinPrior3398, "dual-wire free==pinPrior3398") && ok;
        ok = expect(got == pinPrior3146, "dual-wire free==pinPrior3146") && ok;
        ok = expect(got == compose, "formula free==status==kStatusNormal") && ok;
    }

    // --- Production resetTempBoxes path semantics ---
    // Eligible → may continue DISAPPEAR / resetLocalVars / setAnimationSub(8).
    // Blocked → leave casket untouched.
    ok = expect(ShouldResetTempBox(kStatusNormal), "resetTempBoxes eligible → continue path") && ok;
    ok = expect(!ShouldResetTempBox(kStatusDisappear), "resetTempBoxes DISAPPEAR → blocked") && ok;
    ok = expect(!ShouldResetTempBox(1), "resetTempBoxes other status → blocked") && ok;

    // Dense compose identity over required poles.
    const uint8 composeStatuses[] = { 0, 1, 2, kStatusNormal, kStatusDisappear, 99, 255 };
    for (const uint8 status : composeStatuses)
    {
        const bool got = ShouldResetTempBox(status);
        const bool want = status == kStatusNormal;
        ok = expect(got == want, "compose status==kStatusNormal") && ok;
        ok = expect(got == inlineShouldResetTempBox(status), "compose inline") && ok;
        ok = expect(got == pinShouldResetTempBox(status), "compose pin") && ok;
        ok = expect(got == pinShouldResetTempBox3398(status), "compose pinPrior3398") && ok;
        ok = expect(got == pinShouldResetTempBox3146(status), "compose pinPrior3146") && ok;
    }

    // Compose identity: free function is the gate; host injects status only.
    ok = expect(ShouldResetTempBox(kStatusNormal) == pinShouldResetTempBox(kStatusNormal),
                "formula free == pin StatusNormal") &&
         ok;
    ok = expect(ShouldResetTempBox(kStatusNormal) == inlineShouldResetTempBox(kStatusNormal),
                "formula free == inline StatusNormal") &&
         ok;
    ok = expect(!ShouldResetTempBox(kStatusDisappear), "formula DISAPPEAR path must block") && ok;
    ok = expect(ShouldResetTempBox(kStatusDisappear) == pinShouldResetTempBox(kStatusDisappear),
                "formula DISAPPEAR free == pin") &&
         ok;
    ok = expect(ShouldResetTempBox(kStatusDisappear) == pinShouldResetTempBox3398(kStatusDisappear),
                "formula DISAPPEAR free == residual 3398 pin") &&
         ok;
    ok = expect(ShouldResetTempBox(kStatusDisappear) == pinShouldResetTempBox3146(kStatusDisappear),
                "formula DISAPPEAR free == residual 3146 pin") &&
         ok;

    return ok;
}
