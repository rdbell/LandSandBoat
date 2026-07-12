#include "test_exp_award_1504.h"

#include "map/exp_award_capacity.h"

#include <iostream>

namespace
{
using expawardhelpers::CapExpAtNextMinusOne;
using expawardhelpers::IsAtGenkaiCap;
using expawardhelpers::IsLimitModeFromCap;
using expawardhelpers::IsLimitModeFromMerit;
using expawardhelpers::PostLevelResidualExp;
using expawardhelpers::SelectMessage;
using expawardhelpers::ShouldApplyExpRate;
using expawardhelpers::ShouldApplyLevelToEntity;
using expawardhelpers::ShouldIncrementChainNumber;
using expawardhelpers::ShouldLevelUp;
using expawardhelpers::ShouldRejectDead;
using expawardhelpers::ShouldShowExpMessage;
using expawardhelpers::ShouldShowLevelUpAnimation;
using expawardhelpers::ShouldUseChainMessage;
using expawardhelpers::ExpMessage;

auto Check() -> bool
{
    if (!ShouldRejectDead(true, false) || ShouldRejectDead(true, true) || ShouldRejectDead(false, false))
    {
        return false;
    }
    if (!ShouldApplyExpRate(false, false) || ShouldApplyExpRate(true, false) || ShouldApplyExpRate(false, true))
    {
        return false;
    }
    if (!IsLimitModeFromMerit(true, 75, false) || IsLimitModeFromMerit(true, 74, false) || IsLimitModeFromMerit(false, 75, false))
    {
        return false;
    }
    if (!IsLimitModeFromCap(75, 75, 99, 100) || IsLimitModeFromCap(75, 75, 50, 100) || IsLimitModeFromCap(74, 75, 99, 100))
    {
        return false;
    }
    if (!ShouldShowExpMessage(false, 10) || ShouldShowExpMessage(true, 10) || ShouldShowExpMessage(false, 0))
    {
        return false;
    }
    if (!ShouldUseChainMessage(true, true) || ShouldUseChainMessage(false, true) || ShouldUseChainMessage(true, false))
    {
        return false;
    }
    if (SelectMessage(false, true, true) != ExpMessage::ExpChain ||
        SelectMessage(true, true, true) != ExpMessage::LimitChain ||
        SelectMessage(false, true, false) != ExpMessage::ExpGained ||
        SelectMessage(true, false, false) != ExpMessage::LimitGained)
    {
        return false;
    }
    if (!ShouldIncrementChainNumber(true) || ShouldIncrementChainNumber(false))
    {
        return false;
    }
    if (!ShouldLevelUp(900, 200, 1000, false) || ShouldLevelUp(900, 200, 1000, true) || ShouldLevelUp(100, 50, 1000, false))
    {
        return false;
    }
    if (!IsAtGenkaiCap(75, 75) || IsAtGenkaiCap(74, 75))
    {
        return false;
    }
    if (CapExpAtNextMinusOne(1000) != 999)
    {
        return false;
    }
    if (PostLevelResidualExp(500, 800) != 500 || PostLevelResidualExp(900, 800) != 799)
    {
        return false;
    }
    if (!ShouldApplyLevelToEntity(0, 50) || !ShouldApplyLevelToEntity(60, 50) || ShouldApplyLevelToEntity(40, 50))
    {
        return false;
    }
    if (!ShouldShowLevelUpAnimation(false) || ShouldShowLevelUpAnimation(true))
    {
        return false;
    }
    return true;
}
} // namespace

auto runExpAward1504SelfTests() -> bool
{
    const bool ok = Check();
    if (!ok)
    {
        std::cerr << "exp award 1504 self-test failed\n";
    }
    return ok;
}
