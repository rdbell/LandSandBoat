#pragma once

#include <cstdint>

namespace partyreloadidsynchelpers
{
// ShouldSynchronize preserves ReloadParty's existing-party ID comparison.
constexpr auto ShouldSynchronize(const std::uint32_t currentPartyID, const std::uint32_t persistedPartyID) -> bool
{
    return currentPartyID != persistedPartyID;
}
} // namespace partyreloadidsynchelpers
