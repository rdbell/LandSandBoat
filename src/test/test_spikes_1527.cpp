#include "test_spikes_1527.h"

#include "map/spikes_capacity.h"

#include <iostream>

namespace
{
using spikeshelpers::ClassifySpikesPath;
using spikeshelpers::DreadSpikesOnUndeadNull;
using spikeshelpers::ItemSubEffectIsSpikeType;
using spikeshelpers::RetaliationDamage;
using spikeshelpers::RetaliationProc;
using spikeshelpers::ReprisalApplies;
using spikeshelpers::SpikesEquipLevelDiff;
using spikeshelpers::SpikesEquipProcs;
using spikeshelpers::SpikesEquipRatio;
using spikeshelpers::SpikesPacketParam;
using spikeshelpers::SpikesPath;
using spikeshelpers::ShouldCallOnSpikesDamage;
using spikeshelpers::ShouldClaimOnSpikeKill;
using spikeshelpers::ShouldRunSpellSpikeSwitch;
using spikeshelpers::ParrySpikesActive;

auto Check() -> bool
{
    if (ClassifySpikesPath(true, true, true) != SpikesPath::Retaliation)
    {
        return false;
    }
    if (ClassifySpikesPath(false, true, true) != SpikesPath::SpellOrAuto)
    {
        return false;
    }
    if (ClassifySpikesPath(false, false, true) != SpikesPath::ItemGear)
    {
        return false;
    }
    if (ClassifySpikesPath(false, false, false) != SpikesPath::ClearNone)
    {
        return false;
    }
    if (!RetaliationProc(true, true, 100, 49, true) || RetaliationProc(true, true, 100, 50, true))
    {
        return false;
    }
    if (RetaliationDamage(100, 50) != 150)
    {
        return false;
    }
    if (SpikesPacketParam(40, 100, 50) != 40 || SpikesPacketParam(-30, 100, 80) != 20)
    {
        return false;
    }
    if (!ShouldRunSpellSpikeSwitch(false, 1) || ShouldRunSpellSpikeSwitch(true, 1))
    {
        return false;
    }
    if (!ShouldCallOnSpikesDamage(true, 1) || ShouldCallOnSpikesDamage(true, 0))
    {
        return false;
    }
    if (!DreadSpikesOnUndeadNull(true, true) || DreadSpikesOnUndeadNull(true, false))
    {
        return false;
    }
    if (!ReprisalApplies(true, true) || ReprisalApplies(true, false))
    {
        return false;
    }
    if (SpikesEquipLevelDiff(20, 10) != 10 || SpikesEquipLevelDiff(1, 20) != -10)
    {
        return false;
    }
    if (!SpikesEquipProcs(20, 10, 29) || SpikesEquipProcs(20, 10, 30))
    {
        return false;
    }
    if (SpikesEquipRatio(3) != 1 || SpikesEquipRatio(40) != 10)
    {
        return false;
    }
    if (!ItemSubEffectIsSpikeType(6) || ItemSubEffectIsSpikeType(7))
    {
        return false;
    }
    if (!ShouldClaimOnSpikeKill(true) || ShouldClaimOnSpikeKill(false))
    {
        return false;
    }
    if (!ParrySpikesActive(false) || ParrySpikesActive(true))
    {
        return false;
    }
    return true;
}
} // namespace

auto runSpikes1527SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "spikes_1527 self-tests failed\n";
        return false;
    }
    return true;
}
