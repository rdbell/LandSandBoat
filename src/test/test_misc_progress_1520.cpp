#include "test_misc_progress_1520.h"

#include "map/misc_progress_capacity.h"

#include <iostream>

namespace
{
using miscprogresshelpers::ArrowsUsedAmmoID;
using miscprogresshelpers::ArrowsUsedCount;
using miscprogresshelpers::AvailableTraverserStones;
using miscprogresshelpers::EncodeArrowsUsed;
using miscprogresshelpers::HasChargeAdd;
using miscprogresshelpers::IncrementArrowsUsed;
using miscprogresshelpers::IsSameArrowAsLast;
using miscprogresshelpers::IsTraverserEpochUnset;
using miscprogresshelpers::PairedBloodPactRecast;
using miscprogresshelpers::RecastBloodPactRage;
using miscprogresshelpers::RecastBloodPactWard;
using miscprogresshelpers::RecastInnin;
using miscprogresshelpers::ShouldAddWeaponSkillPoints;
using miscprogresshelpers::ShouldIncrementArrowsUsed;
using miscprogresshelpers::ShouldInitArrowsUsed;
using miscprogresshelpers::ShouldMirrorYoninToInnin;
using miscprogresshelpers::ShouldRebuildAfterWSPoints;
using miscprogresshelpers::ShouldShareBloodPactTimer;
using miscprogresshelpers::TraverserStonesGenerated;
using miscprogresshelpers::TraverserWaitHours;

auto Check() -> bool
{
    if (!ShouldAddWeaponSkillPoints(true, true, false) || ShouldAddWeaponSkillPoints(true, true, true) ||
        ShouldAddWeaponSkillPoints(false, true, false) || ShouldAddWeaponSkillPoints(true, false, false))
    {
        return false;
    }
    if (!ShouldRebuildAfterWSPoints(true) || ShouldRebuildAfterWSPoints(false))
    {
        return false;
    }
    if (!HasChargeAdd(true) || HasChargeAdd(false))
    {
        return false;
    }
    if (!ShouldShareBloodPactTimer(true, RecastBloodPactRage) || !ShouldShareBloodPactTimer(true, RecastBloodPactWard) ||
        ShouldShareBloodPactTimer(false, RecastBloodPactRage) || ShouldShareBloodPactTimer(true, 100))
    {
        return false;
    }
    if (PairedBloodPactRecast(RecastBloodPactRage) != RecastBloodPactWard ||
        PairedBloodPactRecast(RecastBloodPactWard) != RecastBloodPactRage)
    {
        return false;
    }
    if (!ShouldMirrorYoninToInnin(146) || ShouldMirrorYoninToInnin(147) || RecastInnin != 147)
    {
        return false;
    }
    if (EncodeArrowsUsed(12) != 120001 || ArrowsUsedAmmoID(120005) != 12 || ArrowsUsedCount(120005) != 5)
    {
        return false;
    }
    if (!ShouldInitArrowsUsed(0) || ShouldInitArrowsUsed(1))
    {
        return false;
    }
    if (!IsSameArrowAsLast(120001, 12) || IsSameArrowAsLast(120001, 13))
    {
        return false;
    }
    if (!ShouldIncrementArrowsUsed(120001) || !ShouldIncrementArrowsUsed(10000 + 1979) || ShouldIncrementArrowsUsed(10000 + 1980))
    {
        return false;
    }
    if (IncrementArrowsUsed(5) != 6)
    {
        return false;
    }
    if (!IsTraverserEpochUnset(0) || IsTraverserEpochUnset(1))
    {
        return false;
    }
    if (TraverserWaitHours(0) != 20 || TraverserWaitHours(1) != 16 || TraverserWaitHours(3) != 8 || TraverserWaitHours(5) != 8)
    {
        return false;
    }
    if (TraverserStonesGenerated(40, 20) != 2 || TraverserStonesGenerated(19, 20) != 0 || TraverserStonesGenerated(10, 0) != 0)
    {
        return false;
    }
    if (AvailableTraverserStones(5, 2) != 3 || AvailableTraverserStones(2, 2) != 0)
    {
        return false;
    }
    // Unsigned wrap when claimed > generated (LSB parity).
    if (AvailableTraverserStones(2, 5) != static_cast<std::uint32_t>(2u - 5u))
    {
        return false;
    }
    return true;
}
} // namespace

auto runMiscProgress1520SelfTests() -> bool
{
    const bool ok = Check();
    if (!ok)
    {
        std::cerr << "misc progress 1520 self-test failed\n";
    }
    return ok;
}
