#include "test_monstrosity_instinct_mutation_7499.h"

#include "map/monstrosity.h"

#include <iostream>

auto runMonstrosityInstinctMutation7499SelfTests() -> bool
{
    using monstrosity::InstinctCatalog;
    using monstrosity::InstinctCatalogRow;
    using monstrosity::PlanInstinctLoadoutMutation;

    auto catalog = InstinctCatalog{
        { 10, { .monstrosityInstinctId = 10, .cost = 3 } },
        { 11, { .monstrosityInstinctId = 11, .cost = 11 } },
        { 12, { .monstrosityInstinctId = 12, .cost = 1 } },
    };

    std::array<uint16, 12> empty{};
    std::array<uint16, 12> acceptedRequests{};
    std::array<bool, 12>   acceptedUnlocked{};
    acceptedRequests[0] = 10;
    acceptedUnlocked[0] = true;
    const auto accepted = PlanInstinctLoadoutMutation(catalog, empty, acceptedRequests, acceptedUnlocked, 0);
    if (accepted.equipped[0] != 10 || !accepted.slotPlans[0].setRequestedInstinct || !accepted.slotPlans[0].addRequestedModifiers || accepted.slotPlans[0].restorePreviousLoadout || accepted.slotPlans[0].abortHandler)
    {
        std::cerr << "monstrosity instinct mutation: accepted request failed\n";
        return false;
    }

    auto original = empty;
    original[0] = 10;
    auto rejectedRequests = empty;
    rejectedRequests[0] = 0xFFFF;
    rejectedRequests[1] = 11;
    std::array<bool, 12> rejectedUnlocked{};
    rejectedUnlocked[1] = true;
    const auto rejected = PlanInstinctLoadoutMutation(catalog, original, rejectedRequests, rejectedUnlocked, 0);
    if (rejected.equipped != original || !rejected.slotPlans[0].removePreviousModifiers || !rejected.slotPlans[1].restorePreviousLoadout || rejected.slotPlans[1].addRequestedModifiers)
    {
        std::cerr << "monstrosity instinct mutation: rejected request should restore original loadout\n";
        return false;
    }

    auto lockedRequests = empty;
    lockedRequests[0] = 0xFFFF;
    lockedRequests[1] = 12;
    const auto locked = PlanInstinctLoadoutMutation(catalog, original, lockedRequests, std::array<bool, 12>{}, 99);
    if (locked.equipped[0] != 0 || !locked.slotPlans[0].removePreviousModifiers || !locked.slotPlans[1].abortHandler)
    {
        std::cerr << "monstrosity instinct mutation: prior updates should remain before a locked-request abort\n";
        return false;
    }

    return true;
}
