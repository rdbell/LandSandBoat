#include "test_exp_loss_1505.h"

#include "map/exp_loss_capacity.h"

#include <iostream>

namespace
{
using explosshelpers::ApplyRetentionAndRate;
using explosshelpers::BaseLossAmount;
using explosshelpers::DelevelResidualExp;
using explosshelpers::EffectiveLossLevel;
using explosshelpers::IsExpLossLevelSettingValid;
using explosshelpers::IsRetainPercentValid;
using explosshelpers::RemainingExpAfterLoss;
using explosshelpers::ResolveLossAmount;
using explosshelpers::ShouldApplyDelevelToEntity;
using explosshelpers::ShouldDelevel;
using explosshelpers::ShouldSkipByLevel;
using explosshelpers::ShouldZeroExpAtLevel1;

auto Check() -> bool
{
    if (!IsRetainPercentValid(0.0f) || !IsRetainPercentValid(1.0f) || IsRetainPercentValid(-0.1f) || IsRetainPercentValid(1.1f))
    {
        return false;
    }
    if (!IsExpLossLevelSettingValid(1) || !IsExpLossLevelSettingValid(99) || IsExpLossLevelSettingValid(0) || IsExpLossLevelSettingValid(100))
    {
        return false;
    }
    if (!ShouldSkipByLevel(40, 50, 0) || ShouldSkipByLevel(40, 50, 10) || ShouldSkipByLevel(50, 50, 0))
    {
        return false;
    }
    if (EffectiveLossLevel(75, 50) != 50 || EffectiveLossLevel(75, 0) != 75 || EffectiveLossLevel(40, 50) != 40)
    {
        return false;
    }
    if (BaseLossAmount(67, 10000) != 800 || BaseLossAmount(68, 10000) != 2400)
    {
        return false;
    }
    if (ResolveLossAmount(1000, 100, 0.0f, 1.0f) != 100)
    {
        return false;
    }
    if (ApplyRetentionAndRate(1000, 0.05f, 1.0f) != 950)
    {
        return false;
    }
    if (!ShouldDelevel(100, 200, 5) || ShouldDelevel(300, 200, 5) || ShouldDelevel(100, 200, 1))
    {
        return false;
    }
    if (!ShouldZeroExpAtLevel1(100, 200, 1) || ShouldZeroExpAtLevel1(100, 200, 2))
    {
        return false;
    }
    if (DelevelResidualExp(5000, 300, 100) != 4800 || DelevelResidualExp(100, 300, 100) != 0)
    {
        return false;
    }
    if (!ShouldApplyDelevelToEntity(0, 40) || !ShouldApplyDelevelToEntity(50, 40) || ShouldApplyDelevelToEntity(40, 40))
    {
        return false;
    }
    if (RemainingExpAfterLoss(500, 100) != 400)
    {
        return false;
    }
    return true;
}
} // namespace

auto runExpLoss1505SelfTests() -> bool
{
    const bool ok = Check();
    if (!ok)
    {
        std::cerr << "exp loss 1505 self-test failed\n";
    }
    return ok;
}
