#include "test_s2c_pendingstr_runtime.h"

#include <array>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>
#include <string_view>

#include "map/packets/s2c/0x05d_pendingstr.h"

namespace
{

auto expect(bool condition, std::string_view label) -> bool
{
    if (!condition)
    {
        std::cerr << "s2c PENDINGSTR runtime self-test failed: " << label << '\n';
    }
    return condition;
}

auto testPlanCopiesEveryParameterAndString() -> bool
{
    const auto facts = pendingstrhelpers::Facts{
        .strings = { std::string("ab\0cdefghijklmnopqrstuvwxyz", 20), "0123456789abcdef", std::string("\0ABCDEFGHIJKLMNO", 16), "four" },
        .params  = { 0x11223344, 0x80000000, 2, 3, 4, 5, 6, 7, UINT32_MAX },
    };
    const auto plan = pendingstrhelpers::PlanFor(facts);

    bool ok = true;
    for (std::size_t index = 0; index < facts.params.size(); ++index)
    {
        ok = expect(static_cast<uint32>(plan.num[index]) == facts.params[index], "parameter slot") && ok;
    }
    ok = expect(std::memcmp(plan.string1, std::array<char, 15>{ 'a', 'b', 0, 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n' }.data(), 15) == 0 && plan.string1[15] == 0, "string1 NUL-safe truncation") && ok;
    ok = expect(std::memcmp(plan.string2, "0123456789abcde", 15) == 0 && plan.string2[15] == 0, "string2 truncation") && ok;
    ok = expect(plan.string3[0] == 0 && std::memcmp(plan.string3 + 1, "ABCDEFGHIJKLMN", 14) == 0 && plan.string3[15] == 0, "string3 leading NUL copy") && ok;
    ok = expect(std::memcmp(plan.string4, "four", 4) == 0 && plan.string4[4] == 0, "string4 copy") && ok;
    return ok;
}

} // namespace

auto runS2CPendingStrRuntimeSelfTests() -> bool
{
    return testPlanCopiesEveryParameterAndString();
}
