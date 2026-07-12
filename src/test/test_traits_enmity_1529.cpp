#include "test_traits_enmity_1529.h"

#include "map/traits_enmity_capacity.h"

#include <iostream>
#include <vector>

namespace
{
using traitsenmityhelpers::ExistingTrait;
using traitsenmityhelpers::ResolveInRangeEnmitySource;
using traitsenmityhelpers::ResolveTraitAdd;
using traitsenmityhelpers::ShouldGenerateCureEnmity;
using traitsenmityhelpers::ShouldGenerateInRangeEnmity;
using traitsenmityhelpers::ShouldUpdateCureEnmity;
using traitsenmityhelpers::ShouldUpdateInRangeEnmity;
using traitsenmityhelpers::TraitCandidate;

auto Check() -> bool
{
    TraitCandidate c{ 1, 10, 1, 5, 0 };
    auto           emptyMerit = [](std::uint16_t) -> std::uint8_t { return 0; };
    {
        const auto d = ResolveTraitAdd(9, c, {}, false, emptyMerit, emptyMerit);
        if (d.eligible || d.shouldAdd)
        {
            return false;
        }
    }
    {
        const auto d = ResolveTraitAdd(10, c, {}, false, emptyMerit, emptyMerit);
        if (!d.eligible || !d.shouldAdd || d.delExistingIndex != -1)
        {
            return false;
        }
    }
    {
        std::vector<ExistingTrait> existing{ { 7, 1, 10, 0 } };
        TraitCandidate             up{ 7, 1, 2, 10, 0 };
        const auto                 d = ResolveTraitAdd(50, up, existing, false, emptyMerit, emptyMerit);
        if (!d.shouldAdd || d.delExistingIndex != 0)
        {
            return false;
        }
    }
    {
        std::vector<ExistingTrait> existing{ { 7, 3, 10, 0 } };
        TraitCandidate             up{ 7, 1, 2, 10, 0 };
        const auto                 d = ResolveTraitAdd(50, up, existing, false, emptyMerit, emptyMerit);
        if (d.shouldAdd)
        {
            return false;
        }
    }

    if (!ShouldGenerateCureEnmity(true, true) || ShouldGenerateCureEnmity(false, true))
    {
        return false;
    }
    if (!ShouldUpdateCureEnmity(true, 1, true) || ShouldUpdateCureEnmity(true, 0, true))
    {
        return false;
    }
    if (!ShouldGenerateInRangeEnmity(true) || ShouldGenerateInRangeEnmity(false))
    {
        return false;
    }
    {
        bool useS = false;
        bool useM = false;
        ResolveInRangeEnmitySource(true, false, false, useS, useM);
        if (!useS || useM)
        {
            return false;
        }
        ResolveInRangeEnmitySource(false, true, true, useS, useM);
        if (useS || !useM)
        {
            return false;
        }
    }
    if (!ShouldUpdateInRangeEnmity(5, true) || ShouldUpdateInRangeEnmity(0, true))
    {
        return false;
    }
    return true;
}
} // namespace

auto runTraitsEnmity1529SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "traits_enmity_1529 self-tests failed\n";
        return false;
    }
    return true;
}
