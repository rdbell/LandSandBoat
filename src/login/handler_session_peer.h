#pragma once

#include <string>
#include <utility>

// Pure handler_session construction policy. Kept separate from Asio so the
// endpoint failure behavior is pinned without a live socket.

struct handler_session_peer_plan
{
    bool        enableReuseAddress{ true };
    std::string ipAddress;
    bool        closeSocket{};
};

inline auto planHandlerSessionPeer(std::string remoteAddress, const bool endpointError) -> handler_session_peer_plan
{
    if (endpointError)
    {
        return { .ipAddress = "error", .closeSocket = true };
    }
    return { .ipAddress = std::move(remoteAddress), .closeSocket = false };
}
