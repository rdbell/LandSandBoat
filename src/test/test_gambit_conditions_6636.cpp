#include "test_gambit_conditions_6636.h"

#include "map/gambits_capacity.h"

#include <iostream>

namespace
{
using gambitshelpers::CanUseRunes;
using gambitshelpers::CastingDebuff;
using gambitshelpers::CastingElementalAoe;
using gambitshelpers::CastingElementalMagic;
using gambitshelpers::CastingElementalOnSelf;
using gambitshelpers::ElementFire;
using gambitshelpers::ElementWater;
using gambitshelpers::IsElementalMagic;
using gambitshelpers::SpellAoeRadial;
using gambitshelpers::HasTopEnmity;
using gambitshelpers::IsHealerJob;
using gambitshelpers::IsTankJob;
using gambitshelpers::JobNin;
using gambitshelpers::JobPld;
using gambitshelpers::JobRdm;
using gambitshelpers::JobRun;
using gambitshelpers::JobSch;
using gambitshelpers::JobWhm;
using gambitshelpers::MagicBurstAvailable;
using gambitshelpers::MaxRuneEffects;
using gambitshelpers::MaxRuneEffects;
using gambitshelpers::NoSkillchain;
using gambitshelpers::NotHasTopEnmity;
using gambitshelpers::RuneJobRun;
using gambitshelpers::SkillchainAvailable;
using gambitshelpers::SkillchainReady;
using gambitshelpers::SkillchainReadyDelayMs;

auto CheckRunes() -> bool
{
    // Only a main Runemaster gains extra runes, at 35 and again at 65.
    if (MaxRuneEffects(RuneJobRun, 1) != 1 || MaxRuneEffects(RuneJobRun, 34) != 1)
    {
        return false;
    }
    if (MaxRuneEffects(RuneJobRun, 35) != 2 || MaxRuneEffects(RuneJobRun, 64) != 2)
    {
        return false;
    }
    if (MaxRuneEffects(RuneJobRun, 65) != 3 || MaxRuneEffects(RuneJobRun, 99) != 3)
    {
        return false;
    }

    // A non-RUN main job is capped at one rune regardless of level.
    if (MaxRuneEffects(1, 99) != 1 || MaxRuneEffects(0, 75) != 1)
    {
        return false;
    }

    if (!CanUseRunes(0, 1) || CanUseRunes(1, 1) || CanUseRunes(2, 1))
    {
        return false;
    }
    if (!CanUseRunes(2, 3) || CanUseRunes(3, 3))
    {
        return false;
    }

    return true;
}

auto CheckTopEnmity() -> bool
{
    // Owner holds top enmity.
    if (!HasTopEnmity(true, 7, 7) || NotHasTopEnmity(true, 7, 7))
    {
        return false;
    }

    // Someone else holds it.
    if (HasTopEnmity(true, 8, 7) || !NotHasTopEnmity(true, 8, 7))
    {
        return false;
    }

    // Nobody holds it: both are false, so NotHasTopEnmity is not a negation.
    if (HasTopEnmity(false, 0, 7) || NotHasTopEnmity(false, 0, 7))
    {
        return false;
    }

    return true;
}

auto CheckSkillchain() -> bool
{
    // The age gate is strict, so exactly the delay is not yet ready.
    if (SkillchainReady(true, SkillchainReadyDelayMs))
    {
        return false;
    }
    if (!SkillchainReady(true, SkillchainReadyDelayMs + 1))
    {
        return false;
    }
    if (SkillchainReady(false, SkillchainReadyDelayMs + 1000))
    {
        return false;
    }

    const uint32 ready = SkillchainReadyDelayMs + 1;

    // Tier 0 is an open skillchain; above 0 is closed and burstable.
    if (!SkillchainAvailable(true, ready, 0) || SkillchainAvailable(true, ready, 1))
    {
        return false;
    }
    if (!MagicBurstAvailable(true, ready, 1) || MagicBurstAvailable(true, ready, 0))
    {
        return false;
    }

    // Too fresh for either.
    if (SkillchainAvailable(true, 0, 0) || MagicBurstAvailable(true, 0, 1))
    {
        return false;
    }

    // NoSkillchain ignores age and tier, so it is not the negation of
    // SkillchainAvailable: a fresh chain makes both false.
    if (!NoSkillchain(false) || NoSkillchain(true))
    {
        return false;
    }
    if (SkillchainAvailable(true, 0, 0) || NoSkillchain(true))
    {
        return false;
    }

    return true;
}

auto CheckPartyRoles() -> bool
{
    // Healers: WHM, RDM, PLD, SCH.
    if (!IsHealerJob(JobWhm) || !IsHealerJob(JobRdm) || !IsHealerJob(JobPld) || !IsHealerJob(JobSch))
    {
        return false;
    }
    if (IsHealerJob(JobNin) || IsHealerJob(JobRun) || IsHealerJob(1))
    {
        return false;
    }

    // Tanks: NIN, PLD, RUN.
    if (!IsTankJob(JobNin) || !IsTankJob(JobPld) || !IsTankJob(JobRun))
    {
        return false;
    }
    if (IsTankJob(JobWhm) || IsTankJob(JobRdm) || IsTankJob(JobSch) || IsTankJob(1))
    {
        return false;
    }

    // Paladin is the only job counted as both.
    if (!IsHealerJob(JobPld) || !IsTankJob(JobPld))
    {
        return false;
    }

    return true;
}

auto CheckCasting() -> bool
{
    // Fire..Water are elemental; None, Light, and Dark are not.
    for (uint8 element = ElementFire; element <= ElementWater; ++element)
    {
        if (!IsElementalMagic(element))
        {
            return false;
        }
    }
    if (IsElementalMagic(0) || IsElementalMagic(7) || IsElementalMagic(8))
    {
        return false;
    }

    // Every casting condition first requires the magic state.
    if (CastingDebuff(false, true) || CastingElementalMagic(false, ElementFire))
    {
        return false;
    }
    if (CastingElementalAoe(false, ElementFire, SpellAoeRadial) || CastingElementalOnSelf(false, ElementFire, 7, 7))
    {
        return false;
    }

    if (!CastingDebuff(true, true) || CastingDebuff(true, false))
    {
        return false;
    }
    if (!CastingElementalMagic(true, ElementWater) || CastingElementalMagic(true, 8))
    {
        return false;
    }

    // Radial area of effect only.
    if (!CastingElementalAoe(true, ElementFire, SpellAoeRadial) || CastingElementalAoe(true, ElementFire, 0))
    {
        return false;
    }
    // A non-elemental radial spell is still rejected.
    if (CastingElementalAoe(true, 8, SpellAoeRadial))
    {
        return false;
    }

    // Aimed at the owner only.
    if (!CastingElementalOnSelf(true, ElementFire, 7, 7) || CastingElementalOnSelf(true, ElementFire, 8, 7))
    {
        return false;
    }
    if (CastingElementalOnSelf(true, 8, 7, 7))
    {
        return false;
    }

    return true;
}

auto Check() -> bool
{
    return CheckRunes() && CheckTopEnmity() && CheckSkillchain() && CheckPartyRoles() && CheckCasting();
}
} // namespace

auto runGambitConditions6636SelfTests() -> bool
{
    const bool ok = Check();
    if (!ok)
    {
        std::cerr << "gambit conditions 6636 self-test failed\n";
    }
    return ok;
}
