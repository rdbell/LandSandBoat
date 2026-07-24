#pragma once

#include "common/timer.h"

// Pure CDeathState::allowSendRaise / acceptRaise field plans (slice 6318).
// Dual-wire of Go aistate.AllowSendRaisePlan / AcceptRaisePlan
// (internal/aistate/death.go). Host applies returned fields to the death state.

namespace deathraise
{

// AllowSendRaisePlan: raiseTime = now + 12s, raiseSent = false.
struct AllowSendRaisePlan
{
    timer::time_point raiseTime;
    bool              raiseSent{ false };
};

inline auto allowSendRaisePlan(const timer::time_point now) -> AllowSendRaisePlan
{
    return AllowSendRaisePlan{ now + 12s, false };
}

// AcceptRaisePlan: raiseAcceptedTime = now, raiseAccepted = true, complete = true.
struct AcceptRaisePlan
{
    timer::time_point raiseAcceptedTime;
    bool              raiseAccepted{ true };
    bool              complete{ true };
};

inline auto acceptRaisePlan(const timer::time_point now) -> AcceptRaisePlan
{
    return AcceptRaisePlan{ now, true, true };
}

} // namespace deathraise
