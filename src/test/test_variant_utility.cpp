#include "test_variant_utility.h"

#include <iostream>
#include <string>

#include "common/types/variant.h"

auto runVariantUtilitySelfTests() -> bool
{
    bool ok = true;

    auto value = Variant<int, std::string>{ 7 };
    value.visit(xi::overload{
        [&ok](int& number)
        {
            ok     = ok && number == 7;
            number = 9;
        },
        [&ok](std::string&)
        {
            ok = false;
        },
    });
    ok = ok && std::get<int>(value) == 9;

    const auto text   = Variant<int, std::string>{ "Omega" };
    const auto length = text.visit(xi::overload{
        [](const int&) -> std::size_t
        {
            return 0;
        },
        [](const std::string& string) -> std::size_t
        {
            return string.size();
        },
    });
    ok                = ok && length == 5;

    const auto rvalueResult = Variant<int, std::string>{ 42 }.visit(xi::overload{
        [](int number) -> int
        {
            return number + 1;
        },
        [](std::string) -> int
        {
            return -1;
        },
    });
    ok                      = ok && rvalueResult == 43;

    if (!ok)
    {
        std::cerr << "variant utility self-test failed\n";
    }
    return ok;
}
