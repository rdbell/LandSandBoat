#pragma once

#include <cstdint>
#include <vector>

// Pure SetupJob / SetupRoaming / SetupPetSkills policy from mobutils.
// Parity: internal/mobutils/setup_policy.go (slice 1621).

namespace mobsetuphelpers
{

// MOBMODIFIER pins used by setup policy.
constexpr std::int16_t MobModBuffChance        = 6;
constexpr std::int16_t MobModGAChance          = 7;
constexpr std::int16_t MobModSevereSpellChance = 13;
constexpr std::int16_t MobModSkillList         = 14;
constexpr std::int16_t MobModSpecialSkill      = 30;
constexpr std::int16_t MobModRoamDistance      = 31;
constexpr std::int16_t MobModSpecialCool       = 33;
constexpr std::int16_t MobModMagicCool         = 34;
constexpr std::int16_t MobModStandbackCool     = 35;
constexpr std::int16_t MobModRoamCool          = 36;
constexpr std::int16_t MobModDualWield         = 44;
constexpr std::int16_t MobModRoamResetFacing   = 50;
constexpr std::int16_t MobModRoamTurns         = 51;
constexpr std::int16_t MobModRoamRate          = 52;
constexpr std::int16_t MobModGilBonus          = 54;
constexpr std::int16_t MobModHPStandback       = 56;
constexpr std::int16_t MobModMagicDelay        = 57;

// JOBTYPE pins.
constexpr std::uint8_t JobNIN = 13;
constexpr std::uint8_t JobTHF = 6;
constexpr std::uint8_t JobWHM = 3;
constexpr std::uint8_t JobBLM = 4;
constexpr std::uint8_t JobRDM = 5;
constexpr std::uint8_t JobPLD = 7;
constexpr std::uint8_t JobDRK = 8;
constexpr std::uint8_t JobBST = 9;
constexpr std::uint8_t JobBRD = 10;
constexpr std::uint8_t JobRNG = 11;
constexpr std::uint8_t JobSMN = 15;
constexpr std::uint8_t JobBLU = 16;
constexpr std::uint8_t JobPUP = 18;
constexpr std::uint8_t JobSCH = 20;
constexpr std::uint8_t JobGEO = 21;
constexpr std::uint8_t JobRUN = 22;

// Family IDs.
constexpr std::uint16_t FamilyGigas  = 57;
constexpr std::uint16_t FamilyTrolls = 72;
constexpr std::uint16_t FamilyAern   = 131;
constexpr std::uint16_t FamilyQuadav = 67;
constexpr std::uint16_t FamilyDemon  = 88;
constexpr std::uint16_t FamilyFomor  = 172;
constexpr std::uint16_t FamilyMaat   = 119;

// Roam / special flags.
constexpr std::uint16_t RoamFlagAmbush    = 0x80;
constexpr std::uint16_t RoamFlagScripted  = 0x100;
constexpr std::uint8_t  SpecialFlagHidden = 0x1;

enum class MobModApplyKind : std::uint8_t
{
    Default = 0, // defaultMobMod
    Force   = 1, // setMobMod
};

struct MobModEntry
{
    std::int16_t    mod   = 0;
    std::int16_t    value = 0;
    MobModApplyKind kind  = MobModApplyKind::Default;
};

struct SetupJobPlan
{
    std::vector<MobModEntry> mods;
    bool                     callSetupRangedAttack = false;
};

struct SetupRoamingPlan
{
    std::vector<MobModEntry> mods;
    std::uint8_t             specialFlagsOR  = 0;
    float                    maxRoamDistance = 0.0f;
    bool                     setMaxRoam      = false;
};

inline auto ResolveSetupJob(const std::uint8_t mJob, const std::uint8_t sJob, const std::uint8_t mainJobMPGrade) -> std::uint8_t
{
    if (mainJobMPGrade > 0 || mJob == JobNIN)
    {
        return mJob;
    }
    return sJob;
}

inline void PushDefault(std::vector<MobModEntry>& mods, const std::int16_t mod, const std::int16_t value)
{
    mods.push_back(MobModEntry{ mod, value, MobModApplyKind::Default });
}

inline void PushForce(std::vector<MobModEntry>& mods, const std::int16_t mod, const std::int16_t value)
{
    mods.push_back(MobModEntry{ mod, value, MobModApplyKind::Force });
}

inline auto SetupJobMagicMods(const std::uint8_t job) -> std::vector<MobModEntry>
{
    std::vector<MobModEntry> mods;
    switch (job)
    {
        case JobBLM:
            PushDefault(mods, MobModMagicCool, 35);
            PushDefault(mods, MobModGAChance, 40);
            PushDefault(mods, MobModBuffChance, 15);
            PushDefault(mods, MobModSevereSpellChance, 20);
            break;
        case JobPLD:
            PushDefault(mods, MobModMagicCool, 35);
            PushDefault(mods, MobModMagicDelay, 7);
            break;
        case JobDRK:
            PushDefault(mods, MobModMagicCool, 35);
            PushDefault(mods, MobModMagicDelay, 7);
            break;
        case JobWHM:
            PushDefault(mods, MobModMagicCool, 35);
            PushDefault(mods, MobModMagicDelay, 10);
            break;
        case JobBRD:
            PushDefault(mods, MobModMagicCool, 35);
            PushDefault(mods, MobModGAChance, 25);
            PushDefault(mods, MobModBuffChance, 60);
            PushDefault(mods, MobModMagicDelay, 10);
            break;
        case JobRDM:
            PushDefault(mods, MobModMagicCool, 35);
            PushDefault(mods, MobModGAChance, 15);
            PushDefault(mods, MobModBuffChance, 40);
            PushDefault(mods, MobModMagicDelay, 10);
            break;
        case JobSMN:
            PushDefault(mods, MobModMagicCool, 70);
            PushDefault(mods, MobModBuffChance, 100);
            break;
        case JobNIN:
            PushDefault(mods, MobModSpecialCool, 9);
            PushDefault(mods, MobModMagicCool, 35);
            PushDefault(mods, MobModBuffChance, 20);
            PushDefault(mods, MobModMagicDelay, 7);
            break;
        case JobBLU:
        case JobSCH:
        case JobGEO:
        case JobRUN:
            PushDefault(mods, MobModMagicCool, 35);
            break;
        default:
            break;
    }
    return mods;
}

inline auto SetupJobMainMods(const std::uint8_t mJob, const std::uint16_t family, const bool isBeastmen) -> SetupJobPlan
{
    SetupJobPlan plan;
    switch (mJob)
    {
        case JobTHF:
            if (isBeastmen)
            {
                PushDefault(plan.mods, MobModGilBonus, 150);
            }
            break;
        case JobRNG:
            switch (family)
            {
                case FamilyGigas:
                    PushDefault(plan.mods, MobModSpecialSkill, 658);
                    break;
                case FamilyTrolls:
                    PushDefault(plan.mods, MobModSpecialSkill, 1747);
                    PushDefault(plan.mods, MobModStandbackCool, 0);
                    PushDefault(plan.mods, MobModSpecialCool, 14);
                    PushDefault(plan.mods, MobModHPStandback, 70);
                    return plan; // early break
                case FamilyAern:
                    PushDefault(plan.mods, MobModSpecialSkill, 1388);
                    break;
                case FamilyQuadav:
                    PushDefault(plan.mods, MobModSpecialSkill, 1123);
                    break;
                case FamilyDemon:
                    PushDefault(plan.mods, MobModSpecialSkill, 1152);
                    break;
                case FamilyFomor:
                    plan.callSetupRangedAttack = true;
                    break;
                default:
                    PushDefault(plan.mods, MobModSpecialSkill, 272);
                    break;
            }
            PushDefault(plan.mods, MobModStandbackCool, 6);
            PushDefault(plan.mods, MobModSpecialCool, 12);
            PushDefault(plan.mods, MobModHPStandback, 70);
            break;
        case JobNIN:
            switch (family)
            {
                case FamilyAern:
                    PushDefault(plan.mods, MobModSpecialSkill, 1388);
                    PushDefault(plan.mods, MobModSpecialCool, 12);
                    break;
                case FamilyQuadav:
                    PushDefault(plan.mods, MobModSpecialSkill, 1123);
                    break;
                case FamilyDemon:
                    PushDefault(plan.mods, MobModSpecialSkill, 1152);
                    break;
                case FamilyFomor:
                    PushForce(plan.mods, MobModDualWield, 1);
                    plan.callSetupRangedAttack = true;
                    break;
                case FamilyMaat:
                    break;
                default:
                    PushDefault(plan.mods, MobModSpecialSkill, 272);
                    PushDefault(plan.mods, MobModSpecialCool, 12);
                    break;
            }
            PushDefault(plan.mods, MobModHPStandback, 70);
            break;
        case JobBST:
            PushDefault(plan.mods, MobModSpecialCool, 70);
            PushDefault(plan.mods, MobModSpecialSkill, 1017);
            break;
        case JobPUP:
            PushDefault(plan.mods, MobModSpecialSkill, 1901);
            PushDefault(plan.mods, MobModSpecialCool, 720);
            break;
        case JobBLM:
            PushDefault(plan.mods, MobModStandbackCool, 12);
            PushDefault(plan.mods, MobModHPStandback, 70);
            break;
        default:
            break;
    }
    return plan;
}

inline auto BuildSetupJobPlan(const std::uint8_t mJob, const std::uint8_t sJob, const std::uint8_t mainJobMPGrade, const std::uint16_t family,
                              const bool isBeastmen) -> SetupJobPlan
{
    const auto magicJob = ResolveSetupJob(mJob, sJob, mainJobMPGrade);
    auto       magic    = SetupJobMagicMods(magicJob);
    auto       main     = SetupJobMainMods(mJob, family, isBeastmen);
    main.mods.insert(main.mods.begin(), magic.begin(), magic.end());
    return main;
}

inline auto PlanSetupRoaming(const bool isBeastmen, const std::uint16_t roamFlags) -> SetupRoamingPlan
{
    std::int16_t distance = 10;
    std::int16_t turns    = 1;
    std::int16_t cool     = 20;
    std::int16_t rate     = 15;
    if (isBeastmen)
    {
        distance = 20;
        turns    = 5;
        cool     = 45;
    }
    SetupRoamingPlan plan;
    PushDefault(plan.mods, MobModRoamDistance, distance);
    PushDefault(plan.mods, MobModRoamTurns, turns);
    PushDefault(plan.mods, MobModRoamCool, cool);
    PushDefault(plan.mods, MobModRoamRate, rate);
    if ((roamFlags & RoamFlagAmbush) != 0)
    {
        plan.specialFlagsOR  = SpecialFlagHidden;
        plan.maxRoamDistance = 2.0f;
        plan.setMaxRoam      = true;
        PushForce(plan.mods, MobModRoamDistance, 5);
        PushForce(plan.mods, MobModRoamTurns, 1);
    }
    if ((roamFlags & RoamFlagScripted) != 0)
    {
        PushForce(plan.mods, MobModRoamResetFacing, 1);
    }
    return plan;
}

inline auto PetSkillListID(const std::uint16_t species) -> std::int16_t
{
    switch (species)
    {
        case 248:
            return 715;
        case 255:
            return 716;
        case 249:
            return 717;
        case 247:
            return 718;
        case 253:
            return 719;
        case 252:
            return 720;
        case 243:
            return 721;
        default:
            return 0;
    }
}

} // namespace mobsetuphelpers
