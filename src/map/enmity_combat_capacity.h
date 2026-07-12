#pragma once

#include <cstdint>

// Pure killshot/secondary/transfer/Issekigan enmity combat tails.

namespace enmitycombathelpers
{

constexpr std::int32_t  IssekiganCE           = 300;
constexpr std::uint16_t IssekiganJPVEPerPoint = 10;

constexpr auto ShouldTransferEnmity(const bool hasMob, const bool hasEnmityContainer) -> bool
{
    return hasMob && hasEnmityContainer;
}

constexpr auto ShouldClearKillshotEnmity(const bool attackerIsMob,
                                         const bool hasTarget,
                                         const bool targetDead,
                                         const bool hasHighest,
                                         const bool highestTargidMatches) -> bool
{
    return attackerIsMob && hasTarget && targetDead && hasHighest && highestTargidMatches;
}

enum class SecondaryTargetEnmityAction : std::uint8_t
{
    None = 0,
    Deactivate,
    Activate,
};

constexpr auto ClassifySecondaryTargetEnmity(const bool attackerIsMob, const bool hasTarget, const bool targetDead)
    -> SecondaryTargetEnmityAction
{
    if (!attackerIsMob || !hasTarget)
    {
        return SecondaryTargetEnmityAction::None;
    }
    if (targetDead)
    {
        return SecondaryTargetEnmityAction::Deactivate;
    }
    return SecondaryTargetEnmityAction::Activate;
}

struct IssekiganEnmityResult
{
    bool         applied{ false };
    std::int32_t ce{ 0 };
    std::int32_t ve{ 0 };
};

constexpr auto IssekiganEnmityBonus(const bool         attackerIsMob,
                                    const bool         defenderHasIssekigan,
                                    const bool         defenderIsPC,
                                    const std::uint8_t issekiganJP) -> IssekiganEnmityResult
{
    if (!attackerIsMob || !defenderHasIssekigan)
    {
        return {};
    }
    std::int32_t ve = 0;
    if (defenderIsPC)
    {
        ve = static_cast<std::int32_t>(static_cast<std::uint16_t>(issekiganJP) * IssekiganJPVEPerPoint);
    }
    return IssekiganEnmityResult{ true, IssekiganCE, ve };
}

} // namespace enmitycombathelpers
