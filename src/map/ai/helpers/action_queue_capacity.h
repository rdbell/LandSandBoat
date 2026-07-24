#pragma once

// Pure CAIActionQueue::isEmpty inject (slice 6331).
// Dual-wire of Go actionqueue.IsBothQueuesEmpty
// (internal/actionqueue/is_empty.go).

namespace actionqueuehelpers
{

// IsBothQueuesEmpty reports whether both the state-gated action queue and
// the timer queue are empty.
// Mirrors: return actionQueue.empty() && timerQueue.empty();
// Formula (slice 6331): actionEmpty && timerEmpty
inline auto IsBothQueuesEmpty(const bool actionEmpty, const bool timerEmpty) -> bool
{
    return actionEmpty && timerEmpty;
}

// ActionDueStrict reports whether checkAction may drain a queued action at
// tick for the host-injected due deadline (start_time + delay).
// Mirrors: tick > topaction.start_time + topaction.delay
// Formula (slice 6332): tick > dueAt
// Dual-wire of Go actionqueue.ActionDueStrict (action_due.go).
// Call sites: CAIActionQueue::checkAction timer and action while admissions.
template <typename TimePoint>
inline auto ActionDueStrict(const TimePoint tick, const TimePoint dueAt) -> bool
{
    return tick > dueAt;
}

// ShouldRouteToActionQueue reports whether pushAction uses the state-gated
// action queue (true) versus the timer queue (false).
// Mirrors: if (action.checkState) actionQueue else timerQueue
// Formula (slice 6333): checkState
// Dual-wire of Go actionqueue.ShouldRouteToActionQueue (push_state_gate.go).
// Call site: CAIActionQueue::pushAction.
inline auto ShouldRouteToActionQueue(const bool checkState) -> bool
{
    return checkState;
}

// ActionStateGateAllows reports whether a due action may drain under the
// host-injected checkState and CanChangeState flags.
// Mirrors: !topaction.checkState || PEntity->PAI->CanChangeState()
// Formula (slice 6333): !checkState || canChangeState
// Dual-wire of Go actionqueue.ActionStateGateAllows (push_state_gate.go).
// Call site: CAIActionQueue::checkAction action-queue while admission.
inline auto ActionStateGateAllows(const bool checkState, const bool canChangeState) -> bool
{
    return !checkState || canChangeState;
}

} // namespace actionqueuehelpers
