#include "test_party_stamp_leader_created_2991.h"

#include "map/party_capacity.h"

#include <cstddef>
#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "party ShouldStampLeaderCreatedPartyTime 2991 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline CParty::AddMember leader-created-time stamp for dual-wire cross-check
// (slice 2991):
//   isPCEntity && memberCountAfterAdd > 1
auto inlineShouldStampLeaderCreatedPartyTime(const bool isPCEntity, const std::size_t memberCountAfterAdd) -> bool
{
    return isPCEntity && memberCountAfterAdd > 1;
}

} // namespace

// Pure dual-wire expansion for partyhelpers::ShouldStampLeaderCreatedPartyTime
// (CParty::AddMember TYPE_PC && members.size() > 1 gate; slice 2991).
auto runPartyStampLeaderCreated2991SelfTests() -> bool
{
    using partyhelpers::ShouldApplyPartyLevelSyncOnJoin;
    using partyhelpers::ShouldClearSeekingParty;
    using partyhelpers::ShouldRunPCAddPostProcess;
    using partyhelpers::ShouldStampLeaderCreatedPartyTime;

    bool ok = true;

    // Residual 1350 pins still hold under dual-wire.
    ok = expect(ShouldStampLeaderCreatedPartyTime(true, 2), "residual stamp at PC size 2") && ok;
    ok = expect(!ShouldStampLeaderCreatedPartyTime(true, 1), "residual no stamp at PC size 1") && ok;
    ok = expect(!ShouldStampLeaderCreatedPartyTime(false, 2), "residual no stamp mob size 2") && ok;

    const struct
    {
        bool        isPCEntity;
        std::size_t memberCountAfterAdd;
        bool        want;
        const char* label;
    } cases[] = {
        // Edges: memberCount 0, 1, 2 × isPC true/false.
        { true, 0, false, "PC size 0 no stamp" },
        { true, 1, false, "PC size 1 no stamp (solo)" },
        { true, 2, true, "PC size 2 stamps leader time" },
        { false, 0, false, "mob size 0 no stamp" },
        { false, 1, false, "mob size 1 no stamp" },
        { false, 2, false, "mob size 2 no stamp" },

        // Above boundary and residual polarity pins.
        { true, 3, true, "PC size 3 stamps" },
        { true, 6, true, "PC size 6 stamps" },
        { false, 6, false, "mob size 6 no stamp" },
        { true, 2, true, "residual stamp size 2" },
        { true, 1, false, "residual no stamp size 1" },
        { false, 2, false, "residual no stamp mob" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldStampLeaderCreatedPartyTime(c.isPCEntity, c.memberCountAfterAdd);
        const bool inlineF = inlineShouldStampLeaderCreatedPartyTime(c.isPCEntity, c.memberCountAfterAdd);
        const bool wantPin = c.isPCEntity && c.memberCountAfterAdd > 1;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldStampLeaderCreatedPartyTime dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldStampLeaderCreatedPartyTime == pin formula isPC && count > 1") && ok;
    }

    // Pin composition: both TYPE_PC and size > 1 required.
    ok = expect(ShouldStampLeaderCreatedPartyTime(true, 2), "PC size 2 must stamp") && ok;
    ok = expect(!ShouldStampLeaderCreatedPartyTime(true, 1), "PC size 1 must not stamp") && ok;
    ok = expect(!ShouldStampLeaderCreatedPartyTime(true, 0), "PC size 0 must not stamp") && ok;
    ok = expect(!ShouldStampLeaderCreatedPartyTime(false, 2), "mob size 2 must not stamp") && ok;

    // The production count is std::size_t; preserve unsigned conversion for
    // synthetic negative host values at the greater-than-one boundary.
    ok = expect(ShouldStampLeaderCreatedPartyTime(true, -1), "PC signed wrap must stamp") && ok;

    // Dense compose: isPC × memberCount edges free == inline == pin.
    // Edges: memberCount 0, 1, 2 (and a few above for coverage).
    for (const bool isPCEntity : { false, true })
    {
        for (const std::size_t memberCount : { std::size_t{ 0 }, std::size_t{ 1 }, std::size_t{ 2 },
                                               std::size_t{ 3 }, std::size_t{ 6 }, std::size_t{ 99 } })
        {
            const bool got  = ShouldStampLeaderCreatedPartyTime(isPCEntity, memberCount);
            const bool want = isPCEntity && memberCount > 1;
            ok              = expect(got == want, "compose free == pin formula") && ok;
            ok              = expect(got == inlineShouldStampLeaderCreatedPartyTime(isPCEntity, memberCount),
                        "compose free == inline") &&
                 ok;
        }
    }

    // --- Production CParty::AddMember path semantics ---
    // Host injects:
    //   isPCEntity          = PEntity->objtype == TYPE_PC
    //   memberCountAfterAdd = members.size() after emplace_back
    // when true  → PLeader->m_LeaderCreatedPartyTime = timer::now()
    // when false → leave leader created-party time unchanged
    ok = expect(ShouldStampLeaderCreatedPartyTime(true, 2), "AddMember PC size 2 → stamp path") && ok;
    ok = expect(!ShouldStampLeaderCreatedPartyTime(true, 1), "AddMember PC size 1 → no stamp path") && ok;
    ok = expect(!ShouldStampLeaderCreatedPartyTime(true, 0), "AddMember PC size 0 → no stamp path") && ok;
    ok = expect(!ShouldStampLeaderCreatedPartyTime(false, 2), "AddMember mob size 2 → no stamp path") && ok;
    ok = expect(!ShouldStampLeaderCreatedPartyTime(false, 0), "AddMember mob size 0 → no stamp path") && ok;

    // Explicit dual-wire: free function is isPCEntity && count > 1 of injects.
    for (const bool isPCEntity : { false, true })
    {
        for (const std::size_t memberCount : { std::size_t{ 0 }, std::size_t{ 1 }, std::size_t{ 2 } })
        {
            const bool want = isPCEntity && memberCount > 1;
            ok              = expect(ShouldStampLeaderCreatedPartyTime(isPCEntity, memberCount) == want,
                        "host inject identity") &&
                 ok;
            ok = expect(ShouldStampLeaderCreatedPartyTime(isPCEntity, memberCount) ==
                            inlineShouldStampLeaderCreatedPartyTime(isPCEntity, memberCount),
                        "host inject dual-wire free == inline") &&
                 ok;
        }
    }

    // Residual sibling AddMember post-process halves remain independent of the
    // stamp gate (1350 suite coexistence).
    ok = expect(ShouldRunPCAddPostProcess(true), "residual ShouldRunPCAddPostProcess true") && ok;
    ok = expect(!ShouldRunPCAddPostProcess(false), "residual ShouldRunPCAddPostProcess false") && ok;
    ok = expect(ShouldClearSeekingParty(true), "residual ShouldClearSeekingParty true") && ok;
    ok = expect(!ShouldClearSeekingParty(false), "residual ShouldClearSeekingParty false") && ok;
    ok = expect(ShouldApplyPartyLevelSyncOnJoin(true), "residual ShouldApplyPartyLevelSyncOnJoin true") && ok;
    ok = expect(!ShouldApplyPartyLevelSyncOnJoin(false), "residual ShouldApplyPartyLevelSyncOnJoin false") && ok;

    return ok;
}
