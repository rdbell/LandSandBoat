#pragma once

// Pure receive-loop decision from MapSocket::receive. Socket I/O, endpoint
// conversion, logging, and callback invocation remain in the host.
namespace mapsockethelpers
{

struct ReceivePlan
{
    bool reportError;
    bool reportEmpty;
    bool dispatch;
    bool receiveAgain;
};

inline auto PlanReceive(const bool hasError, const bool emptyBuffer, const bool closeRequested, const bool socketOpen) -> ReceivePlan
{
    return {
        .reportError  = hasError,
        .reportEmpty  = !hasError && emptyBuffer,
        .dispatch     = !hasError && !emptyBuffer,
        .receiveAgain = !closeRequested && socketOpen,
    };
}

} // namespace mapsockethelpers
