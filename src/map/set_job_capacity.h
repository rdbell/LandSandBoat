#pragma once

#include <cstdint>
#include <utility>

// Pure CBattleEntity::SetMJob / SetSJob validation (accept/reject; no storage).
// Parity: internal/setjob (slice 1665).
//
// Reference: src/map/entities/battle_entity.cpp
//   CBattleEntity::SetMJob (~1724–1733)
//   CBattleEntity::SetSJob (~1735–1744)
//
// Host retains m_mjob / m_sjob storage and ShowWarning logs; helpers only
// report whether a requested job type is acceptable to assign.
//
// MAX_JOBTYPE = 24. JOB_NON (0) is invalid for main but valid for sub (clear SJ).

namespace setjobhelpers
{

// Mirrors LSB MAX_JOBTYPE (exclusive upper bound).
constexpr std::uint8_t MaxJobType = 24;

// CanSetMainJob mirrors CBattleEntity::SetMJob acceptance:
//   mjob != 0 && mjob < MAX_JOBTYPE
constexpr auto CanSetMainJob(const std::uint8_t mjob) -> bool
{
    return mjob != 0 && mjob < MaxJobType;
}

// CanSetSubJob mirrors CBattleEntity::SetSJob acceptance:
//   sjob < MAX_JOBTYPE
// JOB_NON (0) is allowed (clears subjob).
constexpr auto CanSetSubJob(const std::uint8_t sjob) -> bool
{
    return sjob < MaxJobType;
}

// ResolveMainJob returns {accepted, value}. On reject, value is the rejected
// input for diagnostics; host must not write m_mjob.
constexpr auto ResolveMainJob(const std::uint8_t mjob) -> std::pair<bool, std::uint8_t>
{
    if (!CanSetMainJob(mjob))
    {
        return { false, mjob };
    }
    return { true, mjob };
}

// ResolveSubJob returns {accepted, value}. On reject, value is the rejected
// input for diagnostics; host must not write m_sjob.
constexpr auto ResolveSubJob(const std::uint8_t sjob) -> std::pair<bool, std::uint8_t>
{
    if (!CanSetSubJob(sjob))
    {
        return { false, sjob };
    }
    return { true, sjob };
}

} // namespace setjobhelpers
