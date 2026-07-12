#pragma once

#include <algorithm>
#include <cstdint>

// Pure EXP distribution math from charutils::DistributeExperiencePoints
// (party-gap penalty, monster bonus, per-level caps, chain multipliers, chain
// timer durations). Host retains alliance iteration, signet/sanction, share
// multipliers, and award packets.

namespace expdistributehelpers
{

// Distance threshold for party counting and sync checks (yalms).
constexpr float PartyDistance = 100.0f;

// IsInPartyRange mirrors distance < 100.
inline auto IsInPartyRange(const float dist) -> bool
{
    return dist < PartyDistance;
}

// IsSyncTargetBlocking mirrors distance >= 100 || hp == 0 for the sync target.
inline auto IsSyncTargetBlocking(const float distToSync, const bool syncDead) -> bool
{
    return distToSync >= PartyDistance || syncDead;
}

// ApplyPartyGapPenalty applies EXP_PARTY_GAP_PENALTIES when enabled.
// partyGapNoExp > 0 and maxlevel >= memberlevel + partyGapNoExp → 0.
// else if maxlevel > 50 || maxlevel > memberlevel + 7 → * memberlevel/maxlevel
// else → * nextExp(member)/nextExp(max)
inline auto ApplyPartyGapPenalty(const float exp,
                                 const std::uint8_t maxlevel,
                                 const std::uint8_t memberlevel,
                                 const std::uint8_t partyGapNoExp,
                                 const std::uint32_t memberNextExp,
                                 const std::uint32_t maxNextExp) -> float
{
    if (partyGapNoExp > 0 && maxlevel >= static_cast<std::uint8_t>(memberlevel + partyGapNoExp))
    {
        return 0.0f;
    }
    if (maxlevel > 50 || maxlevel > static_cast<std::uint8_t>(memberlevel + 7))
    {
        if (maxlevel == 0)
        {
            return exp;
        }
        return exp * (static_cast<float>(memberlevel) / static_cast<float>(maxlevel));
    }
    if (maxNextExp == 0)
    {
        return exp;
    }
    return exp * (static_cast<float>(memberNextExp) / static_cast<float>(maxNextExp));
}

// ApplyMonsterBonus mirrors 1.0 + MOBMOD_EXP_BONUS/100 when mod non-zero.
inline auto ApplyMonsterBonus(const float exp, const std::int16_t expBonusMod) -> float
{
    if (expBonusMod == 0)
    {
        return exp;
    }
    return exp * (1.0f + static_cast<float>(expBonusMod) / 100.0f);
}

// CapExpByLevel mirrors the 400/500/600 per-monster caps by member main level.
inline auto CapExpByLevel(const float exp, const std::uint8_t memberLevel) -> float
{
    if (memberLevel <= 50)
    {
        return std::min(exp, 400.0f);
    }
    if (memberLevel <= 60)
    {
        return std::min(exp, 500.0f);
    }
    return std::min(exp, 600.0f);
}

// ChainMultiplier for expChain.chainNumber when chain is active.
inline auto ChainMultiplier(const std::uint8_t chainNumber) -> float
{
    switch (chainNumber)
    {
        case 0:
            return 1.0f;
        case 1:
            return 1.2f;
        case 2:
            return 1.25f;
        case 3:
            return 1.3f;
        case 4:
            return 1.4f;
        case 5:
            return 1.5f;
        default:
            return 1.55f;
    }
}

// ApplyChainMultiplier multiplies exp by the chain number factor.
inline auto ApplyChainMultiplier(const float exp, const std::uint8_t chainNumber) -> float
{
    return exp * ChainMultiplier(chainNumber);
}

// ChainTimerSeconds for inactive-chain reset by member main level.
inline auto ChainTimerSeconds(const std::uint8_t memberLevel) -> std::int64_t
{
    if (memberLevel <= 10)
    {
        return 50;
    }
    if (memberLevel <= 20)
    {
        return 100;
    }
    if (memberLevel <= 30)
    {
        return 150;
    }
    if (memberLevel <= 40)
    {
        return 200;
    }
    if (memberLevel <= 50)
    {
        return 250;
    }
    if (memberLevel <= 60)
    {
        return 300;
    }
    return 360;
}

// MaxTrackedPartySize mirrors max(pcinzone, m_HiPartySize).
constexpr auto MaxTrackedPartySize(const std::uint8_t pcinzone, const std::uint8_t hiPartySize) -> std::uint8_t
{
    return pcinzone > hiPartySize ? pcinzone : hiPartySize;
}

// MaxTrackedPCLevel mirrors max(maxlevel, m_HiPCLvl).
constexpr auto MaxTrackedPCLevel(const std::uint8_t maxlevel, const std::uint8_t hiPCLvl) -> std::uint8_t
{
    return maxlevel > hiPCLvl ? maxlevel : hiPCLvl;
}

// ShouldProcessMember mirrors !null && !dead for alliance EXP award path.
constexpr auto ShouldProcessMember(const bool isChar, const bool isDead) -> bool
{
    return isChar && !isDead;
}

// ShouldAwardBaseExp mirrors mobCheck > TooWeak.
constexpr auto ShouldAwardBaseExp(const bool aboveTooWeak) -> bool
{
    return aboveTooWeak;
}

// ShouldApplyChain mirrors mobCheck > DecentChallenge.
constexpr auto ShouldApplyChain(const bool aboveDecentChallenge) -> bool
{
    return aboveDecentChallenge;
}

} // namespace expdistributehelpers
