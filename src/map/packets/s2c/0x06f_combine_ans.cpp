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

#include "0x06f_combine_ans.h"
#include "combine_ans_runtime.h"

#include "entities/char_entity.h"
#include "items/craft_state.h"
#include "items/transactions/synth.h"

GP_SERV_COMMAND_COMBINE_ANS::GP_SERV_COMMAND_COMBINE_ANS(const CCharEntity* PChar, const SynthesisResult result, const CCraftState::Result item)
{
    auto& packet = this->data();

    packet.Result = result;

    if (item.itemId != 0)
    {
        packet.Count  = item.qty;
        packet.ItemNo = item.itemId;
    }

    auto facts = combineanshelpers::Facts{};
    facts.hasActiveSynth = PChar->activeTransaction<SynthTransaction>();
    if (facts.hasActiveSynth)
    {
        const auto& craftState = PChar->craftState();
        facts.crystalID = craftState.crystalItemId();
        for (uint8 i = 0; i < SynthMaxIngredients; ++i)
        {
            facts.requiredSkills[i] = craftState.skillRequired(i);
            facts.ingredients[i] = { .itemID = craftState.ingredientItemId(i), .broken = craftState.isBroken(i) };
        }
    }
    combineanshelpers::ApplyActiveSynth(packet, facts);
}
