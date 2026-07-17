#include "test_avatarpet_apply_buff_2968.h"

#include "map/avatarpet_capacity.h"

#include <cstdint>
#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "avatarpet CanApplyBuff 2968 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline tryBuffSpell canApplyBuff formula for dual-wire cross-check (slice 2968):
//   if !hasStatusEffect { return true }
//   if !spellHasTier { return false }
//   return statusTier < spellTier
//
// Lua:
//   not statusEffect or spellData.tier and statusEffect:getTier() < spellData.tier
auto inlineCanApplyBuff(
    const bool hasStatusEffect,
    const uint8 statusTier,
    const bool spellHasTier,
    const uint8 spellTier) -> bool
{
    if (!hasStatusEffect)
    {
        return true;
    }
    if (!spellHasTier)
    {
        return false;
    }
    return statusTier < spellTier;
}

// One-liner pin matching Lua operator precedence (and tighter than or).
auto pinCanApplyBuff(
    const bool hasStatusEffect,
    const uint8 statusTier,
    const bool spellHasTier,
    const uint8 spellTier) -> bool
{
    return !hasStatusEffect || (spellHasTier && statusTier < spellTier);
}

} // namespace

// Pure dual-wire expansion for avatarpethelpers::CanApplyBuff
// (tryBuffSpell canApplyBuff tier overwrite inject; slice 2968).
auto runAvatarpetApplyBuff2968SelfTests() -> bool
{
    using avatarpethelpers::CanApplyBuff;

    bool ok = true;

    // Residual 1043 pins still hold under dual-wire.
    ok = expect(CanApplyBuff(false, 0, false, 0), "residual no status + no tier applies") && ok;
    ok = expect(CanApplyBuff(false, 99, true, 1), "residual no status + tiered applies") && ok;
    ok = expect(!CanApplyBuff(true, 0, false, 0), "residual status + untiered rejects") && ok;
    ok = expect(CanApplyBuff(true, 2, true, 3), "residual tier 2 < 3 applies") && ok;
    ok = expect(!CanApplyBuff(true, 3, true, 3), "residual tier 3 < 3 rejects") && ok;
    ok = expect(!CanApplyBuff(true, 4, true, 3), "residual tier 4 < 3 rejects") && ok;

    const struct
    {
        bool        hasStatusEffect;
        uint8       statusTier;
        bool        spellHasTier;
        uint8       spellTier;
        bool        want;
        const char* label;
    } cases[] = {
        // No status → always apply (tier flags ignored).
        { false, 0, false, 0, true, "no status untiered applies" },
        { false, 0, true, 1, true, "no status tiered applies" },
        { false, 255, true, 1, true, "no status high statusTier applies" },
        { false, 0, true, 0, true, "no status spell tier 0 applies" },

        // Status present, untiered spell (Haste/Regen nil tier): never reapply.
        { true, 0, false, 0, false, "status + untiered never reapply" },
        { true, 5, false, 99, false, "status + untiered ignores spellTier" },
        { true, 0, false, 1, false, "status + untiered spellTier 1 still rejects" },

        // Status present, tiered: only higher spell tier overwrites.
        { true, 1, true, 2, true, "tier 1 < 2 overwrites" },
        { true, 2, true, 3, true, "tier 2 < 3 overwrites" },
        { true, 3, true, 4, true, "tier 3 < 4 overwrites" },
        { true, 4, true, 5, true, "tier 4 < 5 overwrites" },
        { true, 0, true, 1, true, "tier 0 < 1 overwrites" },

        // Equal tier: no overwrite.
        { true, 1, true, 1, false, "equal tier 1 rejects" },
        { true, 3, true, 3, false, "equal tier 3 rejects" },
        { true, 5, true, 5, false, "equal tier 5 rejects" },
        { true, 0, true, 0, false, "equal tier 0 rejects" },

        // Lower spell tier: no overwrite.
        { true, 2, true, 1, false, "tier 2 < 1 false rejects" },
        { true, 5, true, 3, false, "tier 5 < 3 false rejects" },
        { true, 255, true, 1, false, "status max < spell 1 false rejects" },

        // Boundary poles.
        { true, 254, true, 255, true, "254 < 255 overwrites" },
        { true, 255, true, 255, false, "255 < 255 equal rejects" },
        { true, 255, true, 0, false, "255 < 0 false rejects" },
        { true, 0, true, 255, true, "0 < 255 overwrites" },

        // Residual 1043 re-pins.
        { false, 0, false, 0, true, "residual no status no tier" },
        { false, 99, true, 1, true, "residual no status tiered" },
        { true, 0, false, 0, false, "residual status untiered" },
        { true, 2, true, 3, true, "residual tier 2 < 3" },
        { true, 3, true, 3, false, "residual tier 3 < 3" },
        { true, 4, true, 3, false, "residual tier 4 < 3" },
    };

    for (const auto& c : cases)
    {
        const bool got     = CanApplyBuff(c.hasStatusEffect, c.statusTier, c.spellHasTier, c.spellTier);
        const bool inlineF = inlineCanApplyBuff(c.hasStatusEffect, c.statusTier, c.spellHasTier, c.spellTier);
        const bool wantPin = pinCanApplyBuff(c.hasStatusEffect, c.statusTier, c.spellHasTier, c.spellTier);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "CanApplyBuff dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "CanApplyBuff == pin formula") && ok;
    }

    // Pin composition: free function is the four-arg inject only.
    ok = expect(CanApplyBuff(false, 0, false, 0), "no status must apply") && ok;
    ok = expect(!CanApplyBuff(true, 0, false, 0), "status + untiered must reject") && ok;
    ok = expect(CanApplyBuff(true, 1, true, 2), "higher tier must apply") && ok;
    ok = expect(!CanApplyBuff(true, 2, true, 2), "equal tier must reject") && ok;
    ok = expect(!CanApplyBuff(true, 3, true, 2), "lower spell tier must reject") && ok;

    // Dense compose over representative poles: free == pin == inline.
    const bool bools[]  = { false, true };
    const uint8 tiers[] = { 0, 1, 2, 3, 4, 5, 254, 255 };
    for (const bool hasStatus : bools)
    {
        for (const uint8 statusTier : tiers)
        {
            for (const bool spellHasTier : bools)
            {
                for (const uint8 spellTier : tiers)
                {
                    const bool got  = CanApplyBuff(hasStatus, statusTier, spellHasTier, spellTier);
                    const bool want = pinCanApplyBuff(hasStatus, statusTier, spellHasTier, spellTier);
                    ok              = expect(got == want, "compose free == pin formula") && ok;
                    ok              = expect(got == inlineCanApplyBuff(hasStatus, statusTier, spellHasTier, spellTier),
                                "compose free == inline") &&
                         ok;
                }
            }
        }
    }

    // Host-style inject poles: getStatusEffect / getTier / spellData.tier.
    // (Live tryBuffSpell party scan host remains residual 1043 / Lua.)
    const struct
    {
        bool  hasStatusEffect;
        uint8 statusTier;
        bool  spellHasTier;
        uint8 spellTier;
        bool  want;
    } hostPoles[] = {
        { false, 0, false, 0, true },
        { false, 0, true, 5, true },
        { true, 0, false, 0, false },
        { true, 2, true, 3, true },
        { true, 3, true, 3, false },
        { true, 4, true, 3, false },
        { true, 4, true, 5, true },
        { true, 5, true, 5, false },
    };
    for (const auto& h : hostPoles)
    {
        const bool inject = pinCanApplyBuff(h.hasStatusEffect, h.statusTier, h.spellHasTier, h.spellTier);
        ok                = expect(CanApplyBuff(h.hasStatusEffect, h.statusTier, h.spellHasTier, h.spellTier) == inject,
                    "host inject dual-wire identity") &&
             ok;
        ok = expect(CanApplyBuff(h.hasStatusEffect, h.statusTier, h.spellHasTier, h.spellTier) ==
                        inlineCanApplyBuff(h.hasStatusEffect, h.statusTier, h.spellHasTier, h.spellTier),
                    "host inject free == inline") &&
             ok;
        ok = expect(CanApplyBuff(h.hasStatusEffect, h.statusTier, h.spellHasTier, h.spellTier) == h.want,
                    "host inject want pin") &&
             ok;
    }

    // Production path semantics:
    //   CanApplyBuff(status~=nil, getTier(), tier~=nil, tier)
    // when true  → return spellData.spell, member (cast path)
    // when false → continue party/buff scan
    ok = expect(CanApplyBuff(false, 0, true, 5), "no status → cast path") && ok;
    ok = expect(!CanApplyBuff(true, 5, true, 5), "equal Protect V → continue scan") && ok;
    ok = expect(CanApplyBuff(true, 3, true, 5), "Protect V over III → cast path") && ok;
    ok = expect(!CanApplyBuff(true, 0, false, 0), "Haste active → continue scan") && ok;

    // Catalog-style tier poles (Protect/Shell 1..5, Haste/Regen untiered).
    ok = expect(CanApplyBuff(true, 3, true, 5), "Protect V over Protect III") && ok;
    ok = expect(!CanApplyBuff(true, 5, true, 5), "Protect V equal tier 5") && ok;
    ok = expect(CanApplyBuff(true, 3, true, 4), "Protect IV over Protect III") && ok;
    ok = expect(!CanApplyBuff(true, 4, true, 4), "Protect IV equal tier 4") && ok;
    ok = expect(!CanApplyBuff(true, 1, false, 0), "Haste while active rejects") && ok;
    ok = expect(!CanApplyBuff(true, 0, false, 0), "Regen while active rejects") && ok;
    ok = expect(CanApplyBuff(false, 0, false, 0), "Haste no status applies") && ok;
    ok = expect(CanApplyBuff(false, 0, true, 1), "Shell I no status applies") && ok;

    return ok;
}
