#pragma once

#include "common/cbasetypes.h"

#include <cstdint>
#include <fmt/format.h>
#include <string>

// Pure DATA 0xA2 character-selection residual gates and diagnostics extracted
// so native tests can pin policy without sockets/SQL. Packet shaping and key
// byte adjustment remain covered by earlier selection/key-packet tests.

namespace loginHelpers
{

// data_a2_session_account_gate is the pure outcome of session.accountID == 0.
enum class data_a2_session_account_gate : uint8_t
{
    OK,      // continue character/zone query path
    CORRUPT, // warn, error on data socket, close
};

// ClassifyDataA2SessionAccount mirrors session.accountID == 0 at the start of 0xA2.
inline auto ClassifyDataA2SessionAccount(const uint32 sessionAccountID) -> data_a2_session_account_gate
{
    return sessionAccountID == 0 ? data_a2_session_account_gate::CORRUPT : data_a2_session_account_gate::OK;
}

// FormatDataA2CorruptSessionWarning mirrors the ShowWarning when accountID is 0.
inline auto FormatDataA2CorruptSessionWarning(const std::string& ipAddress) -> std::string
{
    return fmt::format("data_session: login data corrupt (0xA2). Disconnecting client {}", ipAddress);
}

// data_a2_character_row_gate is the pure outcome of the zone/chars JOIN query.
enum class data_a2_character_row_gate : uint8_t
{
    FOUND,   // continue selection/admission path
    MISSING, // unable to connect to world server
};

// ClassifyDataA2CharacterRow mirrors rset success with at least one row.
inline auto ClassifyDataA2CharacterRow(const bool queryOk, const bool rowFound) -> data_a2_character_row_gate
{
    if (queryOk && rowFound)
    {
        return data_a2_character_row_gate::FOUND;
    }
    return data_a2_character_row_gate::MISSING;
}

// data_a2_existing_session_gate is the pure outcome of
// SELECT charid FROM accounts_sessions WHERE accid = ? LIMIT 1.
enum class data_a2_existing_session_gate : uint8_t
{
    NONE,               // no row — continue INSERT
    OTHER_CHARACTER,    // row exists for a different charid — continue INSERT
    ALREADY_LOGGED_IN,  // same charid — error CHARACTER_ALREADY_LOGGED_IN
};

// ClassifyDataA2ExistingSession mirrors the post-admission duplicate-session check.
// queryOk/rowFound describe the accounts_sessions lookup; sessionCharID is the
// charid from that row when found.
inline auto ClassifyDataA2ExistingSession(
    const bool   queryOk,
    const bool   rowFound,
    const uint32 sessionCharID,
    const uint32 requestedCharacterID) -> data_a2_existing_session_gate
{
    if (!queryOk || !rowFound)
    {
        return data_a2_existing_session_gate::NONE;
    }
    if (sessionCharID == requestedCharacterID)
    {
        return data_a2_existing_session_gate::ALREADY_LOGGED_IN;
    }
    return data_a2_existing_session_gate::OTHER_CHARACTER;
}

// DataA2AlreadyLoggedInKeyIncrement is added to session.incrementKeyValue when
// the same character is already present in accounts_sessions.
constexpr uint8 DataA2AlreadyLoggedInKeyIncrement = 1;

// ShouldUpdatePrevZone mirrors PrevZone == 0 (first login from character create).
inline auto ShouldUpdatePrevZone(const uint16 prevZone) -> bool
{
    return prevZone == 0;
}

// IsGMLevel mirrors gmlevel > 0 used for zone-cap and login-limit overrides.
inline auto IsGMLevel(const uint16 gmlevel) -> bool
{
    return gmlevel > 0;
}

// LoginLimitOK mirrors loginLimit == 0 || sessionCount < loginLimit || excepted.
inline auto LoginLimitOK(const uint8 loginLimit, const uint16 sessionCount, const bool excepted) -> bool
{
    return loginLimit == 0 || sessionCount < loginLimit || excepted;
}

// data_a2_admission_decision is the pure admission outcome after zone-cap and
// maintenance/login-limit policy. Zone capacity denies first; GM overrides
// maintenance and login limit but not zone capacity.
enum class data_a2_admission_decision : uint8_t
{
    ALLOWED,
    ZONE_AT_CAP,
    LOBBY_DENIED,
};

// DecideDataA2Admission mirrors data_session.cpp 0xA2 admission precedence.
inline auto DecideDataA2Admission(
    const bool   maintenanceMode,
    const uint8  loginLimit,
    const uint16 sessionCount,
    const bool   excepted,
    const bool   isGM,
    const bool   zoneAtCap) -> data_a2_admission_decision
{
    if (zoneAtCap)
    {
        return data_a2_admission_decision::ZONE_AT_CAP;
    }
    const auto limitOK = LoginLimitOK(loginLimit, sessionCount, excepted);
    if ((!maintenanceMode && limitOK) || isGM)
    {
        return data_a2_admission_decision::ALLOWED;
    }
    return data_a2_admission_decision::LOBBY_DENIED;
}

// FormatDataA2LoginLimitWarning mirrors the ShowWarning when loginLimitOK is false.
// Production logs this even when a GM later proceeds.
inline auto FormatDataA2LoginLimitWarning(
    const uint32      accountID,
    const std::string& ipAddress,
    const uint16      sessionCount,
    const uint8       loginLimit) -> std::string
{
    return fmt::format(
        "data_session: account {} attempting to login when {} already has {} active session(s), limit is {}",
        accountID,
        ipAddress,
        sessionCount,
        loginLimit);
}

// FormatDataA2ZoneCapWarning mirrors the ShowWarning when the destination zone is full.
// isGM is rendered as 0/1 to match production.
inline auto FormatDataA2ZoneCapWarning(const uint16 zoneID, const uint32 charid, const bool isGM) -> std::string
{
    return fmt::format("data_session: zone {} at player cap, denying charid {} (gm={})", zoneID, charid, isGM ? 1 : 0);
}

// data_a2_zone_cap_response_plan is the pure zone-at-cap control flow for DATA 0xA2.
// FormatDataA2ZoneCapWarning and WORLD_IS_FULL packet bytes remain host-owned.
struct data_a2_zone_cap_response_plan
{
    bool logWarning{};
    bool writeWorldFullError{}; // needs view session
    bool returnFromRead{};
};

// PlanDataA2ZoneCapResponse mirrors data_session's zone-cap branch before
// maint/limit admission (DecideDataA2Admission is called with zoneAtCap=false
// after this path). zoneAtCap false: no-op; with view peer: warn+error+return;
// without view peer: warn only (continue into maint/limit path).
inline auto PlanDataA2ZoneCapResponse(const bool zoneAtCap, const bool hasViewSession) -> data_a2_zone_cap_response_plan
{
    if (!zoneAtCap)
    {
        return {};
    }
    if (hasViewSession)
    {
        return { .logWarning = true, .writeWorldFullError = true, .returnFromRead = true };
    }
    return { .logWarning = true };
}

} // namespace loginHelpers
