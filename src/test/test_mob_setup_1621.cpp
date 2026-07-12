#include "test_mob_setup_1621.h"

#include "map/mob_setup_capacity.h"

#include <iostream>

namespace
{
using namespace mobsetuphelpers;

auto HasMod(const std::vector<MobModEntry>& mods, const std::int16_t mod, const std::int16_t value, const MobModApplyKind kind = MobModApplyKind::Default)
    -> bool
{
    for (const auto& m : mods)
    {
        if (m.mod == mod && m.value == value && m.kind == kind)
        {
            return true;
        }
    }
    return false;
}

auto Check() -> bool
{
    if (ResolveSetupJob(JobWHM, 1, 1) != JobWHM || ResolveSetupJob(JobNIN, JobWHM, 0) != JobNIN || ResolveSetupJob(1, JobBLM, 0) != JobBLM)
    {
        return false;
    }

    {
        const auto mods = SetupJobMagicMods(JobBLM);
        if (mods.size() != 4 || !HasMod(mods, MobModMagicCool, 35) || !HasMod(mods, MobModGAChance, 40))
        {
            return false;
        }
    }
    {
        const auto mods = SetupJobMagicMods(JobSMN);
        if (mods.size() != 2 || !HasMod(mods, MobModMagicCool, 70) || !HasMod(mods, MobModBuffChance, 100))
        {
            return false;
        }
    }

    {
        const auto p = SetupJobMainMods(JobTHF, 0, true);
        if (p.mods.size() != 1 || !HasMod(p.mods, MobModGilBonus, 150))
        {
            return false;
        }
    }
    {
        const auto p = SetupJobMainMods(JobRNG, FamilyTrolls, false);
        if (p.mods.size() != 4 || !HasMod(p.mods, MobModSpecialSkill, 1747) || !HasMod(p.mods, MobModStandbackCool, 0) || p.callSetupRangedAttack)
        {
            return false;
        }
    }
    {
        const auto p = SetupJobMainMods(JobRNG, 1, false);
        if (p.mods.size() != 4 || !HasMod(p.mods, MobModSpecialSkill, 272) || !HasMod(p.mods, MobModStandbackCool, 6))
        {
            return false;
        }
    }
    {
        const auto p = SetupJobMainMods(JobNIN, FamilyFomor, false);
        if (!p.callSetupRangedAttack || !HasMod(p.mods, MobModDualWield, 1, MobModApplyKind::Force))
        {
            return false;
        }
    }
    {
        const auto p = SetupJobMainMods(JobNIN, FamilyMaat, false);
        if (p.mods.size() != 1 || !HasMod(p.mods, MobModHPStandback, 70))
        {
            return false;
        }
    }

    {
        const auto p = PlanSetupRoaming(false, 0);
        if (p.mods.size() != 4 || !HasMod(p.mods, MobModRoamDistance, 10) || !HasMod(p.mods, MobModRoamRate, 15))
        {
            return false;
        }
    }
    {
        const auto p = PlanSetupRoaming(true, RoamFlagAmbush);
        if (p.specialFlagsOR != SpecialFlagHidden || !p.setMaxRoam || p.maxRoamDistance != 2.0f)
        {
            return false;
        }
        if (!HasMod(p.mods, MobModRoamDistance, 20) || !HasMod(p.mods, MobModRoamDistance, 5, MobModApplyKind::Force))
        {
            return false;
        }
    }

    if (PetSkillListID(248) != 715 || PetSkillListID(243) != 721 || PetSkillListID(0) != 0)
    {
        return false;
    }

    // Combined: WAR/BLM → BLM magic cool
    {
        const auto p = BuildSetupJobPlan(1, JobBLM, 0, 0, false);
        if (!HasMod(p.mods, MobModMagicCool, 35) || !HasMod(p.mods, MobModGAChance, 40))
        {
            return false;
        }
    }

    return true;
}
} // namespace

auto runMobSetup1621SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "mob_setup_1621 self-tests failed\n";
        return false;
    }
    return true;
}
