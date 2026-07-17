#include "test_login_view_character_create_response_2763.h"

#include "login/view_character_create_response.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "login view character create response 2763 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runLoginViewCharacterCreateResponse2763SelfTests() -> bool
{
    const auto failed   = login::PlanViewCharacterCreateResponse(false, true);
    const auto okNoData = login::PlanViewCharacterCreateResponse(true, false);
    const auto okData   = login::PlanViewCharacterCreateResponse(true, true);

    return expect(failed.closeViewSocket && failed.returnFromRead && !failed.writeLobbyAck && !failed.addCharIntoDataSession, "create failed") &&
           expect(!okNoData.closeViewSocket && !okNoData.addCharIntoDataSession && okNoData.setJustCreatedNewChar && okNoData.logCharacterCreated && okNoData.writeLobbyAck, "create ok no data peer") &&
           expect(okData.addCharIntoDataSession && okData.setJustCreatedNewChar && okData.logCharacterCreated && okData.writeLobbyAck && !okData.returnFromRead, "create ok with data peer");
}
