#include "test_status_effect_add_remove_1371.h"

#include "map/status_effect_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "status effect add/remove 1371 self-test failed: " << label << '\n';
    }
    return condition;
}

auto expectEq(const auto actual, const auto expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "status effect add/remove 1371 self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

} // namespace

auto runStatusEffectAddRemove1371SelfTests() -> bool
{
    using namespace statuseffecthelpers;
    bool ok = true;

    ok = expect(ShouldRejectNullStatusEffect(true) && !ShouldRejectNullStatusEffect(false), "null") && ok;
    ok = expect(ShouldClampMinDuration(5, 10) && !ShouldClampMinDuration(10, 10), "min dur") && ok;
    ok = expect(ShouldCheckManeuverAttachments(ManeuverIDFirst, true), "maneuver pc") && ok;
    ok = expect(!ShouldCheckManeuverAttachments(ManeuverIDFirst, false), "maneuver non-pc") && ok;
    ok = expect(!ShouldCheckManeuverAttachments(3, true), "not maneuver") && ok;
    ok = expect(ShouldUpdateHealthOnGain(true) && !ShouldUpdateHealthOnGain(false), "hp") && ok;
    ok = expect(ShouldUpdateStatusIconsOnGain(true, 1) && !ShouldUpdateStatusIconsOnGain(true, 0), "icons") && ok;
    ok = expect(!ShouldUpdateStatusIconsOnGain(false, 1), "icons non-pc") && ok;
    ok = expect(ShouldCheckLatentsOnGain(true, true) && !ShouldCheckLatentsOnGain(true, false), "latents") && ok;

    ok = expect(ShouldNotifyLossMessage(false, 5, false), "notify") && ok;
    ok = expect(!ShouldNotifyLossMessage(true, 5, false), "silent") && ok;
    ok = expect(!ShouldNotifyLossMessage(false, 0, false), "no icon") && ok;
    ok = expect(!ShouldNotifyLossMessage(false, 5, true), "no loss msg") && ok;

    ok = expect(ShouldNotifyOriginOnLoss(10, 5, true), "origin other") && ok;
    ok = expect(!ShouldNotifyOriginOnLoss(5, 5, true), "origin self") && ok;
    ok = expect(!ShouldNotifyOriginOnLoss(0, 5, true), "origin zero") && ok;
    ok = expect(ShouldNotifyOriginOnLoss(10, 0, false), "origin non-pc") && ok;

    ok = expectEq(WearOffMessageOrDefault(3, MaxEffectID, 99, 206), static_cast<uint16>(99), "wear catalog") && ok;
    ok = expectEq(WearOffMessageOrDefault(MaxEffectID, MaxEffectID, 99, 206), static_cast<uint16>(206), "wear default") && ok;
    ok = expect(ShouldNotifyNonPCLoss(true, false) && !ShouldNotifyNonPCLoss(true, true), "non-pc dead") && ok;
    ok = expect(ShouldMarkDeleted(false) && !ShouldMarkDeleted(true), "mark") && ok;
    ok = expect(IsSilentNotice(EffectNoticeSilent, EffectNoticeSilent), "silent notice") && ok;
    ok = expect(!IsSilentNotice(EffectNoticeShowMessage, EffectNoticeSilent), "show notice") && ok;

    return ok;
}
