#include "test_c2s_fragments_servmes_plan.h"
#include <cstdint>
#include <iostream>
#include <optional>

namespace
{

struct Plan
{
    bool                   send{};
    std::optional<int32_t> next{};
};

auto plan(std::optional<int32_t> last, int32_t offset, size_t len) -> Plan
{
    if (last && *last == offset)
        return {};
    Plan       p{ true, offset };
    const auto size = static_cast<int64_t>(len) + 1;
    auto       sent = size - offset;
    if (sent > 236)
        sent = 236;
    if (offset + sent >= size)
        p.next = std::nullopt;
    return p;
}

} // namespace

auto runC2SFragmentsServmesPlanSelfTests() -> bool
{
    bool ok = true;
    if (plan(4, 4, 10).send)
    {
        std::cerr << "FRAGMENTS duplicate failed\n";
        ok = false;
    }
    auto p = plan({}, 0, 500);
    if (!p.send || !p.next || *p.next != 0)
    {
        std::cerr << "FRAGMENTS continuation failed\n";
        ok = false;
    }
    if (plan({}, 236, 300).next)
    {
        std::cerr << "FRAGMENTS final failed\n";
        ok = false;
    }
    return ok;
}
