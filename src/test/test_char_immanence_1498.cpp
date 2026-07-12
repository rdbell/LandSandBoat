#include "test_char_immanence_1498.h"

#include "map/char_immanence_capacity.h"

#include <iostream>

namespace
{
using charimmanencehelpers::MapFamily;

auto Check() -> bool
{
    const auto stone = MapFamily(charimmanencehelpers::FamilyStone);
    if (!stone.applies || stone.isHelix || stone.skillchainElement != charimmanencehelpers::SCScission)
    {
        return false;
    }
    const auto geo = MapFamily(charimmanencehelpers::FamilyGeohelix);
    if (!geo.applies || !geo.isHelix || geo.skillchainElement != charimmanencehelpers::SCScission)
    {
        return false;
    }
    const auto nocto = MapFamily(charimmanencehelpers::FamilyNoctohelix);
    if (!nocto.applies || !nocto.isHelix || nocto.skillchainElement != charimmanencehelpers::SCCompression)
    {
        return false;
    }
    const auto lumino = MapFamily(charimmanencehelpers::FamilyLuminohelix);
    if (!lumino.applies || !lumino.isHelix || lumino.skillchainElement != charimmanencehelpers::SCTransfixion)
    {
        return false;
    }
    const auto other = MapFamily(0);
    if (other.applies || other.isHelix)
    {
        return false;
    }
    if (charimmanencehelpers::HelixDurationExtensionSeconds() != 2)
    {
        return false;
    }
    // Spot-check remaining elements.
    if (MapFamily(charimmanencehelpers::FamilyFire).skillchainElement != charimmanencehelpers::SCLiquefaction ||
        MapFamily(charimmanencehelpers::FamilyWater).skillchainElement != charimmanencehelpers::SCReverberation ||
        MapFamily(charimmanencehelpers::FamilyAero).skillchainElement != charimmanencehelpers::SCDetonation ||
        MapFamily(charimmanencehelpers::FamilyBlizzard).skillchainElement != charimmanencehelpers::SCInduration ||
        MapFamily(charimmanencehelpers::FamilyThunder).skillchainElement != charimmanencehelpers::SCImpaction)
    {
        return false;
    }
    return true;
}
} // namespace

auto runCharImmanence1498SelfTests() -> bool
{
    const bool ok = Check();
    if (!ok)
    {
        std::cerr << "char immanence 1498 self-test failed\n";
    }
    return ok;
}
