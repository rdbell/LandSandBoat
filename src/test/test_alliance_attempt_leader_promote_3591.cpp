#include "test_alliance_attempt_leader_promote_3591.h"

#include "map/alliance_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "alliance ShouldAttemptAllianceLeaderPromote 3591 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline removeParty promote-gate formula for dual-wire cross-check
// (slice 3591 dedicated dual-wire expand residual 2979):
//   isMainParty
auto inlineShouldAttemptAllianceLeaderPromote(const bool isMainParty) -> bool
{
    return isMainParty;
}

// Prior dedicated expand residual 2979 pin (slice 3547; formula UNCHANGED).
// free == inline == pin == pin3547.
auto pinShouldAttemptAllianceLeaderPromote3547(const bool isMainParty) -> bool
{
    return isMainParty;
}

// This slice's dual-wire pin (slice 3591; formula isMainParty identity —
// UNCHANGED). free == inline == pin == pin3547.
auto pinShouldAttemptAllianceLeaderPromote3591(const bool isMainParty) -> bool
{
    return isMainParty;
}

} // namespace

// Pure dual-wire expansion for alliancehelpers::ShouldAttemptAllianceLeaderPromote
// (isMainParty identity; slice 3591 dedicated dual-wire expand residual 2979).
//
// Coverage:
//   - main party → attempt promote true
//   - non-main → attempt promote false
//   - free == inline == pin == pin3547 (isMainParty identity)
//   - residual 1341 / 1346 / 2979 / 3144 / 3393 / 3447 / 3490 / 3547 pins still hold
//   - dense 2^1 compose + removeParty / assign residual integration
auto runAllianceAttemptLeaderPromote3591SelfTests() -> bool
{
    using alliancehelpers::ShouldAttemptAllianceLeaderPromote;

    bool ok = true;

    // Residual 1341 / prior dual-wire 2979 / prior expansion 3144 / prior
    // dedicated 3393 / prior dedicated 3447 / prior dedicated 3490 / prior
    // dedicated 3547 pins still hold.
    ok = expect(ShouldAttemptAllianceLeaderPromote(true), "residual: admit main party") && ok;
    ok = expect(!ShouldAttemptAllianceLeaderPromote(false), "residual: reject non-main") && ok;

    const struct
    {
        bool        isMainParty;
        bool        want;
        const char* label;
    } cases[] = {
        // Main party → attempt promote DB lookup
        { true, true, "main party attempts promote" },

        // Non-main party → skip promote
        { false, false, "non-main skips promote" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldAttemptAllianceLeaderPromote(c.isMainParty);
        const bool inlineF = inlineShouldAttemptAllianceLeaderPromote(c.isMainParty);
        const bool pin     = pinShouldAttemptAllianceLeaderPromote3591(c.isMainParty);
        const bool pin3547 = pinShouldAttemptAllianceLeaderPromote3547(c.isMainParty);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF && got == pin && got == pin3547,
                    "ShouldAttemptAllianceLeaderPromote free == inline == pin == pin3547") &&
             ok;
    }

    // Pin composition: identity of isMainParty.
    ok = expect(ShouldAttemptAllianceLeaderPromote(true), "main party must attempt") && ok;
    ok = expect(!ShouldAttemptAllianceLeaderPromote(false), "non-main must skip") && ok;

    // Dense compose: full 2^1 boolean space free == inline == pin == pin3547.
    for (const bool isMainParty : { false, true })
    {
        const bool got     = ShouldAttemptAllianceLeaderPromote(isMainParty);
        const bool inlineF = inlineShouldAttemptAllianceLeaderPromote(isMainParty);
        const bool pin     = pinShouldAttemptAllianceLeaderPromote3591(isMainParty);
        const bool pin3547 = pinShouldAttemptAllianceLeaderPromote3547(isMainParty);
        const bool want    = isMainParty;
        ok                 = expect(got == want, "compose free == pin formula") && ok;
        ok                 = expect(got == inlineF && got == pin && got == pin3547,
                    "compose free == inline == pin == pin3547") &&
             ok;
    }

    // --- Production removeParty path semantics ---
    // Host injects:
    //   isMainParty = !isNull && getMainParty() == party
    // true  → SELECT another PARTY_LEADER then assignAllianceLeader
    // false → skip promote; re-check stillMainAfterPromote + classify
    ok = expect(ShouldAttemptAllianceLeaderPromote(true), "removeParty main → promote DB path") && ok;
    ok = expect(!ShouldAttemptAllianceLeaderPromote(false), "removeParty non-main → skip promote") && ok;

    // Host inject identity: free function is the isMainParty identity;
    // free == inline == pin == pin3547.
    for (const bool isMainParty : { false, true })
    {
        const bool got     = ShouldAttemptAllianceLeaderPromote(isMainParty);
        const bool inlineF = inlineShouldAttemptAllianceLeaderPromote(isMainParty);
        const bool pin     = pinShouldAttemptAllianceLeaderPromote3591(isMainParty);
        const bool pin3547 = pinShouldAttemptAllianceLeaderPromote3547(isMainParty);
        ok                 = expect(got == isMainParty, "host inject identity") && ok;
        ok                 = expect(got == inlineF && got == pin && got == pin3547,
                    "host inject dual-wire free == inline == pin == pin3547") &&
             ok;
    }

    // removeParty plan residual stays orthogonal (promote gate only gates DB).
    ok = expect(alliancehelpers::ClassifyRemoveAllianceParty(false, true, true) ==
                    alliancehelpers::remove_alliance_party_plan::DISSOLVE_STILL_MAIN,
                "removeParty still-main plan residual") &&
         ok;
    ok = expect(alliancehelpers::ClassifyRemoveAllianceParty(false, true, false) ==
                    alliancehelpers::remove_alliance_party_plan::DEL_AND_NOTIFY,
                "removeParty main after promote residual") &&
         ok;
    ok = expect(alliancehelpers::ClassifyRemoveAllianceParty(false, false, true) ==
                    alliancehelpers::remove_alliance_party_plan::DEL_AND_NOTIFY,
                "removeParty non-main residual") &&
         ok;
    ok = expect(alliancehelpers::ClassifyRemoveAllianceParty(true, false, false) ==
                    alliancehelpers::remove_alliance_party_plan::NULL_PARTY,
                "removeParty null plan residual") &&
         ok;

    // assignAllianceLeader pure residual (1346).
    ok = expect(alliancehelpers::ClassifyAssignAllianceLeader(true, true) ==
                    alliancehelpers::assign_alliance_leader_gate::FOUND,
                "assignAllianceLeader found residual") &&
         ok;
    ok = expect(alliancehelpers::ClassifyAssignAllianceLeader(false, true) ==
                    alliancehelpers::assign_alliance_leader_gate::NOT_FOUND,
                "assignAllianceLeader query-fail residual") &&
         ok;
    ok = expect(alliancehelpers::ClassifyAssignAllianceLeader(true, false) ==
                    alliancehelpers::assign_alliance_leader_gate::NOT_FOUND,
                "assignAllianceLeader no-row residual") &&
         ok;
    ok = expect(alliancehelpers::FormatRemoveAlliancePartyNullWarning() ==
                    "CAlliance::removeParty - party is null!",
                "removeParty null warning residual") &&
         ok;

    return ok;
}
