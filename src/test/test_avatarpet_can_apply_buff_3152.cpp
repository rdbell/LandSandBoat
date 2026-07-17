#include "test_avatarpet_can_apply_buff_3152.h"

#include "map/avatarpet_capacity.h"

#include <cstdint>
#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "avatarpet CanApplyBuff 3152 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline tryBuffSpell canApplyBuff formula for dual-wire cross-check
// (slice 3152; residual 2968):
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

// Positive if/else pin matching free function / capacity body (slice 3152).
// Avoid De Morgan rewrites of the compound-not form.
auto pinCanApplyBuff(
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

} // namespace

// Pure dual-wire expansion for avatarpethelpers::CanApplyBuff
// (tryBuffSpell canApplyBuff tier overwrite inject; OmegaXI internal/avatarpet;
// slice 3152).
//
// Coverage:
//   - no status → apply true
//   - un-tiered active (Haste/Regen) → apply false
//   - tier overwrite / equal / lower → higher only
//   - free == inline == pin (positive if/else)
//   - residual 1043 / 2968 pins still hold
auto runAvatarpetCanApplyBuff3152SelfTests() -> bool
{
    using avatarpethelpers::CanApplyBuff;

    bool ok = true;

    // Residual 1043 / 2968 pins still hold under dual-wire.
    ok = expect(CanApplyBuff(false, 0, false, 0), "residual no status + no tier applies") && ok;
    ok = expect(CanApplyBuff(false, 99, true, 1), "residual no status + tiered applies") && ok;
    ok = expect(!CanApplyBuff(true, 0, false, 0), "residual status + untiered rejects") && ok;
    ok = expect(CanApplyBuff(true, 2, true, 3), "residual tier 2 < 3 applies") && ok;
    ok = expect(!CanApplyBuff(true, 3, true, 3), "residual tier 3 < 3 rejects") && ok;
    ok = expect(!CanApplyBuff(true, 4, true, 3), "residual tier 4 < 3 rejects") && ok;

    // --- Dense poles: no-status / un-tiered active / overwrite / equal / lower ---
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
        { false, 0, false, 0, true, "no-status untiered applies" },
        { false, 0, true, 1, true, "no-status tiered applies" },
        { false, 255, true, 5, true, "no-status high statusTier applies" },
        { false, 0, true, 0, true, "no-status spellTier 0 applies" },
        { false, 99, true, 1, true, "no-status residual tiered applies" },

        // Status present, untiered spell (Haste/Regen): never reapply.
        { true, 0, false, 0, false, "un-tiered active never reapply" },
        { true, 5, false, 99, false, "un-tiered active ignores spellTier" },
        { true, 1, false, 1, false, "un-tiered active Haste rejects" },

        // Status present, tiered: only higher spell tier overwrites.
        { true, 1, true, 2, true, "tier overwrite 1 < 2" },
        { true, 2, true, 3, true, "tier overwrite 2 < 3" },
        { true, 3, true, 5, true, "tier overwrite 3 < 5 Protect V" },
        { true, 0, true, 1, true, "tier overwrite 0 < 1" },
        { true, 254, true, 255, true, "tier overwrite 254 < 255" },

        // Equal tier: no overwrite.
        { true, 1, true, 1, false, "equal tier 1 rejects" },
        { true, 3, true, 3, false, "equal tier 3 rejects" },
        { true, 5, true, 5, false, "equal tier 5 Protect V rejects" },
        { true, 0, true, 0, false, "equal tier 0 rejects" },
        { true, 255, true, 255, false, "equal tier 255 rejects" },

        // Lower spell tier: no overwrite.
        { true, 2, true, 1, false, "lower tier 2 < 1 rejects" },
        { true, 5, true, 3, false, "lower tier 5 < 3 rejects" },
        { true, 4, true, 3, false, "lower tier 4 < 3 rejects" },
        { true, 255, true, 0, false, "lower tier 255 < 0 rejects" },
    };

    for (const auto& c : cases)
    {
        const bool got     = CanApplyBuff(c.hasStatusEffect, c.statusTier, c.spellHasTier, c.spellTier);
        const bool inlineF = inlineCanApplyBuff(c.hasStatusEffect, c.statusTier, c.spellHasTier, c.spellTier);
        const bool pinGot  = pinCanApplyBuff(c.hasStatusEffect, c.statusTier, c.spellHasTier, c.spellTier);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "CanApplyBuff dual-wire == inline LSB formula") && ok;
        ok = expect(got == pinGot, "CanApplyBuff == pin formula (positive if/else)") && ok;
    }

    // Explicit free-path poles matching capacity body.
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
                    ok              = expect(got == want, "compose free == pin (positive if/else)") && ok;
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
        const bool freeGot = CanApplyBuff(h.hasStatusEffect, h.statusTier, h.spellHasTier, h.spellTier);
        const bool pinGot  = pinCanApplyBuff(h.hasStatusEffect, h.statusTier, h.spellHasTier, h.spellTier);
        const bool inlineF = inlineCanApplyBuff(h.hasStatusEffect, h.statusTier, h.spellHasTier, h.spellTier);
        ok                 = expect(freeGot == h.want, "host inject want pin") && ok;
        ok                 = expect(freeGot == pinGot, "host inject free == pin") && ok;
        ok                 = expect(freeGot == inlineF, "host inject free == inline") && ok;
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
