#pragma once

#include <cstdint>

// Pure handler accept-loop decisions, separated from Asio and Scheduler hosts.

enum class handler_accept_loop_action : std::uint8_t
{
    ACCEPT,
    STOP,
};

enum class handler_accept_completion_action : std::uint8_t
{
    START_SESSION_ON_WORKER,
    REPORT_ERROR,
};

constexpr auto handlerAcceptLoopAction(const bool closeRequested) -> handler_accept_loop_action
{
    return closeRequested ? handler_accept_loop_action::STOP : handler_accept_loop_action::ACCEPT;
}

constexpr auto handlerAcceptCompletionAction(const bool hasError) -> handler_accept_completion_action
{
    return hasError ? handler_accept_completion_action::REPORT_ERROR : handler_accept_completion_action::START_SESSION_ON_WORKER;
}
