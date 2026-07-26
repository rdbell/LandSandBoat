#pragma once

#include "modifier.h"

// Pure CSynthState::Update / SynthReady gates (slice 6315).
// Dual-wire of Go aistate.SynthReady / SynthUpdatePure
// (internal/aistate/synth.go). Host owns doSynthCriticalFail / sendSynthDone
// and finish-time mutation.

namespace synthupdate
{

enum class UpdateAction
{
    Countdown,
    CriticalFail,
    Done,
};

// isReady mirrors CSynthState::SynthReady:
//   return m_synthFinishTime < 0ms && isAlive;
// remainingNegative — host m_synthFinishTime < 0ms (strict <)
// isAlive — host m_PEntity->isAlive()
constexpr auto isReady(const bool remainingNegative, const bool isAlive) -> bool
{
    return remainingNegative && isAlive;
}

// speedModID mirrors CSynthState's constructor skill switch. Non-craft skills
// return NONE and must not consume a synthesis-speed modifier.
constexpr auto speedModID(const uint8 skillID) -> Mod
{
    switch (skillID)
    {
        case 49: return Mod::SYNTH_SPEED_WOODWORKING;
        case 50: return Mod::SYNTH_SPEED_SMITHING;
        case 51: return Mod::SYNTH_SPEED_GOLDSMITHING;
        case 52: return Mod::SYNTH_SPEED_CLOTHCRAFT;
        case 53: return Mod::SYNTH_SPEED_LEATHERCRAFT;
        case 54: return Mod::SYNTH_SPEED_BONECRAFT;
        case 55: return Mod::SYNTH_SPEED_ALCHEMY;
        case 56: return Mod::SYNTH_SPEED_COOKING;
        default: return Mod::NONE;
    }
}

// shouldCriticalFailExit reports whether Update should critical-fail and exit.
// Mirrors: if (m_PEntity->isDead()) { doSynthCriticalFail; return true; }
constexpr auto shouldCriticalFailExit(const bool isDead) -> bool
{
    return isDead;
}

// shouldFinishExit reports whether Update should sendSynthDone and exit after
// the dead check has failed (entity not dead for this path).
// Mirrors: if (SynthReady()) { sendSynthDone; return true; }
// Equivalent to isReady(remainingNegative, isAlive).
constexpr auto shouldFinishExit(const bool remainingNegative, const bool isAlive) -> bool
{
    return isReady(remainingNegative, isAlive);
}

// updateAction selects CSynthState::Update's terminal side effect. Critical
// failure has precedence over a simultaneously ready synthesis session.
constexpr auto updateAction(const bool isDead, const bool isReady) -> UpdateAction
{
    if (isDead)
    {
        return UpdateAction::CriticalFail;
    }
    if (isReady)
    {
        return UpdateAction::Done;
    }
    return UpdateAction::Countdown;
}

} // namespace synthupdate
