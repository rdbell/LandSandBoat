#pragma once

#include "zone_out_capacity.h"

// Pure removeCharFromZone transition policy from charutils.

namespace zoneouttransitionhelpers
{
struct ZoneOutTransitionInput
{
    bool         hasSession{};
    bool         hasZone{};
    std::uint8_t animation{};
    bool         hasTrusts{};
    std::uint8_t status{};
    bool         hasParty{};
    bool         hasAlliance{};
    bool         isPartyLeader{};
    bool         partyHasOnlyOneMember{};
    bool         allianceHasOnlyOneParty{};
    bool         petPersists{};
};

struct ZoneOutTransitionPlan
{
    bool                               markBlowfishPendingZone{};
    bool                               cleanTradePending{};
    bool                               cleanInvitePending{};
    bool                               notifyNominateOnLeave{};
    bool                               clearWideScanTarget{};
    bool                               clearAttackAnimation{};
    bool                               setUpdateHP{};
    bool                               clearTrusts{};
    zoneouthelpers::ZoneOutPartyAction partyAction{};
    bool                               savePetZoningInfo{};
    bool                               resetPetZoningInfo{};
    std::uint8_t                       sessionShuttingDown{};
    std::uint8_t                       charStatsZoning{};
    bool                               decreaseZoneCounter{};
    bool                               saveStatusEffectsAsLogout{};
    bool                               persistData{};
    bool                               savePlayTime{};
    bool                               saveCharStats{};
    bool                               saveCharExp{};
    bool                               saveEminenceData{};
    bool                               saveLastLogout{};
    bool                               setDisappearStatus{};

    constexpr auto operator==(const ZoneOutTransitionPlan&) const -> bool = default;
};

// MakeZoneOutTransitionPlan mirrors removeCharFromZone after host facts are read.
constexpr auto MakeZoneOutTransitionPlan(const ZoneOutTransitionInput& input) -> ZoneOutTransitionPlan
{
    const auto isShutdownLogout = zoneouthelpers::IsShutdownLogout(input.status);
    const auto partyAction = isShutdownLogout
        ? zoneouthelpers::ClassifyZoneOutPartyLeave(
              input.hasParty, input.hasAlliance, input.isPartyLeader, input.partyHasOnlyOneMember, input.allianceHasOnlyOneParty)
        : zoneouthelpers::ZoneOutPartyAction::None;
    const auto savePetZoningInfo = isShutdownLogout && zoneouthelpers::ShouldSetPetZoningInfo(input.petPersists);
    const auto sessionShuttingDown = zoneouthelpers::SessionShuttingDownValue(isShutdownLogout);
    const auto clearAttackAnimation = zoneouthelpers::ShouldClearAttackAnimation(input.animation);

    return {
        .markBlowfishPendingZone      = zoneouthelpers::ShouldMarkBlowfishPendingZone(input.hasSession),
        .cleanTradePending            = true,
        .cleanInvitePending           = true,
        .notifyNominateOnLeave        = zoneouthelpers::ShouldNotifyNominateOnLeave(input.hasZone),
        .clearWideScanTarget          = true,
        .clearAttackAnimation         = clearAttackAnimation,
        .setUpdateHP                  = clearAttackAnimation,
        .clearTrusts                  = zoneouthelpers::ShouldClearTrusts(input.hasTrusts),
        .partyAction                  = partyAction,
        .savePetZoningInfo            = savePetZoningInfo,
        .resetPetZoningInfo           = isShutdownLogout && !savePetZoningInfo,
        .sessionShuttingDown          = sessionShuttingDown,
        .charStatsZoning              = zoneouthelpers::CharStatsZoningValue(isShutdownLogout),
        .decreaseZoneCounter          = zoneouthelpers::ShouldDecreaseZoneCounter(input.hasZone),
        .saveStatusEffectsAsLogout    = zoneouthelpers::SaveStatusEffectsLogoutFlag(sessionShuttingDown),
        .persistData                  = true,
        .savePlayTime                 = true,
        .saveCharStats                = true,
        .saveCharExp                  = true,
        .saveEminenceData             = true,
        .saveLastLogout               = true,
        .setDisappearStatus           = true,
    };
}
} // namespace zoneouttransitionhelpers
