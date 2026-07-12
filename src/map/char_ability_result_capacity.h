#pragma once

#include <cstdint>
#include <functional>

// Pure ability action-result message/param resolution from CCharEntity::OnAbility
// single-target and AoE loops. Host supplies Lua-mutated message IDs and absorb/AoE maps.

namespace charabilityresulthelpers
{

// MsgBasic pins.
constexpr std::uint16_t MsgNone           = 0;
constexpr std::uint16_t MsgUsesJobAbility = 100;

struct Result
{
    std::uint16_t messageID{};
    std::int32_t  param{};
};

// ResolveSingle mirrors the non-AoE OnAbility result tail after OnUseAbility.
template <typename AbsorbVariant>
inline auto ResolveSingle(const std::uint16_t messageBeforeLua,
                          const std::uint16_t messageAfterLua,
                          const std::uint16_t abilityDefaultMessage,
                          const std::int32_t value,
                          AbsorbVariant&& absorbVariant) -> Result
{
    std::uint16_t messageID = messageAfterLua;
    if (messageBeforeLua == messageAfterLua)
    {
        messageID = abilityDefaultMessage;
    }
    if (messageID == MsgNone)
    {
        messageID = MsgUsesJobAbility;
    }

    Result result{ .messageID = messageID, .param = value };
    if (value < 0)
    {
        result.messageID = static_cast<std::uint16_t>(std::invoke(absorbVariant, messageID));
        result.param     = -value;
    }
    return result;
}

// ResolveAoETarget mirrors one AoE-target result after OnUseAbility.
// firstTarget uses ability default; later targets use the AoE variant map.
template <typename AoEVariant, typename AbsorbVariant>
inline auto ResolveAoETarget(const bool firstTarget,
                             const std::uint16_t abilityDefaultMessage,
                             const std::int32_t value,
                             AoEVariant&& aoeVariant,
                             AbsorbVariant&& absorbVariant) -> Result
{
    std::uint16_t messageID = firstTarget ? abilityDefaultMessage
                                          : static_cast<std::uint16_t>(std::invoke(aoeVariant, abilityDefaultMessage));
    Result result{ .messageID = messageID, .param = value };
    if (value < 0)
    {
        result.messageID = static_cast<std::uint16_t>(std::invoke(absorbVariant, result.messageID));
        result.param     = -result.param;
    }
    return result;
}

} // namespace charabilityresulthelpers
