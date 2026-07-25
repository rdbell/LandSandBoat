#pragma once

#include <cstdint>

namespace loadjobchangegearhelpers
{
struct Facts
{
    uint16_t savedItemID{};
    bool     candidateIsEquipment{};
    uint16_t candidateItemID{};
    bool     sameAsAdjacentEquip{};
};

struct Plan
{
    bool equipCandidate{};
};

constexpr Plan PlanFor(Facts facts)
{
    return {
        .equipCandidate = facts.savedItemID != 0 && facts.candidateIsEquipment &&
                          facts.candidateItemID == facts.savedItemID && !facts.sameAsAdjacentEquip,
    };
}
} // namespace loadjobchangegearhelpers
