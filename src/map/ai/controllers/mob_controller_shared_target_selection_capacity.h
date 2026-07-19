#pragma once

#include <cstdint>

namespace mobcontrollersharedtargetselection
{
enum class Source : std::uint8_t
{
    None,
    Partner,
    Enmity,
};

inline auto Select(bool hasSharedMob, bool sharedTargetExists, bool hasHighestEnmity) -> Source
{
    if (hasSharedMob && sharedTargetExists)
    {
        return Source::Partner;
    }
    return hasHighestEnmity ? Source::Enmity : Source::None;
}
} // namespace mobcontrollersharedtargetselection
