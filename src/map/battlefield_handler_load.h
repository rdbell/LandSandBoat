#pragma once

#include "battlefield.h"

// Pure CBattlefieldHandler::LoadBattlefield admission and record-query
// decisions. Database access, CBattlefield construction, status effects, Lua
// callbacks, and entity insertion remain host-owned.
namespace battlefieldhandlerhelpers
{

enum class LoadAction : uint8
{
    Return,
    Create,
};

struct LoadAdmissionPlan
{
    LoadAction              action{ LoadAction::Return };
    BATTLEFIELD_RETURN_CODE returnCode{ BATTLEFIELD_RETURN_CODE_WAIT };

    auto operator==(const LoadAdmissionPlan&) const -> bool = default;
};

// PlanLoadAdmission mirrors LoadBattlefield before its bcnm_records query.
inline auto PlanLoadAdmission(const bool alreadyInBattlefield, const bool atBattlefieldCapacity,
                              const bool registrationAreaOccupied, const bool cutsceneOnlyRegistration) -> LoadAdmissionPlan
{
    if (alreadyInBattlefield || atBattlefieldCapacity)
    {
        return {};
    }
    if (registrationAreaOccupied)
    {
        return { LoadAction::Return, BATTLEFIELD_RETURN_CODE_INCREMENT_REQUEST };
    }
    if (cutsceneOnlyRegistration)
    {
        return { LoadAction::Return, BATTLEFIELD_RETURN_CODE_CUTSCENE };
    }
    return { LoadAction::Create, BATTLEFIELD_RETURN_CODE_WAIT };
}

// PlanLoadRecordResult mirrors the bcnm_records query gate after admission.
inline auto PlanLoadRecordResult(const bool recordAvailable) -> BATTLEFIELD_RETURN_CODE
{
    return recordAvailable ? BATTLEFIELD_RETURN_CODE_CUTSCENE : BATTLEFIELD_RETURN_CODE_REQS_NOT_MET;
}

} // namespace battlefieldhandlerhelpers
