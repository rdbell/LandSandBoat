#include "test_data_session_hash_2672.h"

#include "login/data_session_hash.h"

#include <array>
#include <iostream>

auto runDataSessionHash2672SelfTests() -> bool
{
    struct case_t
    {
        const char* packetHash;
        const char* storedHash;
        const char* expected;
    };

    constexpr auto cases = std::array{
        case_t{ "packet", "stored", "packet" },
        case_t{ "", "stored", "stored" },
        case_t{ "packet", "", "packet" },
        case_t{ "", "", nullptr },
    };

    for (const auto& test : cases)
    {
        const auto result = loginHelpers::ResolveDataSessionHash(test.packetHash, test.storedHash);
        if ((test.expected == nullptr && result.has_value()) ||
            (test.expected != nullptr && (!result || *result != test.expected)))
        {
            std::cerr << "data session hash 2672 self-test failed\n";
            return false;
        }
    }
    return true;
}
