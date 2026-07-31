#include "test_mob_party_link_6250.h"

#include "map/mob_party_link_policy.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "mob party link 6250 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pins CZoneEntities::FindPartyForMob's party-link selection before it adds to
// an existing party or creates a new CParty for the source mob.
auto runMobPartyLink6250SelfTests() -> bool
{
    using mobpartylinkhelpers::MatchesPartyLink;
    using mobpartylinkhelpers::ShouldAttemptPartyLink;
    using mobpartylinkhelpers::ShouldConsiderPartyLinkCandidate;

    bool ok = true;

    ok = expect(!ShouldAttemptPartyLink(false, false, 0, false), "no link policy skips lookup") && ok;
    ok = expect(ShouldAttemptPartyLink(true, false, 0, false), "force link admits lookup") && ok;
    ok = expect(ShouldAttemptPartyLink(false, true, 0, false), "family link admits lookup") && ok;
    ok = expect(ShouldAttemptPartyLink(false, false, 7, false), "sublink admits lookup") && ok;
    ok = expect(!ShouldAttemptPartyLink(true, true, 7, true), "existing source party skips lookup") && ok;

    ok = expect(!ShouldConsiderPartyLinkCandidate(false, 0, false), "unlinked candidate skipped") && ok;
    ok = expect(ShouldConsiderPartyLinkCandidate(true, 0, false), "force link considers every candidate") && ok;
    ok = expect(ShouldConsiderPartyLinkCandidate(false, 9, false), "sublink considers every candidate") && ok;
    ok = expect(ShouldConsiderPartyLinkCandidate(false, 0, true), "candidate family link considered") && ok;

    // SUPERLINK has precedence over force link and family/sublink matching.
    ok = expect(MatchesPartyLink(4, true, 7, 10, 4, false, false, 99, 0), "matching superlink wins") && ok;
    ok = expect(!MatchesPartyLink(4, true, 7, 10, 3, true, true, 10, 7), "different superlink rejects despite other matches") && ok;

    // Force-link match applies only after the source has no SUPERLINK.
    ok = expect(MatchesPartyLink(0, true, 7, 10, 0, true, false, 99, 0), "matching forced link") && ok;
    ok = expect(!MatchesPartyLink(0, true, 7, 10, 0, false, true, 10, 7), "force link rejects non-forced candidate") && ok;

    // Normal matching permits a linked family match or matching non-zero sublink.
    ok = expect(MatchesPartyLink(0, false, 7, 10, 0, false, true, 10, 0), "linked family match") && ok;
    ok = expect(MatchesPartyLink(0, false, 7, 10, 0, false, false, 99, 7), "sublink match") && ok;
    ok = expect(!MatchesPartyLink(0, false, 7, 10, 0, false, true, 99, 8), "normal mismatch rejects") && ok;

    // Exhaustive small-domain pins keep the three pure policies aligned with
    // their production formulas, including zero/non-zero link boundaries.
    for (const bool forceLink : { false, true })
    {
        for (const bool familyLink : { false, true })
        {
            for (const int16_t sublink : { int16_t(-1), int16_t(0), int16_t(1) })
            {
                for (const bool hasParty : { false, true })
                {
                    const bool want = (forceLink || familyLink || sublink != 0) && !hasParty;
                    ok              = expect(ShouldAttemptPartyLink(forceLink, familyLink, sublink, hasParty) == want,
                                             "ShouldAttemptPartyLink exhaustive formula") &&
                                      ok;
                }
            }
        }
    }

    for (const bool forceLink : { false, true })
    {
        for (const int16_t sourceSublink : { int16_t(-1), int16_t(0), int16_t(1) })
        {
            for (const bool candidateFamilyLink : { false, true })
            {
                const bool want = forceLink || sourceSublink != 0 || candidateFamilyLink;
                ok              = expect(ShouldConsiderPartyLinkCandidate(forceLink, sourceSublink, candidateFamilyLink) == want,
                                         "ShouldConsiderPartyLinkCandidate exhaustive formula") &&
                                  ok;
            }
        }
    }

    for (const int16_t sourceSuperlink : { int16_t(-1), int16_t(0), int16_t(1) })
    {
        for (const bool sourceForceLink : { false, true })
        {
            for (const int16_t sourceSublink : { int16_t(-1), int16_t(0), int16_t(1) })
            {
                for (const uint16_t sourceFamily : { uint16_t(10), uint16_t(11) })
                {
                    for (const int16_t candidateSuperlink : { int16_t(-1), int16_t(0), int16_t(1) })
                    {
                        for (const bool candidateForceLink : { false, true })
                        {
                            for (const bool candidateFamilyLink : { false, true })
                            {
                                for (const uint16_t candidateFamily : { uint16_t(10), uint16_t(11) })
                                {
                                    for (const int16_t candidateSublink : { int16_t(-1), int16_t(0), int16_t(1) })
                                    {
                                        const bool want = sourceSuperlink != 0 ? candidateSuperlink == sourceSuperlink : sourceForceLink ? candidateForceLink
                                                                                                                                         : (candidateFamilyLink && candidateFamily == sourceFamily) || (sourceSublink != 0 && sourceSublink == candidateSublink);
                                        ok              = expect(MatchesPartyLink(sourceSuperlink, sourceForceLink, sourceSublink, sourceFamily, candidateSuperlink, candidateForceLink, candidateFamilyLink, candidateFamily, candidateSublink) == want,
                                                                 "MatchesPartyLink exhaustive precedence formula") &&
                                                          ok;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return ok;
}
