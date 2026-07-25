#pragma once

#include <array>
#include <cstdint>
#include <vector>

namespace unequipremovedarmorlookhelpers
{
constexpr uint8_t SlotHead = 4;
constexpr uint8_t SlotFeet = 8;

struct EquippedModel
{
    bool     present{};
    uint16_t modelID{};
};

struct Plan
{
    uint8_t  slot{};
    uint16_t modelID{};
};

inline auto PlansFor(const uint32_t removeSlotLookID, const uint32_t removeSlotID,
                     const std::array<EquippedModel, 16>& equippedModels) -> std::vector<Plan>
{
    const uint32_t mask = removeSlotLookID > 0 ? removeSlotLookID : removeSlotID;
    std::vector<Plan> plans;
    for (uint8_t slot = SlotHead; slot <= SlotFeet; ++slot)
    {
        if ((mask & (1u << slot)) != 0)
        {
            const auto& equipped = equippedModels[slot];
            plans.push_back({ .slot = slot, .modelID = static_cast<uint16_t>(equipped.present ? equipped.modelID : 0) });
        }
    }
    return plans;
}
} // namespace unequipremovedarmorlookhelpers
