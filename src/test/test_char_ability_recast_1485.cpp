#include "test_char_ability_recast_1485.h"

#include "map/char_ability_recast_capacity.h"

#include <iostream>

namespace
{
using charabilityrecasthelpers::AdjustPostParalysis;
using charabilityrecasthelpers::BuildInitial;
using charabilityrecasthelpers::InitialInput;

auto CheckInitial() -> bool
{
    // Plain ability: recast minus merit reduction (non-merit add type).
    {
        const auto plan = BuildInitial({
            .abilityID                 = 1,
            .recastID                  = 5,
            .meritModID                = 1,
            .meritRecastReductionSec   = 3,
            .abilityRecastSec          = 30,
        });
        if (plan.recastSec != 27 || plan.baseChargeTimeSec != 0 || plan.setBPRecastTime)
        {
            return false;
        }
    }

    // Merit add-type skips merit reduction.
    {
        const auto plan = BuildInitial({
            .abilityID                 = 1,
            .addType                   = charabilityrecasthelpers::AddTypeMerit,
            .meritModID                = 1,
            .meritRecastReductionSec   = 3,
            .abilityRecastSec          = 30,
        });
        if (plan.recastSec != 30)
        {
            return false;
        }
    }

    // Charge path with Sic recast overwrites reduction and multiplies charge time.
    {
        const auto plan = BuildInitial({
            .abilityID               = 10,
            .recastID                = charabilityrecasthelpers::RecastSic,
            .meritModID              = 1,
            .meritRecastReductionSec = 99,
            .hasCharge               = true,
            .chargeTimeSec           = 20,
            .abilityRecastSec        = 2, // charges used
            .sicMeritReductionSec    = 5,
        });
        // baseChargeTime = 20 - 5 = 15; recast = 15 * 2 = 30
        if (plan.baseChargeTimeSec != 15 || plan.recastSec != 30)
        {
            return false;
        }
    }

    // ABILITY_SIC never uses charge branch even when a charge exists.
    {
        const auto plan = BuildInitial({
            .abilityID        = charabilityrecasthelpers::AbilitySic,
            .hasCharge        = true,
            .chargeTimeSec    = 20,
            .abilityRecastSec = 15,
        });
        if (plan.recastSec != 15 || plan.baseChargeTimeSec != 0)
        {
            return false;
        }
    }

    // Strategems charge + mod, then Tabula Rasa zeroes recast and base charge.
    {
        const auto plan = BuildInitial({
            .abilityID               = 1,
            .recastID                = charabilityrecasthelpers::RecastStrategems,
            .hasCharge               = true,
            .chargeTimeSec           = 60,
            .abilityRecastSec        = 1,
            .strategemRecastModSec   = 10,
            .hasTabulaRasa           = true,
        });
        if (plan.recastSec != 0 || plan.baseChargeTimeSec != 0)
        {
            return false;
        }
    }

    // Light Arts + Tabula Rasa zeroes recast.
    {
        const auto plan = BuildInitial({
            .abilityID        = charabilityrecasthelpers::AbilityLightArts,
            .abilityRecastSec = 60,
            .hasTabulaRasa    = true,
        });
        if (plan.recastSec != 0)
        {
            return false;
        }
    }

    // Blood Pact: snapshot reduced recast into localvar, zero action recast.
    {
        const auto plan = BuildInitial({
            .abilityID          = 1,
            .recastID           = charabilityrecasthelpers::RecastBloodPactRage,
            .abilityRecastSec   = 60,
            .bpDelayMod         = 20, // cap 15
            .bpDelayIIMod       = 20, // cap 15 → stacked 30
            .avatarsFavorPower  = 12, // cap 10 → total reduction 40
        });
        if (!plan.setBPRecastTime || plan.bpRecastTime != 20 || plan.recastSec != 0)
        {
            return false;
        }
    }

    // Blood Pact floor at 0.
    {
        const auto plan = BuildInitial({
            .recastID          = charabilityrecasthelpers::RecastBloodPactWard,
            .abilityRecastSec  = 5,
            .bpDelayMod        = 15,
            .avatarsFavorPower = 10,
        });
        if (!plan.setBPRecastTime || plan.bpRecastTime != 0 || plan.recastSec != 0)
        {
            return false;
        }
    }

    return true;
}

auto CheckAdjust() -> bool
{
    for (const auto itemID : { 15157u, 15158u, 16104u, 16105u })
    {
        if (!charabilityrecasthelpers::IsRewardRecastHead(itemID))
        {
            return false;
        }
    }
    for (const auto itemID : { 0u, 1u, 15156u, 15159u, 16103u, 16106u, 65535u })
    {
        if (charabilityrecasthelpers::IsRewardRecastHead(itemID))
        {
            return false;
        }
    }
    if (AdjustPostParalysis(30, charabilityrecasthelpers::AbilityReward, true, 0) != 20)
    {
        return false;
    }
    if (AdjustPostParalysis(30, charabilityrecasthelpers::AbilityReward, false, 0) != 30)
    {
        return false;
    }
    if (AdjustPostParalysis(12, charabilityrecasthelpers::AbilityReady, false, 5) != 7)
    {
        return false;
    }
    if (AdjustPostParalysis(3, charabilityrecasthelpers::AbilitySic, false, 5) != 0)
    {
        return false;
    }
    if (AdjustPostParalysis(30, 1, true, 99) != 30)
    {
        return false;
    }
    return true;
}
} // namespace

auto runCharAbilityRecast1485SelfTests() -> bool
{
    const bool ok = CheckInitial() && CheckAdjust();
    if (!ok)
    {
        std::cerr << "char ability recast 1485 self-test failed\n";
    }
    return ok;
}
