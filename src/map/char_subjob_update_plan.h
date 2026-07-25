#pragma once

#include "common/cbasetypes.h"

#include <array>

namespace subjobupdatehelpers
{

enum class Action : uint8
{
    RefreshGiftMods,
    BuildSkills,
    CalculateStats,
    CheckValidEquipment,
    ChangeRecasts,
    BuildAbilities,
    BuildTraits,
    UpdateHealth,
    RestoreHP,
    RestoreMP,
    SaveStats,
    SaveJob,
    SaveExperience,
    MarkHealthUpdate,
    PacketJobInfo,
    PacketCliStatus,
    PacketCliStatus2,
    PacketAbilityRecast,
    PacketCommandData,
    PacketCharacterStatus,
    PacketMerits,
    PacketMonstrosity1,
    PacketMonstrosity2,
    PacketCharacterSync,
};

constexpr auto BuildPlan() -> std::array<Action, 24>
{
    return {
        Action::RefreshGiftMods,
        Action::BuildSkills,
        Action::CalculateStats,
        Action::CheckValidEquipment,
        Action::ChangeRecasts,
        Action::BuildAbilities,
        Action::BuildTraits,
        Action::UpdateHealth,
        Action::RestoreHP,
        Action::RestoreMP,
        Action::SaveStats,
        Action::SaveJob,
        Action::SaveExperience,
        Action::MarkHealthUpdate,
        Action::PacketJobInfo,
        Action::PacketCliStatus,
        Action::PacketCliStatus2,
        Action::PacketAbilityRecast,
        Action::PacketCommandData,
        Action::PacketCharacterStatus,
        Action::PacketMerits,
        Action::PacketMonstrosity1,
        Action::PacketMonstrosity2,
        Action::PacketCharacterSync,
    };
}

} // namespace subjobupdatehelpers
