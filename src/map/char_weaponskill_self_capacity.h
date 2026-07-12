#pragma once

#include <algorithm>
#include <cstdint>

// Pure self-target weaponskill result policy from CCharEntity::OnWeaponSkillFinished.
// Host applies addMP and keeps isValidSelfTargetWeaponskill lookup.

namespace charweaponskillselfhelpers
{

// MsgBasic pins.
constexpr std::uint16_t MsgUsesSkillRecoversMP = 224;
constexpr std::uint16_t MsgTargetRecoversMP    = 276;

struct Result
{
    std::uint16_t messageID{};
    std::int32_t  healAmount{}; // max(damage, 0) passed to addMP
};

// BuildResult selects primary/secondary recover-MP messages and floors damage.
constexpr auto BuildResult(const bool primary, const std::int32_t damage) -> Result
{
    return Result{
        .messageID  = primary ? MsgUsesSkillRecoversMP : MsgTargetRecoversMP,
        .healAmount = std::max(damage, 0),
    };
}

} // namespace charweaponskillselfhelpers
