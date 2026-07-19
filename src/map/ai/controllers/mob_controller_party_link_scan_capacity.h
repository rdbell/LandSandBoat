#pragma once

namespace mobcontrollerpartylinkscan
{
struct Scan
{
    bool scanThisTick;
    bool shouldScan;
};

// Resolve advances the alternating party-link scan cadence and admits eligible scans.
template <typename PartyMemberCount, typename IsOneWayLinking>
constexpr auto Resolve(const bool previousScanThisTick, const bool hasParty, PartyMemberCount&& partyMemberCount, IsOneWayLinking&& isOneWayLinking) -> Scan
{
    const bool scanThisTick = !previousScanThisTick;
    return { scanThisTick, scanThisTick && hasParty && partyMemberCount() > 1 && !isOneWayLinking() };
}
} // namespace mobcontrollerpartylinkscan
