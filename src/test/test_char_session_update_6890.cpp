#include "test_char_session_update_6890.h"

#include "map/char_session_update.h"

#include <iostream>

auto runCharSessionUpdate6890SelfTests() -> bool
{
    const bool ok = charsessionupdatehelpers::MakePlan(0x701, 0x7F000001, 54230, 12345) == charsessionupdatehelpers::Plan{
                                                                                   .targetId             = 0x701,
                                                                                   .serverAddress        = 0x7F000001,
                                                                                   .clientPort           = 54230,
                                                                                   .characterId          = 12345,
                                                                               };
    if (!ok)
    {
        std::cerr << "character session update 6890 self-test failed\n";
    }
    return ok;
}
