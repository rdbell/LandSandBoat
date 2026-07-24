#include "test_gambit_conditions_6636.h"

#include "map/gambits_capacity.h"

#include <iostream>

namespace
{
using gambitshelpers::AbilityOnCooldown;
using gambitshelpers::AugustDaybreakActive;
using gambitshelpers::AugustNoQuarter;
using gambitshelpers::IsAugustDaybreakSkill;
using gambitshelpers::IsAugustRegularSkill;
using gambitshelpers::ShouldClearDaybreakSkill;
using gambitshelpers::ShouldUseNoQuarter;
using gambitshelpers::ResonanceFromPower;
using gambitshelpers::ShouldReplaceSkillchain;
using gambitshelpers::CanUseUriel;
using gambitshelpers::IsOffTargetAggro;
using gambitshelpers::UrielAggroGate;
using gambitshelpers::UrielCooldownReady;
using gambitshelpers::UrielLongCooldownSeconds;
using gambitshelpers::UrielRangeYalms;
using gambitshelpers::UrielShortCooldownSeconds;
using gambitshelpers::BarEffectForElement;
using gambitshelpers::CanUseRunes;
using gambitshelpers::IsDarkSkillchain;
using gambitshelpers::IsEcosystem;
using gambitshelpers::IsLightSkillchain;
using gambitshelpers::LungeMagicBurstAvailable;
using gambitshelpers::MatchesSubAnimation;
using gambitshelpers::MinLungeSkillchainTier;
using gambitshelpers::NeedBarEffect;
using gambitshelpers::ScDarkness;
using gambitshelpers::ScDarknessII;
using gambitshelpers::ScLight;
using gambitshelpers::ScLightII;
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

auto CheckBarEffect() -> bool
{
    // Each of the six elements maps to its own bar effect.
    const uint16 expected[] = { 100, 101, 102, 103, 104, 105 };
    for (uint8 element = 1; element <= 6; ++element)
    {
        if (BarEffectForElement(element) != expected[element - 1])
        {
            return false;
        }
    }

    // Non-elemental spells have no bar effect.
    if (BarEffectForElement(0) != 0 || BarEffectForElement(7) != 0 || BarEffectForElement(8) != 0)
    {
        return false;
    }

    // Needed only while casting an elemental spell the owner is unprotected from.
    if (!NeedBarEffect(true, 1, false) || NeedBarEffect(true, 1, true))
    {
        return false;
    }
    if (NeedBarEffect(false, 1, false) || NeedBarEffect(true, 8, false))
    {
        return false;
    }

    return true;
}

auto CheckLunge() -> bool
{
    if (!IsLightSkillchain(ScLight) || !IsLightSkillchain(ScLightII))
    {
        return false;
    }
    if (!IsDarkSkillchain(ScDarkness) || !IsDarkSkillchain(ScDarknessII))
    {
        return false;
    }
    if (IsLightSkillchain(ScDarkness) || IsDarkSkillchain(ScLight))
    {
        return false;
    }

    // Needs a burstable chain, tier 3+, Light or Darkness, and a matching rune.
    if (!LungeMagicBurstAvailable(true, ScLight, MinLungeSkillchainTier, true))
    {
        return false;
    }
    if (LungeMagicBurstAvailable(false, ScLight, MinLungeSkillchainTier, true))
    {
        return false;
    }
    if (LungeMagicBurstAvailable(true, ScLight, MinLungeSkillchainTier - 1, true))
    {
        return false;
    }
    if (LungeMagicBurstAvailable(true, ScLight, MinLungeSkillchainTier, false))
    {
        return false;
    }
    // A lower-tier element never qualifies, nor does SC_NONE.
    if (LungeMagicBurstAvailable(true, 12, MinLungeSkillchainTier, true) ||
        LungeMagicBurstAvailable(true, 0, MinLungeSkillchainTier, true))
    {
        return false;
    }

    return true;
}

auto CheckSimpleGates() -> bool
{
    if (!IsEcosystem(3, 3) || IsEcosystem(3, 4))
    {
        return false;
    }
    if (!MatchesSubAnimation(7, 7) || MatchesSubAnimation(7, 8))
    {
        return false;
    }
    if (!AbilityOnCooldown(1) || AbilityOnCooldown(0))
    {
        return false;
    }

    return true;
}

auto CheckUriel() -> bool
{
    // Off-target aggro needs every one of: alive, in range, targeting the
    // master, not the master's current target, hostile.
    if (!IsOffTargetAggro(true, UrielRangeYalms, true, false, true))
    {
        return false;
    }
    if (IsOffTargetAggro(false, 1.0f, true, false, true) ||
        IsOffTargetAggro(true, UrielRangeYalms + 0.1f, true, false, true) ||
        IsOffTargetAggro(true, 1.0f, false, false, true) ||
        IsOffTargetAggro(true, 1.0f, true, true, true) ||
        IsOffTargetAggro(true, 1.0f, true, false, false))
    {
        return false;
    }

    // Outer gate: master under attack and Val not already holding it, or any
    // off-target aggro.
    if (!UrielAggroGate(true, false, false) || !UrielAggroGate(false, true, true))
    {
        return false;
    }
    if (UrielAggroGate(true, true, false) || UrielAggroGate(false, false, false))
    {
        return false;
    }

    // Cooldown depends on whether Val already holds enmity.
    if (!UrielCooldownReady(true, UrielLongCooldownSeconds) || UrielCooldownReady(true, UrielLongCooldownSeconds - 1))
    {
        return false;
    }
    if (!UrielCooldownReady(false, UrielShortCooldownSeconds) || UrielCooldownReady(false, UrielShortCooldownSeconds - 1))
    {
        return false;
    }

    // Upstream's third clause (offTargetAggro && longCooldown) is redundant:
    // exhaustively confirm the simplified form matches the original.
    for (int valEnmity = 0; valEnmity < 2; ++valEnmity)
    {
        for (int offTarget = 0; offTarget < 2; ++offTarget)
        {
            for (uint32 elapsed = 0; elapsed <= UrielLongCooldownSeconds + 2; ++elapsed)
            {
                const bool longCooldown  = elapsed >= UrielLongCooldownSeconds;
                const bool shortCooldown = elapsed >= UrielShortCooldownSeconds;
                const bool original      = (valEnmity && longCooldown) ||
                                      (!valEnmity && shortCooldown) ||
                                      (offTarget && longCooldown);

                if (UrielCooldownReady(valEnmity != 0, elapsed) != original)
                {
                    return false;
                }
            }
        }
    }

    // Full check: gated, in range, and off cooldown.
    if (!CanUseUriel(true, false, false, true, 1.0f, UrielLongCooldownSeconds))
    {
        return false;
    }
    if (CanUseUriel(false, false, false, true, 1.0f, UrielLongCooldownSeconds))
    {
        return false;
    }
    if (CanUseUriel(true, false, false, true, UrielRangeYalms + 0.1f, UrielLongCooldownSeconds))
    {
        return false;
    }
    if (CanUseUriel(true, false, false, true, 1.0f, UrielLongCooldownSeconds - 1))
    {
        return false;
    }

    return true;
}

auto CheckSkillSelection() -> bool
{
    // Power 0x321 unpacks to primary 1, secondary 2, tertiary 3.
    const auto resonance = ResonanceFromPower(0x321);
    if (resonance.primary != 1 || resonance.secondary != 2 || resonance.tertiary != 3)
    {
        return false;
    }

    // The tertiary nibble is unmasked, so a value above 12 bits bleeds through.
    if (ResonanceFromPower(0xF321).tertiary != 0xF3)
    {
        return false;
    }

    // A formable chain replaces an equal-or-worse best; SC_NONE never does.
    if (!ShouldReplaceSkillchain(5, 0) || !ShouldReplaceSkillchain(5, 5) || !ShouldReplaceSkillchain(6, 5))
    {
        return false;
    }
    if (ShouldReplaceSkillchain(0, 0) || ShouldReplaceSkillchain(4, 5))
    {
        return false;
    }

    return true;
}

auto CheckAugust() -> bool
{
    // The three weaponskill sets are disjoint.
    if (!IsAugustDaybreakSkill(3656) || !IsAugustDaybreakSkill(3657))
    {
        return false;
    }
    if (!IsAugustRegularSkill(3653) || !IsAugustRegularSkill(3654) || !IsAugustRegularSkill(3655))
    {
        return false;
    }
    if (IsAugustDaybreakSkill(AugustNoQuarter) || IsAugustRegularSkill(AugustNoQuarter))
    {
        return false;
    }
    if (IsAugustDaybreakSkill(3653) || IsAugustRegularSkill(3656))
    {
        return false;
    }

    // Daybreak is sub-animation 5.
    if (!AugustDaybreakActive(5) || AugustDaybreakActive(0) || AugustDaybreakActive(4))
    {
        return false;
    }

    // No Quarter needs Daybreak up AND a Daybreak opener as the last skill.
    if (!ShouldUseNoQuarter(true, 3656) || !ShouldUseNoQuarter(true, 3657))
    {
        return false;
    }
    if (ShouldUseNoQuarter(false, 3656) || ShouldUseNoQuarter(true, 0) || ShouldUseNoQuarter(true, 3653))
    {
        return false;
    }

    // The rotation resets only once Daybreak drops with a skill recorded.
    if (!ShouldClearDaybreakSkill(false, 3656))
    {
        return false;
    }
    if (ShouldClearDaybreakSkill(false, 0) || ShouldClearDaybreakSkill(true, 3656))
    {
        return false;
    }

    return true;
}

auto Check() -> bool
{
    return CheckRunes() && CheckTopEnmity() && CheckSkillchain() && CheckPartyRoles() &&
           CheckCasting() && CheckBarEffect() && CheckLunge() && CheckSimpleGates() &&
           CheckUriel() && CheckSkillSelection() && CheckAugust();
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
