#include "test_login_handler_session_2581.h"

#include <iostream>

#include "login/handler_session.h"

namespace
{

auto expect(bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "login handler session self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runLoginHandlerSession2581SelfTests() -> bool
{
    return expect(HandlerSessionBufferSize == 4096, "buffer size") &&
           expect(HandlerSessionKeepaliveIdleSeconds == 300, "keepalive idle seconds") &&
           expect(HandlerSessionKeepaliveIntervalSeconds == 300, "keepalive interval seconds") &&
           expect(HandlerSessionKeepaliveProbeCount == 10, "keepalive probe count") &&
           expect(handlerSessionStartAction(false) == handler_session_start_action::NOOP, "closed start is noop") &&
           expect(handlerSessionStartAction(true) == handler_session_start_action::CONFIGURE_KEEPALIVE_AND_READ, "open start configures and reads") &&
           expect(handlerSessionReadCompletionAction(false) == handler_session_read_completion_action::DISPATCH_READ, "successful read dispatches") &&
           expect(handlerSessionReadCompletionAction(true) == handler_session_read_completion_action::HANDLE_ERROR, "failed read handles error") &&
           expect(handlerSessionWriteCompletionAction(false) == handler_session_write_completion_action::DISPATCH_WRITE, "successful write dispatches") &&
           expect(handlerSessionWriteCompletionAction(true) == handler_session_write_completion_action::REPORT_ERROR, "failed write reports error");
}
