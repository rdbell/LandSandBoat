#include "test_monstrosity_instinct_slot_update_7488.h"

#include "map/monstrosity.h"

#include <iostream>

auto runMonstrosityInstinctSlotUpdate7488SelfTests() -> bool
{
    using monstrosity::PlanInstinctSlotUpdate;

    const auto unchanged = PlanInstinctSlotUpdate(0, false, false, false);
    if (unchanged.setRequestedInstinct || unchanged.removePreviousModifiers || unchanged.addRequestedModifiers || unchanged.restorePreviousLoadout || unchanged.abortHandler)
    {
        std::cerr << "monstrosity instinct slot update: zero request should be unchanged\n";
        return false;
    }

    const auto removed = PlanInstinctSlotUpdate(0xFFFF, false, false, false);
    if (!removed.setRequestedInstinct || removed.requestedInstinct != 0 || !removed.removePreviousModifiers || removed.addRequestedModifiers || removed.restorePreviousLoadout || removed.abortHandler)
    {
        std::cerr << "monstrosity instinct slot update: removal plan failed\n";
        return false;
    }

    const auto unknown = PlanInstinctSlotUpdate(123, false, true, false);
    if (unknown.setRequestedInstinct || unknown.removePreviousModifiers || unknown.addRequestedModifiers || unknown.restorePreviousLoadout || unknown.abortHandler)
    {
        std::cerr << "monstrosity instinct slot update: missing catalog entry should be ignored\n";
        return false;
    }

    const auto locked = PlanInstinctSlotUpdate(123, true, false, false);
    if (locked.setRequestedInstinct || locked.removePreviousModifiers || locked.addRequestedModifiers || locked.restorePreviousLoadout || !locked.abortHandler)
    {
        std::cerr << "monstrosity instinct slot update: locked instinct should abort\n";
        return false;
    }

    const auto accepted = PlanInstinctSlotUpdate(123, true, true, false);
    if (!accepted.setRequestedInstinct || accepted.requestedInstinct != 123 || accepted.removePreviousModifiers || !accepted.addRequestedModifiers || accepted.restorePreviousLoadout || accepted.abortHandler)
    {
        std::cerr << "monstrosity instinct slot update: accepted instinct plan failed\n";
        return false;
    }

    const auto rejected = PlanInstinctSlotUpdate(123, true, true, true);
    if (!rejected.setRequestedInstinct || rejected.requestedInstinct != 123 || rejected.removePreviousModifiers || rejected.addRequestedModifiers || !rejected.restorePreviousLoadout || rejected.abortHandler)
    {
        std::cerr << "monstrosity instinct slot update: rejected instinct plan failed\n";
        return false;
    }

    return true;
}
