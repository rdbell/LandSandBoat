#pragma once

#include "common/cbasetypes.h"

#include <array>

namespace localplayerpackethelpers
{

enum class Action : uint8
{
    GroupAttributes,
    CliStatus,
    CliStatus2,
    AbilityRecast,
    Merits,
    Monstrosity,
    JobPoints,
};

constexpr auto BuildPlan() -> std::array<Action, 7>
{
    return {
        Action::GroupAttributes,
        Action::CliStatus,
        Action::CliStatus2,
        Action::AbilityRecast,
        Action::Merits,
        Action::Monstrosity,
        Action::JobPoints,
    };
}

} // namespace localplayerpackethelpers
