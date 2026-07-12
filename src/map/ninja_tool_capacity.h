#pragma once

#include <cstdint>
#include <optional>

// Pure HasNinjaTool policy after inventory/entity injects.
// Parity: internal/ninjatool

namespace ninjatoolhelpers
{

// ITEMID pins (items.h)
constexpr std::uint16_t Uchitake        = 1161;
constexpr std::uint16_t Tsurara         = 1164;
constexpr std::uint16_t KawahoriOgi     = 1167;
constexpr std::uint16_t Makibishi       = 1170;
constexpr std::uint16_t Hiraishin       = 1173;
constexpr std::uint16_t MizuDeppo       = 1176;
constexpr std::uint16_t Shihei          = 1179;
constexpr std::uint16_t Jusatsu         = 1182;
constexpr std::uint16_t Kaginawa        = 1185;
constexpr std::uint16_t SairuiRan       = 1188;
constexpr std::uint16_t Kodoku          = 1191;
constexpr std::uint16_t ShinobiTabi     = 1194;
constexpr std::uint16_t SanjakuTenugui  = 2553;
constexpr std::uint16_t Soshi           = 2555;
constexpr std::uint16_t Kabenro         = 2642;
constexpr std::uint16_t Jinko           = 2643;
constexpr std::uint16_t Ryuno           = 2644;
constexpr std::uint16_t Mokujin         = 2970;
constexpr std::uint16_t Inoshishinofuda = 2971;
constexpr std::uint16_t Shikanofuda     = 2972;
constexpr std::uint16_t Chonofuda       = 2973;
constexpr std::uint16_t Ranka           = 8803;
constexpr std::uint16_t Furusumi        = 8804;

inline auto Substitute(const std::uint16_t preferred) -> std::optional<std::uint16_t>
{
    switch (preferred)
    {
        case Uchitake:
        case Tsurara:
        case KawahoriOgi:
        case Makibishi:
        case Hiraishin:
        case MizuDeppo:
            return Inoshishinofuda;
        case Ryuno:
        case Mokujin:
        case SanjakuTenugui:
        case Kabenro:
        case ShinobiTabi:
        case Shihei:
        case Ranka:
        case Furusumi:
            return Shikanofuda;
        case Soshi:
        case Kodoku:
        case Kaginawa:
        case Jusatsu:
        case SairuiRan:
        case Jinko:
            return Chonofuda;
        default:
            return std::nullopt;
    }
}

constexpr auto IsElementalWheel(const std::uint16_t toolID) -> bool
{
    switch (toolID)
    {
        case Uchitake:
        case Tsurara:
        case KawahoriOgi:
        case Makibishi:
        case Hiraishin:
        case MizuDeppo:
            return true;
        default:
            return false;
    }
}

struct ResolveResult
{
    std::uint16_t toolID{};
    bool          ok{};
    bool          usedSubstitute{};
};

constexpr auto Resolve(const std::uint16_t preferred,
                       const bool          preferredAvailable,
                       const bool          isNINMain,
                       const bool          substituteAvailable,
                       const std::uint16_t substituteID) -> ResolveResult
{
    if (preferredAvailable)
    {
        return ResolveResult{ preferred, true, false };
    }
    if (!isNINMain || !substituteAvailable)
    {
        return {};
    }
    // substituteID must be the Substitute(preferred) result when available
    return ResolveResult{ substituteID, true, true };
}

// Host: if !preferredAvailable && isNINMain, look up Substitute then SearchItem.
inline auto ResolveWithSubstitute(const std::uint16_t preferred,
                                  const bool          preferredAvailable,
                                  const bool          isNINMain,
                                  const bool          substituteAvailable) -> ResolveResult
{
    if (preferredAvailable)
    {
        return ResolveResult{ preferred, true, false };
    }
    if (!isNINMain)
    {
        return {};
    }
    const auto sub = Substitute(preferred);
    if (!sub || !substituteAvailable)
    {
        return {};
    }
    return ResolveResult{ *sub, true, true };
}

constexpr auto ExpertiseChance(const std::uint16_t ninjaToolMod, const std::uint16_t meritBonus) -> std::uint16_t
{
    return static_cast<std::uint16_t>(ninjaToolMod + meritBonus);
}

// roll is GetRandomNumber(100) → 0..99; consume when roll > chance.
constexpr auto ShouldConsume(const std::uint16_t chance, const int roll) -> bool
{
    return roll > static_cast<int>(chance);
}

// ConsumeQty when ConsumeTool is true.
constexpr auto ConsumeQty(const std::uint16_t toolID,
                          const bool          hasFutae,
                          const std::uint16_t expertiseChance,
                          const int           roll) -> int
{
    if (hasFutae && IsElementalWheel(toolID))
    {
        return 2;
    }
    if (ShouldConsume(expertiseChance, roll))
    {
        return 1;
    }
    return 0;
}

// Non-PC entities always succeed without tool checks.
constexpr auto NonPCAlwaysHasTool() -> bool
{
    return true;
}

} // namespace ninjatoolhelpers
