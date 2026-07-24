#include "test_death_raisable_hold_6290.h"

#include "map/ai/states/death_raisable_hold.h"

#include <iostream>

namespace
{
auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "death raisable hold 6290 self-test failed: " << label << '\n';
    }

    return condition;
}
} // namespace

auto runDeathRaisableHold6290SelfTests() -> bool
{
    using deathraisablehold::shouldComplete;
    using deathraisablehold::shouldExitEarly;
    using deathraisablehold::shouldHold;

    return expect(shouldExitEarly(true, true), "completed dead exits early") &&
           expect(shouldExitEarly(true, false), "completed alive exits early") &&
           expect(shouldExitEarly(false, false), "not completed and not dead exits early") &&
           expect(!shouldExitEarly(false, true), "active death stays past early gate") &&
           expect(shouldHold(true, true), "raisable mob holds after timer") &&
           expect(!shouldHold(true, false), "non-raisable mob does not hold") &&
           expect(!shouldHold(false, true), "non-mob cannot hold via raisable") &&
           expect(!shouldHold(false, false), "non-mob non-raisable does not hold") &&
           expect(!shouldComplete(true, true), "raisable mob does not complete") &&
           expect(shouldComplete(true, false), "non-raisable mob completes") &&
           expect(shouldComplete(false, false), "non-mob non-PC completes") &&
           expect(shouldComplete(false, true), "non-mob ignores raisable flag for complete");
}
