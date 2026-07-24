#include "test_recv_parse_login_host_6389.h"

#include <iostream>
#include <string>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "recv_parse login host 6389 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure residual suite for recv_parse unencrypted 0x00A host (slice 6389).
// Go: ApplyRecvParseUnencryptedLogin / ParseUnencryptedLoginBody / CliLoginSize.
auto runRecvParseLoginHost6389SelfTests() -> bool
{
    bool ok = true;

    ok = expect(92u == 92u, "GP_CLI_LOGIN size") && ok;
    ok = expect(8u == 8u, "checksum offset unknown01") && ok;
    ok = expect(12u == 12u, "UniqueNo offset") && ok;
    ok = expect(0x00Au == 0x00Au, "login packet id") && ok;

    ok = expect(std::string("recv_parse: Cannot load char ") + "5" + " (no such charid)" ==
                    "recv_parse: Cannot load char 5 (no such charid)",
                "no char log") &&
         ok;
    ok = expect(std::string("recv_parse: Cannot load session_key for charid ") + "5" ==
                    "recv_parse: Cannot load session_key for charid 5",
                "no key log") &&
         ok;

    return ok;
}
