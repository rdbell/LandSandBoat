#include "test_login_auth_handshake_2743.h"

#include "login/auth_handshake.h"

#include <iostream>

auto runLoginAuthHandshake2743SelfTests() -> bool
{
    using namespace loginHelpers;

    bool ok = true;
    ok      = ClassifyAuthHandshakeStart(false) == auth_handshake_start_action::NOOP && ok;
    ok      = ClassifyAuthHandshakeStart(true) == auth_handshake_start_action::START_HANDSHAKE && ok;
    ok      = ClassifyAuthHandshakeCompletion(false, "ignored") == auth_handshake_completion_action::START_READ && ok;
    ok      = ClassifyAuthHandshakeCompletion(true, "certificate verify failed") == auth_handshake_completion_action::CLOSE && ok;
    ok      = ClassifyAuthHandshakeCompletion(true, "Error: wrong version number (SSL routines)") ==
                  auth_handshake_completion_action::CLOSE_WITH_LEGACY_XILOADER_HINT &&
              ok;
    ok      = !IsLegacyXiloaderTLSFailure("wrong version number") && ok;

    if (!ok)
    {
        std::cerr << "login auth handshake 2743 self-test failed\n";
    }
    return ok;
}
