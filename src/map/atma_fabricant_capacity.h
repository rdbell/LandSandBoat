#pragma once

#include <cstdint>

// Pure Atma Fabricant onTrigger policy from
// scripts/globals/abyssea/atma_fabricant.lua. The entity host supplies the
// player's Visitant status and applies the selected message or event.
namespace atmafabricant
{

enum class TriggerAction : uint8_t
{
    NoVisitantMessage,
    StartEvent,
};

struct TriggerPlan
{
    TriggerAction action;
    uint16_t      eventID;
    uint16_t      messageID;
};

constexpr uint16_t EventID = 2182;

// Mirrors:
//   if not player:hasStatusEffect(xi.effect.VISITANT) then
//       player:messageSpecial(ID.text.NO_VISITANT_STATUS)
//   else
//       player:startEvent(2182)
inline auto PlanTrigger(const bool hasVisitant, const uint16_t messageID) -> TriggerPlan
{
    if (!hasVisitant)
    {
        return {TriggerAction::NoVisitantMessage, 0, messageID};
    }

    return {TriggerAction::StartEvent, EventID, 0};
}

} // namespace atmafabricant
