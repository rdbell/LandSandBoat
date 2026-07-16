#include "test_map_socket_receive_2654.h"

#include "map/map_socket_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "map socket receive 2654 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runMapSocketReceive2654SelfTests() -> bool
{
    bool ok = true;

    auto plan = mapsockethelpers::PlanReceive(false, false, false, true);
    ok        = expect(!plan.reportError && !plan.reportEmpty && plan.dispatch && plan.receiveAgain, "nonempty successful datagram dispatches and continues") && ok;

    plan = mapsockethelpers::PlanReceive(false, true, false, true);
    ok   = expect(!plan.reportError && plan.reportEmpty && !plan.dispatch && plan.receiveAgain, "empty datagram reports and continues") && ok;

    plan = mapsockethelpers::PlanReceive(true, true, false, true);
    ok   = expect(plan.reportError && !plan.reportEmpty && !plan.dispatch && plan.receiveAgain, "error takes precedence over empty") && ok;

    plan = mapsockethelpers::PlanReceive(false, false, true, true);
    ok   = expect(plan.dispatch && !plan.receiveAgain, "close request stops receive loop after dispatch") && ok;

    plan = mapsockethelpers::PlanReceive(false, false, false, false);
    ok   = expect(plan.dispatch && !plan.receiveAgain, "closed socket stops receive loop after dispatch") && ok;

    return ok;
}
