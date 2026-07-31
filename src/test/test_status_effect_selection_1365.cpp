#include "test_status_effect_selection_1365.h"

#include "map/status_effect_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "status effect selection 1365 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runStatusEffectSelection1365SelfTests() -> bool
{
    using statuseffecthelpers::BardSongIDFirst;
    using statuseffecthelpers::BardSongIDLast;
    using statuseffecthelpers::CanApplyBardReplacingOldest;
    using statuseffecthelpers::CanApplyBardWithoutReplace;
    using statuseffecthelpers::HasRemovableCandidates;
    using statuseffecthelpers::IsBardSongID;
    using statuseffecthelpers::IsErasableCandidate;
    using statuseffecthelpers::IsEarlierSongExpiry;
    using statuseffecthelpers::IsFlagRemovableCandidate;
    using statuseffecthelpers::IsInInclusiveIDRange;
    using statuseffecthelpers::IsManeuverID;
    using statuseffecthelpers::IsOwnBardSong;
    using statuseffecthelpers::IsRuneID;
    using statuseffecthelpers::IsSameBardSongOverwrite;
    using statuseffecthelpers::IsTimedActiveCandidate;
    using statuseffecthelpers::IsWaltzableCandidate;
    using statuseffecthelpers::ManeuverIDFirst;
    using statuseffecthelpers::ManeuverIDLast;
    using statuseffecthelpers::PreferHigherRuneCount;
    using statuseffecthelpers::RuneIDFirst;
    using statuseffecthelpers::RuneIDLast;
    using statuseffecthelpers::ShouldAssignLowestFreeSlot;
    using statuseffecthelpers::ShouldCountActiveInRange;
    using statuseffecthelpers::ShouldRemoveAllInRange;

    bool ok = true;

    // Timed / erasable / waltz / flag removable
    ok = expect(IsTimedActiveCandidate(true, false), "timed active") && ok;
    ok = expect(!IsTimedActiveCandidate(false, false), "zero duration") && ok;
    ok = expect(!IsTimedActiveCandidate(true, true), "deleted timed") && ok;
    ok = expect(IsErasableCandidate(true, true, false), "erasable") && ok;
    ok = expect(!IsErasableCandidate(false, true, false), "not erasable") && ok;
    ok = expect(IsWaltzableCandidate(true, false, true, false), "waltzable") && ok;
    ok = expect(IsWaltzableCandidate(false, true, true, false), "waltz via erasable") && ok;
    ok = expect(!IsWaltzableCandidate(false, false, true, false), "not waltz") && ok;
    ok = expect(IsFlagRemovableCandidate(true, true, false), "dispel flag") && ok;
    ok = expect(!IsFlagRemovableCandidate(true, false, false), "dispel zero dur") && ok;
    ok = expect(HasRemovableCandidates(1), "has candidates") && ok;
    ok = expect(!HasRemovableCandidates(0), "no candidates") && ok;
    // The production count is std::size_t; preserve unsigned conversion for
    // a synthetic negative host value at the non-empty boundary.
    ok = expect(HasRemovableCandidates(-1), "signed wrap has candidates") && ok;

    // Ranges
    ok = expect(IsBardSongID(BardSongIDFirst) && IsBardSongID(BardSongIDLast), "bard ends") && ok;
    ok = expect(!IsBardSongID(BardSongIDFirst - 1) && !IsBardSongID(BardSongIDLast + 1), "bard out") && ok;
    ok = expect(IsRuneID(RuneIDFirst) && IsRuneID(RuneIDLast), "rune ends") && ok;
    ok = expect(IsManeuverID(ManeuverIDFirst) && IsManeuverID(ManeuverIDLast), "maneuver ends") && ok;
    ok = expect(IsInInclusiveIDRange(200, 192, 223), "inclusive mid") && ok;

    // Bard overwrite / own song / expiry / apply gates
    ok = expect(IsSameBardSongOverwrite(2, 196, 2, 196), "same song") && ok;
    ok = expect(!IsSameBardSongOverwrite(2, 196, 3, 196), "diff tier") && ok;
    ok = expect(!IsSameBardSongOverwrite(2, 196, 2, 197), "diff id") && ok;
    ok = expect(IsOwnBardSong(42, 42) && !IsOwnBardSong(42, 43), "own song") && ok;
    ok = expect(IsEarlierSongExpiry(100, 200) && !IsEarlierSongExpiry(200, 100), "earlier expiry") && ok;
    ok = expect(CanApplyBardWithoutReplace(1, 2) && !CanApplyBardWithoutReplace(2, 2), "apply free") && ok;
    ok = expect(ShouldAssignLowestFreeSlot(0) && !ShouldAssignLowestFreeSlot(3), "free slot") && ok;
    ok = expect(CanApplyBardReplacingOldest(true) && !CanApplyBardReplacingOldest(false), "replace oldest") && ok;

    // Range count / highest rune / remove-all
    ok = expect(ShouldCountActiveInRange(523, RuneIDFirst, RuneIDLast, false), "count rune") && ok;
    ok = expect(!ShouldCountActiveInRange(523, RuneIDFirst, RuneIDLast, true), "count deleted") && ok;
    ok = expect(PreferHigherRuneCount(false, 0, 1), "first rune") && ok;
    ok = expect(PreferHigherRuneCount(true, 1, 2), "higher rune") && ok;
    ok = expect(!PreferHigherRuneCount(true, 2, 2), "tie keeps current") && ok;
    ok = expect(!PreferHigherRuneCount(true, 3, 2), "lower loses") && ok;
    ok = expect(ShouldRemoveAllInRange(300, ManeuverIDFirst, ManeuverIDLast), "remove all in") && ok;
    ok = expect(!ShouldRemoveAllInRange(299, ManeuverIDFirst, ManeuverIDLast), "remove all out") && ok;

    return ok;
}
