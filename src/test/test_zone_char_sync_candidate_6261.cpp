#include "test_zone_char_sync_candidate_6261.h"

#include "map/zone_char_sync_candidate.h"

#include <iostream>

namespace
{
auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "zone character-sync candidate 6261 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

// Pins SpawnPCs' production-wired candidate eligibility and score admission.
auto runZoneCharSyncCandidate6261SelfTests() -> bool
{
    using namespace zonecharsynccandidate;

    bool ok = true;
    ok      = expect(HasEligibleIdentity(false, true), "visible same-Mog-House candidate is eligible") && ok;
    ok      = expect(!HasEligibleIdentity(true, true), "hidden GM candidate is excluded") && ok;
    ok      = expect(!HasEligibleIdentity(false, false), "different Mog House candidate is excluded") && ok;
    ok      = expect(IsInCandidateRange(true, true), "candidate needs both range gates") && ok;
    ok      = expect(!IsInCandidateRange(false, true), "horizontal-range failure excludes candidate") && ok;
    ok      = expect(!IsInCandidateRange(true, false), "vertical-range failure excludes candidate") && ok;

    ok = expect(ShouldAdmit(31, 32, true, 0.0f, -100.0f), "below cap admits candidate") && ok;
    ok = expect(ShouldAdmit(32, 32, false, 5.0f, 6.0f), "full list admits score above lowest swap") && ok;
    ok = expect(!ShouldAdmit(32, 32, false, 5.0f, 5.0f), "full list keeps score tie") && ok;
    ok = expect(!ShouldAdmit(32, 32, true, 0.0f, 100.0f), "full list without replaceable entry excludes candidate") && ok;
    ok = expect(!ShouldAdmit(-1, 32, true, 0.0f, 100.0f), "negative synthetic spawned count wraps above cap") && ok;
    return ok;
}
