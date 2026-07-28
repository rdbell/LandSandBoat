#include "test_monstrosity_descriptor_update_7487.h"

#include "map/monstrosity.h"

#include <iostream>

auto runMonstrosityDescriptorUpdate7487SelfTests() -> bool
{
    using monstrosity::PlanDescriptorUpdate;

    const auto first = PlanDescriptorUpdate(false, false, true, true, 12, 34);
    if (!first.setNamePrefix1 || first.namePrefix1 != 12 || first.setNamePrefix2 || !first.writeData || !first.sendFullUpdate)
    {
        std::cerr << "monstrosity descriptor update: Descriptor1 precedence failed\n";
        return false;
    }

    const auto second = PlanDescriptorUpdate(false, false, false, true, 12, 34);
    if (second.setNamePrefix1 || !second.setNamePrefix2 || second.namePrefix2 != 34)
    {
        std::cerr << "monstrosity descriptor update: Descriptor2 update failed\n";
        return false;
    }

    const auto suppressed = PlanDescriptorUpdate(true, true, true, true, 12, 34);
    if (suppressed.setNamePrefix1 || suppressed.setNamePrefix2 || !suppressed.writeData || !suppressed.sendFullUpdate)
    {
        std::cerr << "monstrosity descriptor update: higher-priority suppression failed\n";
        return false;
    }

    return true;
}
