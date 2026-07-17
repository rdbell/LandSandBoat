#include "test_login_handler_accept_2752.h"

#include "login/handler_accept.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "login handler accept 2752 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runLoginHandlerAccept2752SelfTests() -> bool
{
    return expect(handlerAcceptLoopAction(false) == handler_accept_loop_action::ACCEPT, "open scheduler accepts") &&
           expect(handlerAcceptLoopAction(true) == handler_accept_loop_action::STOP, "shutdown stops before accept") &&
           expect(handlerAcceptCompletionAction(false) == handler_accept_completion_action::START_SESSION_ON_WORKER, "successful accept starts session") &&
           expect(handlerAcceptCompletionAction(true) == handler_accept_completion_action::REPORT_ERROR, "failed accept reports error");
}
