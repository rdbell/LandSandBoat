#include "test_enmity_combat_1526.h"

#include "map/enmity_combat_capacity.h"

#include <iostream>

namespace
{
using enmitycombathelpers::ClassifySecondaryTargetEnmity;
using enmitycombathelpers::IssekiganEnmityBonus;
using enmitycombathelpers::SecondaryTargetEnmityAction;
using enmitycombathelpers::ShouldClearKillshotEnmity;
using enmitycombathelpers::ShouldTransferEnmity;

auto Check() -> bool
{
    if (!ShouldTransferEnmity(true, true) || ShouldTransferEnmity(false, true) || ShouldTransferEnmity(true, false))
    {
        return false;
    }
    if (!ShouldClearKillshotEnmity(true, true, true, true, true) || ShouldClearKillshotEnmity(true, true, false, true, true))
    {
        return false;
    }
    if (ClassifySecondaryTargetEnmity(true, true, true) != SecondaryTargetEnmityAction::Deactivate)
    {
        return false;
    }
    if (ClassifySecondaryTargetEnmity(true, true, false) != SecondaryTargetEnmityAction::Activate)
    {
        return false;
    }
    if (ClassifySecondaryTargetEnmity(false, true, true) != SecondaryTargetEnmityAction::None)
    {
        return false;
    }
    {
        const auto r = IssekiganEnmityBonus(true, true, true, 3);
        if (!r.applied || r.ce != 300 || r.ve != 30)
        {
            return false;
        }
    }
    {
        const auto r = IssekiganEnmityBonus(true, true, false, 5);
        if (!r.applied || r.ve != 0)
        {
            return false;
        }
    }
    if (IssekiganEnmityBonus(false, true, true, 1).applied || IssekiganEnmityBonus(true, false, true, 1).applied)
    {
        return false;
    }
    return true;
}
} // namespace

auto runEnmityCombat1526SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "enmity_combat_1526 self-tests failed\n";
        return false;
    }
    return true;
}
