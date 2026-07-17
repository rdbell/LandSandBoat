#include "test_login_view_acquire_player_response_2760.h"

#include "login/view_acquire_player_response.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "login view acquire player response 2760 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runLoginViewAcquirePlayerResponse2760SelfTests() -> bool
{
    const auto present = login::PlanViewAcquirePlayerResponse(loginHelpers::data_session_presence_gate::PRESENT);
    const auto missing = login::PlanViewAcquirePlayerResponse(loginHelpers::data_session_presence_gate::MISSING);

    return expect(present.writeAcquireNotify && !present.writeLobbyError && !present.returnFromRead, "present notifies data peer") &&
           expect(!missing.writeAcquireNotify && missing.writeLobbyError && missing.returnFromRead, "missing writes error and returns");
}
