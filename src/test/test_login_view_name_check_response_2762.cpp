#include "test_login_view_name_check_response_2762.h"

#include "login/view_name_check_response.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "login view name check response 2762 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runLoginViewNameCheckResponse2762SelfTests() -> bool
{
    const auto deniedEnable = login::PlanViewNameCheckEnableResponse(loginHelpers::character_creation_gate::DENIED);
    const auto allowEnable  = login::PlanViewNameCheckEnableResponse(loginHelpers::character_creation_gate::ALLOW);
    const auto invalidName  = login::PlanViewNameCheckNameResponse(false);
    const auto validName    = login::PlanViewNameCheckNameResponse(true);

    return expect(deniedEnable.writeRegisterError && deniedEnable.returnFromRead && !deniedEnable.writeLobbyAck, "creation disabled") &&
           expect(!allowEnable.writeRegisterError && !allowEnable.returnFromRead && !allowEnable.writeLobbyAck, "creation enabled continues") &&
           expect(invalidName.logInvalidName && invalidName.writeNameUnavailableError && invalidName.returnFromRead && !invalidName.writeLobbyAck, "invalid name") &&
           expect(validName.setRequestedNewCharacterName && validName.writeLobbyAck && !validName.returnFromRead && !validName.writeNameUnavailableError, "valid name");
}
