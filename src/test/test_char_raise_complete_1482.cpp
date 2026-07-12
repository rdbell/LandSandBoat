#include "test_char_raise_complete_1482.h"

#include "map/char_raise_complete_capacity.h"

#include <iostream>
#include <string>
#include <vector>

auto runCharRaiseComplete1482SelfTests() -> bool
{
    std::vector<std::string> calls{};
    charraisecompletehelpers::Complete(
        [&]() { calls.emplace_back("on-raise"); },
        [&]() { calls.emplace_back("accept"); },
        [&]() { calls.emplace_back("death-time"); });
    const bool ok = calls == std::vector<std::string>{ "on-raise", "accept", "death-time" };
    if (!ok)
    {
        std::cerr << "char raise complete 1482 self-test failed\n";
    }
    return ok;
}
