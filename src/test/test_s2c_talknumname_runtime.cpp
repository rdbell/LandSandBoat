#include "test_s2c_talknumname_runtime.h"

#include <array>
#include <cstring>
#include <iostream>

#include "map/packets/s2c/0x043_talknumname.h"

auto runS2CTalkNumNameRuntimeSelfTests() -> bool
{
    constexpr auto expectedName = std::array<uint8, 16>{ 'a', 'b', 0, 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o' };
    const auto     plan         = talknumnamehelpers::PlanFor({ .id = 0x11223344, .targid = 0x5566, .messageId = 0x9001, .name = std::string_view("ab\0cdefghijklmnopqrstuvwxyz", 20) });
    const auto     ok           = plan.UniqueNo == 0x11223344 && plan.ActIndex == 0x5566 && plan.MesNum == 0x1001 && std::memcmp(plan.sName, expectedName.data(), expectedName.size()) == 0;
    if (!ok)
        std::cerr << "s2c TALKNUMNAME runtime self-test failed\n";
    return ok;
}
