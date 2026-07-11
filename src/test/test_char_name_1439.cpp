#include "test_char_name_1439.h"

#include "map/char_name_capacity.h"

#include <iostream>
#include <string>

auto runCharName1439SelfTests() -> bool
{
    const std::string embeddedNull{ "ab\0cd", 5 };
    const std::string exactBoundary(16, 'B');
    const std::string longAscii = "1234567890ABCDEFG";
    const std::string splitUtf8 = std::string(15, 'A') + "\xC3\xA9";
    const std::string splitWant = std::string(15, 'A') + "\xC3";

    const bool ok = PacketNameLength == 16 &&
                    charnamehelpers::Normalize("").empty() &&
                    charnamehelpers::Normalize("Zeid") == "Zeid" &&
                    charnamehelpers::Normalize(embeddedNull) == embeddedNull &&
                    charnamehelpers::Normalize(exactBoundary) == exactBoundary &&
                    charnamehelpers::Normalize(longAscii) == "1234567890ABCDEF" &&
                    charnamehelpers::Normalize(splitUtf8) == splitWant;

    if (!ok)
    {
        std::cerr << "char name 1439 self-test failed\n";
    }
    return ok;
}
