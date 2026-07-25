#include "test_char_name_lookup_6888.h"

#include "map/char_name_lookup.h"

#include <iostream>

auto runCharNameLookup6888SelfTests() -> bool
{
    const bool ok = charnamelookuphelpers::CharIDFromResult(false, 7) == 0 &&
                    charnamelookuphelpers::CharIDFromResult(true, 7) == 7 &&
                    charnamelookuphelpers::AccountIDFromResult(false, 11) == 0 &&
                    charnamelookuphelpers::AccountIDFromResult(true, 11) == 11 &&
                    charnamelookuphelpers::IDsFromResult(false, 7, 11) == charnamelookuphelpers::IDs{} &&
                    charnamelookuphelpers::IDsFromResult(true, 7, 11) == charnamelookuphelpers::IDs{ 7, 11 };
    if (!ok)
    {
        std::cerr << "character name lookup 6888 self-test failed\n";
    }
    return ok;
}
