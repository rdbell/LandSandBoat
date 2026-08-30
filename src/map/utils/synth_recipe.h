#pragma once

#include "common/cbasetypes.h"
#include "entities/battle_entity.h"
#include "enums/key_items.h"

#include <string>

namespace synthutils
{

struct SynthRecipe
{
    uint32  ID{};
    uint8   Desynth{};
    KeyItem RequiredKeyItem{};
    uint8   Wood{};
    uint8   Smith{};
    uint8   Gold{};
    uint8   Cloth{};
    uint8   Leather{};
    uint8   Bone{};
    uint8   Alchemy{};
    uint8   Cook{};
    uint16  Crystal{};
    uint16  HQCrystal{};
    uint16  Ingredient1{};
    uint16  Ingredient2{};
    uint16  Ingredient3{};
    uint16  Ingredient4{};
    uint16  Ingredient5{};
    uint16  Ingredient6{};
    uint16  Ingredient7{};
    uint16  Ingredient8{};
    uint16  Result{};
    uint16  ResultHQ1{};
    uint16  ResultHQ2{};
    uint16  ResultHQ3{};
    uint8   ResultQty{};
    uint8   ResultHQ1Qty{};
    uint8   ResultHQ2Qty{};
    uint8   ResultHQ3Qty{};

    std::string ResultName;
    std::string ContentTag;

    uint16 getSkillValue(SKILLTYPE type) const
    {
        switch (type)
        {
            case SKILL_WOODWORKING:
                return Wood;
            case SKILL_SMITHING:
                return Smith;
            case SKILL_GOLDSMITHING:
                return Gold;
            case SKILL_CLOTHCRAFT:
                return Cloth;
            case SKILL_LEATHERCRAFT:
                return Leather;
            case SKILL_BONECRAFT:
                return Bone;
            case SKILL_ALCHEMY:
                return Alchemy;
            case SKILL_COOKING:
                return Cook;
            default:
                return 0;
        }
    }
};

} // namespace synthutils
