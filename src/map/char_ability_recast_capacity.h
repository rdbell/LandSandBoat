#pragma once

#include <algorithm>
#include <cstdint>

// Pure recast planning from CCharEntity::OnAbility (merit/charge/tabula/BP and
// post-paralysis Reward/Sic-Ready adjustments). Host injects resolved merit
// values, charge metadata, mods, and status presence.

namespace charabilityrecasthelpers
{

// Ability ID pins used by OnAbility recast branches.
constexpr std::uint16_t AbilitySic       = 72;
constexpr std::uint16_t AbilityReward    = 78;
constexpr std::uint16_t AbilityLightArts = 211;
constexpr std::uint16_t AbilityDarkArts  = 212;
constexpr std::uint16_t AbilityReady     = 251;

// Recast ID pins (enums/recast.h).
constexpr std::uint16_t RecastSic           = 102;
constexpr std::uint16_t RecastBloodPactRage = 173;
constexpr std::uint16_t RecastBloodPactWard = 174;
constexpr std::uint16_t RecastStrategems    = 231;

// ADDTYPE_MERIT bit.
constexpr std::uint16_t AddTypeMerit = 1;

// Reward head item IDs that subtract 10s from Reward recast.
inline auto IsRewardRecastHead(const std::uint16_t itemID) -> bool
{
    return itemID == 15157 || itemID == 15158 || itemID == 16104 || itemID == 16105;
}

struct InitialInput
{
    std::uint16_t abilityID{};
    std::uint16_t recastID{};
    std::uint16_t addType{};
    std::uint16_t meritModID{};
    std::int32_t  meritRecastReductionSec{}; // GetMeritValue when meritMod applies
    bool          hasCharge{};
    std::int64_t  chargeTimeSec{};           // charge->chargeTime as whole seconds
    std::int64_t  abilityRecastSec{};        // ability recast time / charge cost
    std::int32_t  sicMeritReductionSec{};    // MERIT_SIC_RECAST
    std::int32_t  strategemRecastModSec{};   // Mod::STRATAGEM_RECAST
    bool          hasTabulaRasa{};
    std::int16_t  bpDelayMod{};              // Mod::BP_DELAY
    std::int16_t  bpDelayIIMod{};            // Mod::BP_DELAY_II
    std::int16_t  avatarsFavorPower{};       // 0 when no status
};

struct InitialPlan
{
    std::int64_t  recastSec{};
    std::int64_t  baseChargeTimeSec{};
    bool          setBPRecastTime{};
    std::uint16_t bpRecastTime{};
};

// BuildInitial mirrors recast math from merit reduction through blood-pact delay
// snapshot, before paralysis and Reward/Sic-Ready adjustments.
constexpr auto BuildInitial(const InitialInput& in) -> InitialPlan
{
    InitialPlan plan{};

    std::int64_t recastReduction = 0;
    if (in.meritModID > 0 && (in.addType & AddTypeMerit) == 0)
    {
        recastReduction = in.meritRecastReductionSec;
    }

    if (in.hasCharge && in.abilityID != AbilitySic)
    {
        const auto chargesUsed = in.abilityRecastSec;
        if (in.recastID == RecastSic)
        {
            recastReduction = in.sicMeritReductionSec;
        }
        else if (in.recastID == RecastStrategems)
        {
            recastReduction += in.strategemRecastModSec;
        }
        plan.baseChargeTimeSec = in.chargeTimeSec - recastReduction;
        plan.recastSec         = plan.baseChargeTimeSec * chargesUsed;
    }
    else
    {
        plan.recastSec = in.abilityRecastSec - recastReduction;
    }

    if (in.abilityID == AbilityLightArts || in.abilityID == AbilityDarkArts || in.recastID == RecastStrategems)
    {
        if (in.hasTabulaRasa)
        {
            plan.recastSec         = 0;
            plan.baseChargeTimeSec = 0;
        }
    }
    else if (in.recastID == RecastBloodPactRage || in.recastID == RecastBloodPactWard)
    {
        const auto favorReduction = std::min<std::int16_t>(in.avatarsFavorPower, 10);
        const auto bloodPactI     = std::min<std::int16_t>(in.bpDelayMod, 15);
        const auto bloodPactII    = std::min<std::int16_t>(in.bpDelayIIMod, 15);
        const std::int16_t bloodPactIII = 0;
        const auto stacked        = std::min<std::int16_t>(
            static_cast<std::int16_t>(bloodPactI + bloodPactII + bloodPactIII), 30);
        const auto bloodPactDelayReduction = static_cast<std::int16_t>(favorReduction + stacked);
        const auto remaining = plan.recastSec - bloodPactDelayReduction;
        plan.setBPRecastTime = true;
        plan.bpRecastTime    = static_cast<std::uint16_t>(remaining > 0 ? remaining : 0);
        plan.recastSec       = 0;
    }

    return plan;
}

// AdjustPostParalysis mirrors Reward head -10s and Sic/Ready SIC_READY_RECAST
// reduction applied only on the non-paralyzed path.
constexpr auto AdjustPostParalysis(const std::int64_t recastSec,
                                   const std::uint16_t abilityID,
                                   const bool rewardHead,
                                   const std::int32_t sicReadyRecastModSec) -> std::int64_t
{
    if (abilityID == AbilityReward)
    {
        if (rewardHead)
        {
            return recastSec - 10;
        }
        return recastSec;
    }
    if (abilityID == AbilityReady || abilityID == AbilitySic)
    {
        const auto reduced = recastSec - sicReadyRecastModSec;
        return reduced > 0 ? reduced : 0;
    }
    return recastSec;
}

} // namespace charabilityrecasthelpers
