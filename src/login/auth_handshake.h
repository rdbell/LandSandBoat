#pragma once

#include <cstdint>
#include <string_view>

// Pure AUTH TLS handshake policy extracted from auth_session::start so socket
// ownership and error logging stay in the host.
namespace loginHelpers
{

enum class auth_handshake_start_action : uint8_t
{
    NOOP,
    START_HANDSHAKE,
};

enum class auth_handshake_completion_action : uint8_t
{
    START_READ,
    CLOSE,
    CLOSE_WITH_LEGACY_XILOADER_HINT,
};

inline constexpr std::string_view LegacyXiloaderTLSFailure = "wrong version number (SSL routines)";

inline auto ClassifyAuthHandshakeStart(const bool socketOpen) -> auth_handshake_start_action
{
    return socketOpen ? auth_handshake_start_action::START_HANDSHAKE : auth_handshake_start_action::NOOP;
}

inline auto IsLegacyXiloaderTLSFailure(const std::string_view errorText) -> bool
{
    return errorText.find(LegacyXiloaderTLSFailure) != std::string_view::npos;
}

inline auto ClassifyAuthHandshakeCompletion(const bool hasError, const std::string_view errorText) -> auth_handshake_completion_action
{
    if (!hasError)
    {
        return auth_handshake_completion_action::START_READ;
    }
    return IsLegacyXiloaderTLSFailure(errorText) ? auth_handshake_completion_action::CLOSE_WITH_LEGACY_XILOADER_HINT : auth_handshake_completion_action::CLOSE;
}

} // namespace loginHelpers
