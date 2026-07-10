#include "map/utils/blueutils.h"

#include <array>
#include <iostream>
#include <string_view>

namespace
{
auto expect(const bool condition, const std::string_view label) -> bool
{
    if (!condition)
    {
        std::cerr << "blue magic set runtime 1233 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

auto runBlueMagicSetRuntime1233SelfTests() -> bool
{
    std::array<uint8, 20> sparse{ 0, 3, 0, 1, 0, 2 };
    blueutils::detail::CompactSpellSet(sparse);
    const std::array<uint8, 20> compact{ 3, 1, 2 };

    bool ok = true;
    ok      = expect(sparse == compact, "stable compaction") && ok;
    ok      = expect(!blueutils::detail::ExceedsSetPoints(4, 5, 9), "exact point fit") && ok;
    ok      = expect(blueutils::detail::ExceedsSetPoints(250, 10, 255), "addition does not narrow before comparison") && ok;
    return ok;
}
