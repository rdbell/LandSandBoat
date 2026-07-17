#include "test_latent_all_active_2835.h"

#include "map/latent_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "latent all active 2835 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runLatentAllActive2835SelfTests() -> bool
{
    using namespace latenthelpers;

    bool ok = true;

    // ShouldMarkNotAllActive: !isActivated && slotMatches
    ok = expect(ShouldMarkNotAllActive(false, true), "inactive + match → mark") && ok;
    ok = expect(!ShouldMarkNotAllActive(true, true), "active + match → skip") && ok;
    ok = expect(!ShouldMarkNotAllActive(false, false), "inactive + other slot → skip") && ok;
    ok = expect(!ShouldMarkNotAllActive(true, false), "active + other slot → skip") && ok;

    const struct
    {
        bool        isActivated;
        bool        slotMatches;
        bool        want;
        const char* label;
    } cases[] = {
        { false, true, true, "table inactive match" },
        { true, true, false, "table active match" },
        { false, false, false, "table inactive other" },
        { true, false, false, "table active other" },
    };
    for (const auto& c : cases)
    {
        const bool got = ShouldMarkNotAllActive(c.isActivated, c.slotMatches);
        ok             = expect(got == c.want, c.label) && ok;
        ok             = expect(got == (!c.isActivated && c.slotMatches), "compose !activated && match") && ok;
        ok             = expect(DoesInactiveLatentDisqualifyAllActive(c.isActivated, c.slotMatches) == got, "alias match") && ok;
    }

    // Vacuous / compose: only inactive+match disqualifies all-active
    ok = expect(DoesInactiveLatentDisqualifyAllActive(false, true), "alias inactive match") && ok;
    ok = expect(!DoesInactiveLatentDisqualifyAllActive(true, true), "alias active match") && ok;

    return ok;
}
