#include "test_map_networking_login_character_reset_2735.h"

#include "map/map_networking_capacity.h"

#include <iostream>

auto runMapNetworkingLoginCharacterReset2735SelfTests() -> bool
{
    const bool ok = !mapnetworkinghelpers::ShouldResetCharacterForUnencryptedLogin(false) &&
                    mapnetworkinghelpers::ShouldResetCharacterForUnencryptedLogin(true);
    if (!ok)
    {
        std::cerr << "map networking login character reset 2735 self-test failed\n";
    }
    return ok;
}
