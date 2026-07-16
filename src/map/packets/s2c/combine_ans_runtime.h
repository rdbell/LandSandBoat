#pragma once

#include <array>

#include "0x06f_combine_ans.h"

namespace combineanshelpers
{
struct IngredientFacts { uint16 itemID{}; bool broken{}; };
struct Facts {
    bool hasActiveSynth{};
    std::array<uint8, 8> requiredSkills{};
    uint16 crystalID{};
    std::array<IngredientFacts, 8> ingredients{};
};

inline auto ApplyActiveSynth(GP_SERV_COMMAND_COMBINE_ANS::PacketData& packet, const Facts& facts) -> void
{
    if (!facts.hasActiveSynth) return;
    for (uint8 i = 0; i < 4; ++i)
    {
        uint8 skillValue = 0;
        for (uint8 skillID = 49; skillID < 57; ++skillID)
        {
            if (skillID == packet.UpKind[0] || skillID == packet.UpKind[1] || skillID == packet.UpKind[2] || skillID == packet.UpKind[3]) continue;
            const uint8 required = facts.requiredSkills[skillID - 49];
            if (required > skillValue) { skillValue = required; packet.UpKind[i] = static_cast<int8>(skillID); }
        }
    }
    packet.CrystalNo = facts.crystalID;
    for (uint8 i = 0; i < 8; ++i) { packet.MaterialNo[i] = facts.ingredients[i].itemID; if (facts.ingredients[i].broken) packet.BreakNo[i] = facts.ingredients[i].itemID; }
}
} // namespace combineanshelpers
