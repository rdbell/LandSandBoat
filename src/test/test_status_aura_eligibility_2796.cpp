#include "test_status_aura_eligibility_2796.h"

#include "map/status_effect_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "status aura eligibility 2796 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runStatusAuraEligibility2796SelfTests() -> bool
{
    using statuseffecthelpers::IsInAuraRange;
    using statuseffecthelpers::IsSameZoneForAura;
    using statuseffecthelpers::IsWithinAuraRange;
    using statuseffecthelpers::ShouldAcceptAuraAlly;
    using statuseffecthelpers::ShouldRejectNullAuraMember;

    bool ok = true;

    // 1) null member reject
    ok = expect(ShouldRejectNullAuraMember(true) && !ShouldRejectNullAuraMember(false), "null reject") && ok;

    // 2) same zone: both present and equal IDs
    ok = expect(IsSameZoneForAura(100, 100, true, true), "same zone") && ok;
    ok = expect(!IsSameZoneForAura(100, 101, true, true), "diff zone id") && ok;
    ok = expect(!IsSameZoneForAura(100, 100, false, true), "owner zone absent") && ok;
    ok = expect(!IsSameZoneForAura(100, 100, true, false), "member zone absent") && ok;
    ok = expect(!IsSameZoneForAura(0, 0, false, false), "both absent") && ok;
    // equal zero IDs still accepted when both present (host injects 0 as real zone)
    ok = expect(IsSameZoneForAura(0, 0, true, true), "zone id 0 present") && ok;

    // 3) in range: distance <= auraRange + modelHitboxSize
    ok = expect(IsInAuraRange(6.5f, 6.0f, 1.0f), "in range boundary interior") && ok;
    ok = expect(IsInAuraRange(7.0f, 6.0f, 1.0f), "in range exact") && ok;
    ok = expect(!IsInAuraRange(7.1f, 6.0f, 1.0f), "out of range") && ok;
    ok = expect(IsInAuraRange(0.0f, 6.0f, 0.0f), "zero distance") && ok;
    // parity with slice 1366 name
    ok = expect(IsInAuraRange(6.5f, 6.0f, 1.0f) == IsWithinAuraRange(6.5f, 6.0f, 1.0f), "alias 1366") && ok;

    // 4) ShouldAcceptAuraAlly composition
    ok = expect(ShouldAcceptAuraAlly(false, true, true, false), "accept healthy ally") && ok;
    ok = expect(!ShouldAcceptAuraAlly(true, true, true, false), "reject null") && ok;
    ok = expect(!ShouldAcceptAuraAlly(false, false, true, false), "reject other zone") && ok;
    ok = expect(!ShouldAcceptAuraAlly(false, true, false, false), "reject out of range") && ok;
    ok = expect(!ShouldAcceptAuraAlly(false, true, true, true), "reject dead") && ok;
    // null short-circuits even when other flags look good
    ok = expect(!ShouldAcceptAuraAlly(true, true, true, true), "null wins over dead") && ok;
    // dead + out of zone still rejected
    ok = expect(!ShouldAcceptAuraAlly(false, false, false, true), "all fail") && ok;

    // Compose: accept iff !rejectNull && sameZone && inRange && !dead
    const struct
    {
        bool        memberNull;
        bool        sameZone;
        bool        inRange;
        bool        isDead;
        bool        want;
        const char* label;
    } cases[] = {
        { false, true, true, false, true, "compose accept" },
        { true, true, true, false, false, "compose null" },
        { false, false, true, false, false, "compose zone" },
        { false, true, false, false, false, "compose range" },
        { false, true, true, true, false, "compose dead" },
        { false, true, true, false, true, "compose accept 2" },
    };
    for (const auto& c : cases)
    {
        const bool rejectNull = ShouldRejectNullAuraMember(c.memberNull);
        const bool got        = ShouldAcceptAuraAlly(c.memberNull, c.sameZone, c.inRange, c.isDead);
        const bool composed   = !rejectNull && c.sameZone && c.inRange && !c.isDead;
        ok                    = expect(got == c.want && got == composed, c.label) && ok;
    }

    return ok;
}
