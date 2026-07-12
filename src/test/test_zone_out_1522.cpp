#include "test_zone_out_1522.h"

#include "map/zone_out_capacity.h"

#include <iostream>

namespace
{
using zoneouthelpers::CharStatsZoningValue;
using zoneouthelpers::ClassifyZoneOutPartyLeave;
using zoneouthelpers::IsInstalledMannequin;
using zoneouthelpers::IsMogsafeContainer;
using zoneouthelpers::IsOrchestrionFurniture;
using zoneouthelpers::IsShutdownLogout;
using zoneouthelpers::IsValidMogsafeSlotIndex;
using zoneouthelpers::RemainingDeathDurationSeconds;
using zoneouthelpers::SaveStatusEffectsLogoutFlag;
using zoneouthelpers::SessionShuttingDownValue;
using zoneouthelpers::ShouldApplyDeathTimestamp;
using zoneouthelpers::ShouldClearAttackAnimation;
using zoneouthelpers::ShouldClearTrusts;
using zoneouthelpers::ShouldDecreaseZoneCounter;
using zoneouthelpers::ShouldMarkBlowfishPendingZone;
using zoneouthelpers::ShouldNotifyNominateOnLeave;
using zoneouthelpers::ShouldSetPetZoningInfo;
using zoneouthelpers::ShouldWarnInvalidMannequinRace;
using zoneouthelpers::ZoneOutPartyAction;

auto Check() -> bool
{
    if (!ShouldMarkBlowfishPendingZone(true) || ShouldMarkBlowfishPendingZone(false))
    {
        return false;
    }
    if (!ShouldNotifyNominateOnLeave(true) || ShouldNotifyNominateOnLeave(false))
    {
        return false;
    }
    if (!ShouldClearAttackAnimation(1) || ShouldClearAttackAnimation(0))
    {
        return false;
    }
    if (!ShouldClearTrusts(true) || ShouldClearTrusts(false))
    {
        return false;
    }
    if (!IsShutdownLogout(20) || IsShutdownLogout(2))
    {
        return false;
    }
    if (ClassifyZoneOutPartyLeave(false, false, false, false, false) != ZoneOutPartyAction::None)
    {
        return false;
    }
    if (ClassifyZoneOutPartyLeave(true, false, true, false, false) != ZoneOutPartyAction::RemoveMember)
    {
        return false;
    }
    if (ClassifyZoneOutPartyLeave(true, true, false, false, false) != ZoneOutPartyAction::RemoveMember)
    {
        return false;
    }
    if (ClassifyZoneOutPartyLeave(true, true, true, true, true) != ZoneOutPartyAction::DissolveAlliance)
    {
        return false;
    }
    if (ClassifyZoneOutPartyLeave(true, true, true, true, false) != ZoneOutPartyAction::RemovePartyFromAlliance)
    {
        return false;
    }
    if (ClassifyZoneOutPartyLeave(true, true, true, false, true) != ZoneOutPartyAction::RemoveMember)
    {
        return false;
    }
    if (!ShouldSetPetZoningInfo(true) || ShouldSetPetZoningInfo(false))
    {
        return false;
    }
    if (SessionShuttingDownValue(true) != 1 || SessionShuttingDownValue(false) != 2)
    {
        return false;
    }
    if (CharStatsZoningValue(true) != 0 || CharStatsZoningValue(false) != 1)
    {
        return false;
    }
    if (!ShouldDecreaseZoneCounter(true) || ShouldDecreaseZoneCounter(false))
    {
        return false;
    }
    if (!SaveStatusEffectsLogoutFlag(1) || SaveStatusEffectsLogoutFlag(2))
    {
        return false;
    }
    if (!IsMogsafeContainer(1) || !IsMogsafeContainer(9) || IsMogsafeContainer(0))
    {
        return false;
    }
    if (!IsOrchestrionFurniture(true, true, 426) || IsOrchestrionFurniture(true, false, 426) || IsOrchestrionFurniture(true, true, 425))
    {
        return false;
    }
    if (!IsValidMogsafeSlotIndex(1, 80) || !IsValidMogsafeSlotIndex(80, 80) || IsValidMogsafeSlotIndex(0, 80) || IsValidMogsafeSlotIndex(81, 80))
    {
        return false;
    }
    if (!IsInstalledMannequin(true, true, true) || IsInstalledMannequin(true, true, false))
    {
        return false;
    }
    if (!ShouldWarnInvalidMannequinRace(0) || ShouldWarnInvalidMannequinRace(1))
    {
        return false;
    }
    if (!ShouldApplyDeathTimestamp(0) || ShouldApplyDeathTimestamp(1))
    {
        return false;
    }
    if (RemainingDeathDurationSeconds(3600, 600) != 3000 || RemainingDeathDurationSeconds(3600, 4000) != -400)
    {
        return false;
    }
    return true;
}
} // namespace

auto runZoneOut1522SelfTests() -> bool
{
    const bool ok = Check();
    if (!ok)
    {
        std::cerr << "zone out 1522 self-test failed\n";
    }
    return ok;
}
