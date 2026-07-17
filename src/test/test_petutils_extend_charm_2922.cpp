#include "test_petutils_extend_charm_2922.h"

#include "map/pet_mod_tandem_capacity.h"

#include <cstdint>
#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "petutils extend_charm 2922 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline ExtendCharm early-return formula for dual-wire cross-check:
//   isMob && isCharmed && minSeconds <= maxSeconds && maxSeconds != 0
auto inlineShouldExtendCharm(const bool isMob, const bool isCharmed, const std::uint16_t minSeconds,
                             const std::uint16_t maxSeconds) -> bool
{
    return isMob && isCharmed && minSeconds <= maxSeconds && maxSeconds != 0;
}

} // namespace

// Pure dual-wire expansion for petmodtandemhelpers::ShouldExtendCharm
// (petutils::ExtendCharm eligibility + range gates; slice 2922).
auto runPetutilsExtendCharm2922SelfTests() -> bool
{
    using petmodtandemhelpers::CanExtendCharm;
    using petmodtandemhelpers::CharmSecondsRangeValid;
    using petmodtandemhelpers::ShouldExtendCharm;

    bool ok = true;

    // Residual 1624 component pins.
    ok = expect(CanExtendCharm(true, true), "CanExtendCharm charmed mob") && ok;
    ok = expect(!CanExtendCharm(true, false), "CanExtendCharm mob not charmed") && ok;
    ok = expect(!CanExtendCharm(false, true), "CanExtendCharm non-mob charmed") && ok;
    ok = expect(!CanExtendCharm(false, false), "CanExtendCharm non-mob not charmed") && ok;

    ok = expect(CharmSecondsRangeValid(1, 5), "range 1..5 valid") && ok;
    ok = expect(CharmSecondsRangeValid(3, 3), "range equal valid") && ok;
    ok = expect(!CharmSecondsRangeValid(5, 1), "range inverted invalid") && ok;
    ok = expect(!CharmSecondsRangeValid(0, 0), "range max 0 invalid") && ok;
    ok = expect(!CharmSecondsRangeValid(1, 0), "range max 0 with min invalid") && ok;

    // --- ShouldExtendCharm table ---
    const struct
    {
        bool          isMob;
        bool          isCharmed;
        std::uint16_t minSeconds;
        std::uint16_t maxSeconds;
        bool          want;
        const char*   label;
    } cases[] = {
        { true, true, 1, 10, true, "charmed mob valid range" },
        { true, true, 3, 3, true, "charmed mob equal range" },
        { true, true, 0, 1, true, "charmed mob min 0 max 1" },
        { true, true, 0, 0xffff, true, "charmed mob full uint16 range" },
        { false, true, 1, 10, false, "non-mob charmed blocks" },
        { true, false, 1, 10, false, "mob not charmed blocks" },
        { false, false, 1, 10, false, "non-mob not charmed blocks" },
        { true, true, 5, 1, false, "inverted range blocks" },
        { true, true, 0, 0, false, "max 0 blocks" },
        { true, true, 1, 0, false, "max 0 with min blocks" },
        { false, true, 5, 1, false, "non-mob + inverted range blocks" },
        { true, false, 0, 0, false, "not charmed + max 0 blocks" },
        { true, true, 100, 100, true, "equal high range" },
        { true, true, 100, 99, false, "min > max by one" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldExtendCharm(c.isMob, c.isCharmed, c.minSeconds, c.maxSeconds);
        const bool inlineF = inlineShouldExtendCharm(c.isMob, c.isCharmed, c.minSeconds, c.maxSeconds);
        const bool compose = CanExtendCharm(c.isMob, c.isCharmed) &&
                             CharmSecondsRangeValid(c.minSeconds, c.maxSeconds);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "dual-wire free == inline formula") && ok;
        ok = expect(got == compose, "dual-wire free == CanExtendCharm && CharmSecondsRangeValid") && ok;
    }

    // Production dual-wire compose identity: free == helpers AND.
    const struct
    {
        bool          isMob;
        bool          isCharmed;
        std::uint16_t minSeconds;
        std::uint16_t maxSeconds;
        const char*   label;
    } composeCases[] = {
        { true, true, 1, 10, "compose ok" },
        { true, true, 5, 1, "compose bad range" },
        { false, true, 1, 10, "compose non-mob" },
        { true, false, 1, 10, "compose not charmed" },
        { true, true, 0, 0, "compose max 0" },
        { false, false, 0, 0, "compose all false" },
        { true, true, 50, 50, "compose equal" },
        { true, true, 0xffff, 0xffff, "compose max uint16 equal" },
        { true, true, 0xffff, 0, "compose max 0 min high" },
    };

    for (const auto& c : composeCases)
    {
        const bool got     = ShouldExtendCharm(c.isMob, c.isCharmed, c.minSeconds, c.maxSeconds);
        const bool inlineF = inlineShouldExtendCharm(c.isMob, c.isCharmed, c.minSeconds, c.maxSeconds);
        const bool compose = CanExtendCharm(c.isMob, c.isCharmed) &&
                             CharmSecondsRangeValid(c.minSeconds, c.maxSeconds);

        ok = expect(got == compose, c.label) && ok;
        ok = expect(got == inlineF, "compose dual-wire free == inline") && ok;
    }

    return ok;
}
