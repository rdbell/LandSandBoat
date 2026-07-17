#include "test_alliance_attempt_leader_promote_2979.h"

#include "map/alliance_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "alliance ShouldAttemptAllianceLeaderPromote 2979 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline removeParty promote-gate formula for dual-wire cross-check (slice 2979):
//   isMainParty
auto inlineShouldAttemptAllianceLeaderPromote(const bool isMainParty) -> bool
{
    return isMainParty;
}

} // namespace

// Pure dual-wire expansion for alliancehelpers::ShouldAttemptAllianceLeaderPromote
// (isMainParty identity; slice 2979).
auto runAllianceAttemptLeaderPromote2979SelfTests() -> bool
{
    using alliancehelpers::ShouldAttemptAllianceLeaderPromote;

    bool ok = true;

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
        const bool wantPin = c.isMainParty;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldAttemptAllianceLeaderPromote dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldAttemptAllianceLeaderPromote == pin formula identity") && ok;
    }

    // Pin composition: identity of isMainParty.
    ok = expect(ShouldAttemptAllianceLeaderPromote(true), "main party must attempt") && ok;
    ok = expect(!ShouldAttemptAllianceLeaderPromote(false), "non-main must skip") && ok;

    // Dense compose: full 2^1 boolean space.
    for (const bool isMainParty : { false, true })
    {
        const bool got  = ShouldAttemptAllianceLeaderPromote(isMainParty);
        const bool want = isMainParty;
        ok              = expect(got == want, "compose free == pin formula") && ok;
        ok              = expect(got == inlineShouldAttemptAllianceLeaderPromote(isMainParty),
                    "compose free == inline") &&
             ok;
    }

    // --- Production removeParty path semantics ---
    // Host injects:
    //   isMainParty = !isNull && getMainParty() == party
    // true  → SELECT another PARTY_LEADER then assignAllianceLeader
    // false → skip promote; re-check stillMainAfterPromote + classify
    ok = expect(ShouldAttemptAllianceLeaderPromote(true), "removeParty main → promote DB path") && ok;
    ok = expect(!ShouldAttemptAllianceLeaderPromote(false), "removeParty non-main → skip promote") && ok;

    // Residual 1341 pins still hold under dual-wire.
    ok = expect(ShouldAttemptAllianceLeaderPromote(true), "residual promote main") && ok;
    ok = expect(!ShouldAttemptAllianceLeaderPromote(false), "residual promote non-main") && ok;

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

    // assignAllianceLeader pure residual (1346).
    ok = expect(alliancehelpers::ClassifyAssignAllianceLeader(true, true) ==
                    alliancehelpers::assign_alliance_leader_gate::FOUND,
                "assignAllianceLeader found residual") &&
         ok;
    ok = expect(alliancehelpers::ClassifyAssignAllianceLeader(false, true) ==
                    alliancehelpers::assign_alliance_leader_gate::NOT_FOUND,
                "assignAllianceLeader query-fail residual") &&
         ok;

    return ok;
}
