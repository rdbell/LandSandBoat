#include "test_automaton_repair_mana_1587.h"

#include "map/automaton_repair_mana_capacity.h"

#include <iostream>

namespace
{
using namespace automatonrepairmanahelpers;

auto Check() -> bool
{
    // Attachment boosts
    if (RepairKitHPBoost(AutoRepairKit) != 1 || RepairKitHPBoost(AutoRepairKitIV) != 4 || RepairKitHPBoost(0) != 0)
    {
        return false;
    }
    if (ManaTankMPBoost(ManaTank) != 1 || ManaTankMPBoost(ManaTankIV) != 4 || ManaTankMPBoost(193) != 0)
    {
        return false;
    }

    // Frame divisors
    const auto harlequinRepair = RepairKitFrameDivisor(FrameHarlequin);
    const auto valoredgeRepair = RepairKitFrameDivisor(FrameValoredge);
    const auto sharpshotRepair = RepairKitFrameDivisor(FrameSharpshot);
    const auto stormwakerRepair = RepairKitFrameDivisor(FrameStormwaker);
    if (!harlequinRepair || *harlequinRepair != 20 || !valoredgeRepair || *valoredgeRepair != 24 ||
        !sharpshotRepair || *sharpshotRepair != 18 || !stormwakerRepair || *stormwakerRepair != 16)
    {
        return false;
    }
    if (RepairKitFrameDivisor(0).has_value())
    {
        return false;
    }

    const auto harlequinMana = ManaTankFrameDivisor(FrameHarlequin);
    const auto stormwakerMana = ManaTankFrameDivisor(FrameStormwaker);
    if (!harlequinMana || *harlequinMana != 20 || !stormwakerMana || *stormwakerMana != 24)
    {
        return false;
    }
    if (ManaTankFrameDivisor(FrameValoredge).has_value() || ManaTankFrameDivisor(FrameSharpshot).has_value())
    {
        return false;
    }

    // Kit IV on Harlequin: 1000 + 1000*4/20 = 1200 (Lua comment example)
    if (ApplyAttachmentPoolBoost(1000, 4, 20) != 1200)
    {
        return false;
    }
    // Valoredge Kit IV: 1000 + 1000*4/24 = 1166
    if (ApplyAttachmentPoolBoost(1000, 4, 24) != 1166)
    {
        return false;
    }
    // Integer floor 999*1/20 = 49
    if (ApplyAttachmentPoolBoost(999, 1, 20) != 1048)
    {
        return false;
    }
    if (ApplyAttachmentPoolBoost(1000, 0, 20) != 1000 || ApplyAttachmentPoolBoost(1000, 4, 0) != 1000)
    {
        return false;
    }

    // Tier sum: Kit I + III = 1+3 = 4
    const auto hasIAndIII = [](const std::uint8_t id) -> bool {
        return id == AutoRepairKit || id == AutoRepairKitIII;
    };
    if (SumRepairKitTier(hasIAndIII) != 4)
    {
        return false;
    }
    const auto hasTankIV = [](const std::uint8_t id) -> bool { return id == ManaTankIV; };
    if (SumManaTankTier(hasTankIV) != 4)
    {
        return false;
    }
    const auto none = [](const std::uint8_t) -> bool { return false; };
    if (SumRepairKitTier(none) != 0 || SumManaTankTier(none) != 0)
    {
        return false;
    }

    return true;
}
} // namespace

auto runAutomatonRepairMana1587SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "automaton_repair_mana_1587 self-tests failed\n";
        return false;
    }
    return true;
}
