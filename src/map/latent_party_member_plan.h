#pragma once

#include "data/enums/latent.h"

#include <cstddef>
#include <cstdint>

// Pure CLatentEffectContainer::CheckLatentsPartyMembers action plan.
namespace latenthelpers
{

enum class PartyMemberLatentAction
{
    Ignore,
    Activate,
    Deactivate,
};

constexpr auto DeterminePartyMemberLatentAction(const xi::Latent condition,
                                                const std::uint16_t value,
                                                const std::size_t totalMembers,
                                                const int inZoneMembers) -> PartyMemberLatentAction
{
    switch (condition)
    {
        case xi::Latent::PartyMembers:
            return value <= totalMembers ? PartyMemberLatentAction::Activate : PartyMemberLatentAction::Deactivate;
        case xi::Latent::PartyMembersInZone:
            return value <= totalMembers && inZoneMembers == value ? PartyMemberLatentAction::Activate : PartyMemberLatentAction::Deactivate;
        default:
            return PartyMemberLatentAction::Ignore;
    }
}

} // namespace latenthelpers
