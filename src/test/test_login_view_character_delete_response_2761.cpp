#include "test_login_view_character_delete_response_2761.h"

#include "login/view_character_delete_response.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "login view character delete response 2761 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runLoginViewCharacterDeleteResponse2761SelfTests() -> bool
{
    const auto deniedEnable = login::PlanViewCharacterDeleteEnableResponse(loginHelpers::character_deletion_gate::DENIED);
    const auto allowEnable  = login::PlanViewCharacterDeleteEnableResponse(loginHelpers::character_deletion_gate::ALLOW);
    const auto deniedOwner  = login::PlanViewCharacterDeleteOwnershipResponse(loginHelpers::character_delete_ownership_gate::DENIED, true);
    const auto allowNoData  = login::PlanViewCharacterDeleteOwnershipResponse(loginHelpers::character_delete_ownership_gate::PROCEED, false);
    const auto allowData    = login::PlanViewCharacterDeleteOwnershipResponse(loginHelpers::character_delete_ownership_gate::PROCEED, true);

    return expect(deniedEnable.writeLobbyError && deniedEnable.returnFromRead && !deniedEnable.writeLobbyAck, "delete disabled") &&
           expect(allowEnable.writeLobbyAck && allowEnable.logDeleteAttempt && allowEnable.runOwnershipQuery && !allowEnable.writeLobbyError, "delete enabled") &&
           expect(deniedOwner.logWrongAccount && deniedOwner.closeViewSocket && deniedOwner.returnFromRead && !deniedOwner.softDeleteCharacter, "ownership denied") &&
           expect(!allowNoData.clearCharFromDataSession && allowNoData.softDeleteCharacter && allowNoData.bumpDeleteKey, "ownership proceed no data peer") &&
           expect(allowData.clearCharFromDataSession && allowData.softDeleteCharacter && allowData.bumpDeleteKey, "ownership proceed with data peer");
}
