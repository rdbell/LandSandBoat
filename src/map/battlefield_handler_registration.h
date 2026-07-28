#pragma once

#include "battlefield.h"

// Pure CBattlefieldHandler::RegisterBattlefield decision tree. Entity lookup,
// local-variable mutation, and InsertEntity / LoadBattlefield effects remain
// in the host.
namespace battlefieldhandlerhelpers
{

enum class RegistrationAction : uint8
{
    Return,
    InsertExisting,
    LoadNew,
};

struct RegistrationPlan
{
    RegistrationAction      action{ RegistrationAction::Return };
    BATTLEFIELD_RETURN_CODE returnCode{ BATTLEFIELD_RETURN_CODE_WAIT };
    bool                    resetEnterExisting{};

    auto operator==(const RegistrationPlan&) const -> bool = default;
};

// PlanRegistration mirrors RegisterBattlefield after the host has resolved
// membership and a matching existing battlefield. An existing battlefield
// only admits its registered flow when the character also carries the
// Battlefield status effect; otherwise LoadBattlefield owns the fallback.
inline auto PlanRegistration(const bool alreadyInBattlefield, const bool hasExistingBattlefield,
                             const bool hasBattlefieldStatus, const bool enterExistingRequested,
                             const bool existingBattlefieldInProgress, const bool registrationAreaMatches) -> RegistrationPlan
{
    if (alreadyInBattlefield)
    {
        return {};
    }

    if (!hasExistingBattlefield)
    {
        if (hasBattlefieldStatus)
        {
            return { RegistrationAction::Return, BATTLEFIELD_RETURN_CODE_REQS_NOT_MET, false };
        }
        if (enterExistingRequested)
        {
            return { RegistrationAction::Return, BATTLEFIELD_RETURN_CODE_REQS_NOT_MET, true };
        }
        return { RegistrationAction::LoadNew, BATTLEFIELD_RETURN_CODE_WAIT, false };
    }

    if (!hasBattlefieldStatus)
    {
        return { RegistrationAction::LoadNew, BATTLEFIELD_RETURN_CODE_WAIT, false };
    }

    if (existingBattlefieldInProgress)
    {
        return { RegistrationAction::Return, BATTLEFIELD_RETURN_CODE_LOCKED, true };
    }

    if (!registrationAreaMatches)
    {
        return { RegistrationAction::Return, BATTLEFIELD_RETURN_CODE_INCREMENT_REQUEST, true };
    }

    return { RegistrationAction::InsertExisting, BATTLEFIELD_RETURN_CODE_WAIT, true };
}

} // namespace battlefieldhandlerhelpers
