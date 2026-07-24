#pragma once

// Pure CTriggerState::Update gates (slice 6319).
// Dual-wire of Go aistate.TriggerShouldOpenDoor / TriggerDoorCloseReady
// (internal/aistate/trigger.go). Host owns animation/updatemask mutation.

namespace triggerupdate
{

// shouldOpenDoor mirrors the first-tick door open predicate:
//   PChar && door && animation == ANIMATION_CLOSE_DOOR
// hasCharTarget — dynamic_cast<CCharEntity*>(GetTarget()) != nullptr
// door — ctor door flag
// isCloseDoorAnim — m_PEntity->animation == ANIMATION_CLOSE_DOOR
constexpr auto shouldOpenDoor(const bool hasCharTarget, const bool door, const bool isCloseDoorAnim) -> bool
{
    return hasCharTarget && door && isCloseDoorAnim;
}

// shouldCloseDoor mirrors the re-close gate after open:
//   close && tick > entry + 7s
// close — host close flag (true in the else-if close branch)
// tickAfterEntryPlus7s — host tick > GetEntryTime() + 7s (strict >)
constexpr auto shouldCloseDoor(const bool close, const bool tickAfterEntryPlus7s) -> bool
{
    return close && tickAfterEntryPlus7s;
}

// shouldExitImmediate reports whether Update exits immediately when completed
// and not pending re-close:
//   else { return true; }  // !close after completed
constexpr auto shouldExitImmediate(const bool completed, const bool close) -> bool
{
    return completed && !close;
}

} // namespace triggerupdate
