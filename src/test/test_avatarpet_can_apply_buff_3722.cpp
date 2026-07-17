#include "test_avatarpet_can_apply_buff_3722.h"

#include "map/avatarpet_capacity.h"

#include <cstdint>
#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "avatarpet CanApplyBuff 3722 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline tryBuffSpell canApplyBuff formula for dual-wire cross-check
// (slice 3722; residual 2968 / prior dedicated 3152 / 3230 / 3370 / 3425 /
// 3479 / 3543 / 3587 / 3632 / 3677):
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

// Positive if/else pin matching free function / capacity body (slice 3722).
// Multi-branch / early-return form: positive if/else only (avoid De Morgan rewrites).
//   if !hasStatusEffect → true
//   if !spellHasTier → false
//   else → statusTier < spellTier
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

// Residual one-liner pin (2968 form).
auto pinCanApplyBuffOneLiner(
    const bool hasStatusEffect,
    const uint8 statusTier,
    const bool spellHasTier,
    const uint8 spellTier) -> bool
{
    return !hasStatusEffect || (spellHasTier && statusTier < spellTier);
}

// Prior dedicated 3677 independence pin (positive if/else; formula unchanged).
auto pinCanApplyBuff3677(
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

// Exact formula want for dual-wire cross-check.
auto formulaWant(
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
// dedicated slice 3722 expand residual 2968).
//
// Coverage:
//   - free == inline == pin == pin3677 (positive if/else multi-branch / early-return)
//   - residual 1043 / 2968 poles still hold
//   - residual pins (one-liner + prior dedicated forms incl. 3677) still agree
//   - dense tiers: 0, 1, 2, 255
//   - hasStatus × spellHasTier × tier compare cartesian
auto runAvatarpetCanApplyBuff3722SelfTests() -> bool
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

    // Residual poles: free == inline == pin == pin3677.
    const struct
    {
        bool        hasStatusEffect;
        uint8       statusTier;
        bool        spellHasTier;
        uint8       spellTier;
        bool        want;
        const char* label;
    } poles[] = {
        { false, 0, false, 0, true, "residual no status no tier" },
        { false, 99, true, 1, true, "residual no status tiered" },
        { true, 0, false, 0, false, "residual status untiered" },
        { true, 2, true, 3, true, "residual tier 2 < 3" },
        { true, 3, true, 3, false, "residual tier 3 < 3" },
        { true, 4, true, 3, false, "residual tier 4 < 3" },
        { false, 0, true, 5, true, "no status cast path" },
        { true, 5, true, 5, false, "equal Protect V continue scan" },
        { true, 3, true, 5, true, "Protect V over III cast path" },
        { true, 0, false, 0, false, "Haste active continue scan" },
    };
    for (const auto& p : poles)
    {
        const bool got     = CanApplyBuff(p.hasStatusEffect, p.statusTier, p.spellHasTier, p.spellTier);
        const bool inlineF = inlineCanApplyBuff(p.hasStatusEffect, p.statusTier, p.spellHasTier, p.spellTier);
        const bool pinGot  = pinCanApplyBuff(p.hasStatusEffect, p.statusTier, p.spellHasTier, p.spellTier);
        const bool pin3677 = pinCanApplyBuff3677(p.hasStatusEffect, p.statusTier, p.spellHasTier, p.spellTier);
        ok                 = expect(got == p.want, p.label) && ok;
        ok                 = expect(got == inlineF, "residual pole free == inline") && ok;
        ok                 = expect(got == pinGot, "residual pole free == pin") && ok;
        ok                 = expect(got == pin3677, "residual pole free == pin3677") && ok;
    }

    // --- Dense poles: no-status / un-tiered / overwrite / equal / lower ---
    // Required dense tiers: 0, 1, 2, 255.
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
        { false, 255, true, 2, true, "no-status high statusTier applies" },
        { false, 0, true, 0, true, "no-status spellTier 0 applies" },
        { false, 99, true, 1, true, "no-status residual tiered applies" },
        { false, 1, false, 255, true, "no-status ignores spellHasTier false" },
        { false, 2, true, 255, true, "no-status dense spellTier 255" },

        // Status present, untiered spell (Haste/Regen): never reapply.
        { true, 0, false, 0, false, "un-tiered active never reapply" },
        { true, 5, false, 99, false, "un-tiered active ignores spellTier" },
        { true, 1, false, 1, false, "un-tiered active Haste rejects" },
        { true, 2, false, 255, false, "un-tiered active dense 255 ignores" },
        { true, 255, false, 0, false, "un-tiered active status 255 rejects" },

        // Status present, tiered: only higher spell tier overwrites.
        { true, 0, true, 1, true, "tier overwrite 0 < 1" },
        { true, 1, true, 2, true, "tier overwrite 1 < 2" },
        { true, 2, true, 255, true, "tier overwrite 2 < 255" },
        { true, 0, true, 2, true, "tier overwrite 0 < 2" },
        { true, 1, true, 255, true, "tier overwrite 1 < 255" },
        { true, 0, true, 255, true, "tier overwrite 0 < 255" },

        // Equal tier: no overwrite (dense 0, 1, 2, 255).
        { true, 0, true, 0, false, "equal tier 0 rejects" },
        { true, 1, true, 1, false, "equal tier 1 rejects" },
        { true, 2, true, 2, false, "equal tier 2 rejects" },
        { true, 255, true, 255, false, "equal tier 255 rejects" },

        // Lower spell tier: no overwrite.
        { true, 1, true, 0, false, "lower tier 1 < 0 rejects" },
        { true, 2, true, 1, false, "lower tier 2 < 1 rejects" },
        { true, 2, true, 0, false, "lower tier 2 < 0 rejects" },
        { true, 255, true, 2, false, "lower tier 255 < 2 rejects" },
        { true, 255, true, 1, false, "lower tier 255 < 1 rejects" },
        { true, 255, true, 0, false, "lower tier 255 < 0 rejects" },
    };

    for (const auto& c : cases)
    {
        const bool got     = CanApplyBuff(c.hasStatusEffect, c.statusTier, c.spellHasTier, c.spellTier);
        const bool inlineF = inlineCanApplyBuff(c.hasStatusEffect, c.statusTier, c.spellHasTier, c.spellTier);
        const bool pinGot  = pinCanApplyBuff(c.hasStatusEffect, c.statusTier, c.spellHasTier, c.spellTier);
        const bool pin3677 = pinCanApplyBuff3677(c.hasStatusEffect, c.statusTier, c.spellHasTier, c.spellTier);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "CanApplyBuff dual-wire == inline LSB formula") && ok;
        ok = expect(got == pinGot, "CanApplyBuff == pin formula (positive if/else)") && ok;
        ok = expect(got == pin3677, "CanApplyBuff == pin3677 (prior dedicated)") && ok;
    }

    // Dense cartesian: hasStatus × spellHasTier × tiers{0,1,2,255}
    // free == inline == pin == pin3677 == formula (+ residual pins).
    const bool bools[]  = { false, true };
    const uint8 tiers[] = { 0, 1, 2, 255 };
    for (const bool hasStatus : bools)
    {
        for (const uint8 statusTier : tiers)
        {
            for (const bool spellHasTier : bools)
            {
                for (const uint8 spellTier : tiers)
                {
                    const bool got  = CanApplyBuff(hasStatus, statusTier, spellHasTier, spellTier);
                    const bool want = formulaWant(hasStatus, statusTier, spellHasTier, spellTier);
                    ok              = expect(got == want, "dense free == formula") && ok;
                    ok              = expect(got == pinCanApplyBuff(hasStatus, statusTier, spellHasTier, spellTier),
                                "dense free == pin (positive if/else)") &&
                         ok;
                    ok = expect(got == inlineCanApplyBuff(hasStatus, statusTier, spellHasTier, spellTier),
                                "dense free == inline") &&
                         ok;
                    ok = expect(got == pinCanApplyBuffOneLiner(hasStatus, statusTier, spellHasTier, spellTier),
                                "dense free == residual one-liner pin") &&
                         ok;
                    ok = expect(got == pinCanApplyBuff3677(hasStatus, statusTier, spellHasTier, spellTier),
                                "dense free == residual 3677 pin") &&
                         ok;
                }
            }
        }
    }

    // Explicit free-path poles matching capacity body.
    ok = expect(CanApplyBuff(false, 0, false, 0), "no status must apply") && ok;
    ok = expect(!CanApplyBuff(true, 0, false, 0), "status + untiered must reject") && ok;
    ok = expect(CanApplyBuff(true, 1, true, 2), "higher tier must apply") && ok;
    ok = expect(!CanApplyBuff(true, 2, true, 2), "equal tier must reject") && ok;
    ok = expect(!CanApplyBuff(true, 255, true, 2), "lower spell tier must reject") && ok;
    ok = expect(CanApplyBuff(true, 2, true, 255), "dense 2 < 255 must apply") && ok;

    // Host-style inject poles: free == pin == pin3677 == inline.
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
        { true, 0, true, 1, true },
        { true, 1, true, 2, true },
        { true, 2, true, 255, true },
        { true, 255, true, 255, false },
        { true, 255, true, 0, false },
    };
    for (const auto& h : hostPoles)
    {
        const bool freeGot = CanApplyBuff(h.hasStatusEffect, h.statusTier, h.spellHasTier, h.spellTier);
        const bool pinGot  = pinCanApplyBuff(h.hasStatusEffect, h.statusTier, h.spellHasTier, h.spellTier);
        const bool pin3677 = pinCanApplyBuff3677(h.hasStatusEffect, h.statusTier, h.spellHasTier, h.spellTier);
        const bool inlineF = inlineCanApplyBuff(h.hasStatusEffect, h.statusTier, h.spellHasTier, h.spellTier);
        ok                 = expect(freeGot == h.want, "host inject want pin") && ok;
        ok                 = expect(freeGot == pinGot, "host inject free == pin") && ok;
        ok                 = expect(freeGot == pin3677, "host inject free == pin3677") && ok;
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
    // Residual pins only — CanApplyBuffSpell compose sibling left alone.
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
