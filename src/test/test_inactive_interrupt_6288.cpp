#include "test_inactive_interrupt_6288.h"

#include "map/ai/states/inactive_interrupt.h"

#include <iostream>

namespace
{
auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "inactive interrupt 6288 self-test failed: " << label << '\n';
    }

    return condition;
}
} // namespace

auto runInactiveInterrupt6288SelfTests() -> bool
{
    return expect(inactiveinterrupt::shouldInterrupt(false), "non-changeable state interrupts") &&
           expect(!inactiveinterrupt::shouldInterrupt(true), "changeable state preserves stack");
}
