/*
===========================================================================

  Copyright (c) 2025 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see http://www.gnu.org/licenses/

===========================================================================
*/

#include "0x058_recipe.h"

#include "entities/char_entity.h"
#include "packets/s2c/0x031_recipe.h"
#include "validation.h"

// Go host pure half: packetsystem.ValidateRecipe / ProcessRecipe /
// NewRecipeHandler (6466); plan mappacket.ClientRecipeResponsePlanFor.
auto GP_CLI_COMMAND_RECIPE::validate(MapSession* PSession, const CCharEntity* PChar) const -> PacketValidationResult
{
    return PacketValidator(PChar)
        .range("skill", this->skill, 0x01, 0x08) // Fishing 0x00 to Digging 0x0A. 0x00, 0x09, and 0x0A are not implemented
        .range("level", this->level, 0, 110)
        .range("Mode", this->Mode, // 1-5 but only 1-3 are implemented
               GP_CLI_COMMAND_RECIPE_MODE::RequestAvailableRankList,
               GP_CLI_COMMAND_RECIPE_MODE::RequestRecipeMaterials)
        .custom([&](PacketValidator& v)
                {
                    // clang-format off
                    switch (static_cast<GP_CLI_COMMAND_RECIPE_MODE>(this->Mode))
                    {
                        case GP_CLI_COMMAND_RECIPE_MODE::RequestAvailableRecipeList:
                            v.range("Param4", this->Param4, 0, 11)  // Skill 0 to 110
                            // The recipe pagenation values (Param1 & Param2) are used to walk the pages of recipes. The initial page will start with values 0 and 16.
                            // Each page beyond that will step through the recipes, incrementing the values by 16.
                                .multipleOf("Param1", this->Param1, 16);
                            break;
                        case GP_CLI_COMMAND_RECIPE_MODE::RequestRecipeMaterials:
                            v.range("Param4", this->Param4, 0, 11);
                            break;
                        default:
                            break;
                    }
                    // clang-format on
                });
}

void GP_CLI_COMMAND_RECIPE::process(MapSession* PSession, CCharEntity* PChar) const
{
    const auto responsePlan = recipehelpers::BuildResponsePlan(this->Mode, this->skill, this->level, this->Param1, this->Param3, this->Param4);
    if (responsePlan)
    {
        PChar->pushPacket<GP_SERV_COMMAND_RECIPE>(responsePlan->type, responsePlan->skill, responsePlan->level, responsePlan->skillRank, responsePlan->offset);
    }
}
