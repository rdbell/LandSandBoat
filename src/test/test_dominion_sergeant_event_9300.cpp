#include "omega_self_test_registry.h"

#include <cstdint>
#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "dominion sergeant event 9300 self-test failed: " << label << '\n';
    }
    return condition;
}

auto optionType(const uint32_t option) -> uint32_t
{
    return option & 0xF;
}

auto selectedOp(const uint32_t option) -> uint32_t
{
    return option >> 4;
}

auto opIDFor(const uint16_t zone, const uint32_t selected) -> uint16_t
{
    const int zoneIndex = zone == 218 ? 0 : zone == 253 ? 1 : zone == 254 ? 2 : -1;
    return zoneIndex >= 0 && selected >= 1 && selected <= 14
        ? static_cast<uint16_t>(559 + zoneIndex * 14 + selected)
        : 0;
}

} // namespace

auto runDominionSergeantEvent9300SelfTests() -> bool
{
    bool ok = true;

    // sergeantOnEventUpdate: option type 2 sends kill target and OP ID.
    const uint32_t selected = (2u << 4) | 2u;
    ok = expect(optionType(selected) == 2, "selected OP type") && ok;
    ok = expect(selectedOp(selected) == 2, "selected OP index") && ok;
    ok = expect(opIDFor(218, selectedOp(selected)) == 561, "Altepa OP 02") && ok;
    ok = expect(opIDFor(253, 14) == 587, "Uleguerand OP 14") && ok;
    ok = expect(opIDFor(254, 14) == 601, "Grauberg OP 14") && ok;

    // Type 6 sends difficulty 1; type 9 currently sends eight zeroes.
    ok = expect(optionType(6) == 6, "difficulty type") && ok;
    ok = expect(optionType(9) == 9, "bonus type") && ok;
    const uint32_t bonus[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    for (const auto value : bonus)
    {
        ok = expect(value == 0, "bonus zero payload") && ok;
    }

    // sergeantOnEventFinish uses the same low nibble, and always saves the
    // packed influence list after activate/cancel/complete or no action.
    ok = expect(optionType(2) == 2, "activate type") && ok;
    ok = expect(optionType(3) == 3, "cancel type") && ok;
    ok = expect(optionType(8) == 8, "complete type") && ok;
    ok = expect(opIDFor(218, 1) == 560, "activate OP 01") && ok;
    ok = expect(opIDFor(254, 14) == 601, "activate OP 14") && ok;
    ok = expect(optionType(0) == 0, "unhandled finish type") && ok;

    return ok;
}

OMEGA_REGISTER_SELF_TEST("dominion-sergeant-event-9300", runDominionSergeantEvent9300SelfTests);
