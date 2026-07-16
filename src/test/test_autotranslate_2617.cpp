#include "test_autotranslate_2617.h"

#include <iostream>

#include "map/autotranslate.h"
#include "map/autotranslate_helpers.h"

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "autotranslate self-test failed: " << label << '\n';
    }
    return condition;
}

auto delimiter() -> char
{
    return static_cast<char>(0xFD);
}

auto lookup(const std::string&, const std::vector<uint16>& data) -> std::string
{
    if (data.size() == 4 && data[0] == 0x02 && data[1] == 0x02 && data[2] == 0x01 && data[3] == 0x00)
    {
        return "Greetings";
    }

    return "?";
}

auto testReplacement() -> bool
{
    const auto token = std::string{ delimiter(), static_cast<char>(0x02), static_cast<char>(0x02), static_cast<char>(0x01), static_cast<char>(0x00), delimiter() };

    bool ok = true;
    ok      = expect(autotranslate::replaceBytesWithLookup("hello", lookup) == "hello", "plain text") && ok;
    ok      = expect(autotranslate::replaceBytesWithLookup("say " + token + "!", lookup) == "say {Greetings}!", "known token") && ok;
    ok      = expect(autotranslate::replaceBytesWithLookup(token + token, lookup) == "{Greetings}{Greetings}", "adjacent tokens") && ok;
    ok      = expect(autotranslate::replaceBytes(token) == "{Greetings}", "production table lookup") && ok;
    return ok;
}

auto testInvalidAndUnclosedBlocks() -> bool
{
    const auto invalid  = std::string{ delimiter(), static_cast<char>(0x02), delimiter() };
    const auto unclosed = std::string{ 'x', delimiter(), static_cast<char>(0x02), static_cast<char>(0x02) };

    bool ok = true;
    ok      = expect(autotranslate::replaceBytesWithLookup(invalid, lookup) == "{?}", "invalid block delegates to lookup") && ok;
    ok      = expect(autotranslate::replaceBytesWithLookup(unclosed, lookup) == "x{", "unclosed block retains opening brace only") && ok;
    return ok;
}

} // namespace

auto runAutotranslate2617SelfTests() -> bool
{
    return testReplacement() && testInvalidAndUnclosedBlocks();
}
