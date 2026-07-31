#include "test_notoriety_has_enmity_result_2832.h"

#include "map/notoriety_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "notoriety has enmity result 2832 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runNotorietyHasEnmityResult2832SelfTests() -> bool
{
    using namespace notorietyhelpers;

    bool ok = true;

    // HasEnmityAfterPrune: !lookupEmpty
    ok = expect(HasEnmityAfterPrune(false), "non-empty reports enmity") && ok;
    ok = expect(!HasEnmityAfterPrune(true), "empty reports no enmity") && ok;

    const struct
    {
        bool        lookupEmpty;
        bool        want;
        const char* label;
    } enmityCases[] = {
        { false, true, "table non-empty" },
        { true, false, "table empty" },
    };
    for (const auto& c : enmityCases)
    {
        const bool got = HasEnmityAfterPrune(c.lookupEmpty);
        ok             = expect(got == c.want, c.label) && ok;
        ok             = expect(got == !c.lookupEmpty, "compose !lookupEmpty") && ok;
    }

    // NotorietySize: identity on host-injected count
    ok = expect(NotorietySize(0) == 0, "size zero") && ok;
    ok = expect(NotorietySize(1) == 1, "size one") && ok;
    ok = expect(NotorietySize(7) == 7, "size seven") && ok;
    ok = expect(NotorietySize(16) == 16, "size sixteen") && ok;

    const std::size_t sizeCases[] = { 0, 1, 2, 5, 16, 100 };
    for (const std::size_t n : sizeCases)
    {
        ok = expect(NotorietySize(n) == n, "size identity") && ok;
    }

    const auto wrappedSize = static_cast<std::size_t>(-1);
    ok                     = expect(NotorietySize(-1) == wrappedSize, "size_t conversion") && ok;

    return ok;
}
