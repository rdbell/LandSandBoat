#pragma once

// Pure CState::GetEntryTime / ResetEntryTime injects (slice 6330).
// Dual-wire of Go aistate.EntryTimeValue / ResetEntryTimeValue
// (internal/aistate/entry_time.go).

namespace statehelpers
{

// EntryTime returns the stored entry time point.
// Mirrors: return m_entryTime;
// Formula (slice 6330): entryTime
template <typename TimePoint>
inline auto EntryTime(const TimePoint entryTime) -> TimePoint
{
    return entryTime;
}

// ResetEntryTime returns the entry time written by ResetEntryTime.
// Mirrors: m_entryTime = timer::now(); with host-injected now.
// Formula (slice 6330): now
template <typename TimePoint>
inline auto ResetEntryTime(const TimePoint now) -> TimePoint
{
    return now;
}

} // namespace statehelpers
