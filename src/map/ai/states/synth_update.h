#pragma once

// Pure CSynthState::Update / SynthReady gates (slice 6315).
// Dual-wire of Go aistate.SynthReady / SynthUpdatePure
// (internal/aistate/synth.go). Host owns doSynthCriticalFail / sendSynthDone
// and finish-time mutation.

namespace synthupdate
{

// isReady mirrors CSynthState::SynthReady:
//   return m_synthFinishTime < 0ms && isAlive;
// remainingNegative — host m_synthFinishTime < 0ms (strict <)
// isAlive — host m_PEntity->isAlive()
constexpr auto isReady(const bool remainingNegative, const bool isAlive) -> bool
{
    return remainingNegative && isAlive;
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

} // namespace synthupdate
