#include "test_garden_result_key_2838.h"

#include "map/utils/garden_capacity.h"
#include "map/utils/gardenutils.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "garden result key 2838 self-test failed: " << label << '\n';
    }
    return condition;
}

auto expectU32(const uint32 got, const uint32 want, const char* const label) -> bool
{
    if (got != want)
    {
        std::cerr << "garden result key 2838 self-test failed: " << label
                  << " got=0x" << std::hex << got << " want=0x" << want << std::dec << '\n';
        return false;
    }
    return true;
}

// Inline production formula for cross-check (seed << 8) + (element1 << 4) + element2.
auto inlinePack(const uint8 seed, const uint8 element1, const uint8 element2) -> uint32
{
    return (seed << 8) + (element1 << 4) + element2;
}

} // namespace

auto runGardenResultKey2838SelfTests() -> bool
{
    using gardenutilshelpers::ResultKey;

    bool ok = true;

    // --- Zero and simple packing ---
    ok = expectU32(ResultKey(0, 0, 0), 0, "zero") && ok;
    ok = expectU32(ResultKey(1, 0, 0), 0x100, "seed only") && ok;
    ok = expectU32(ResultKey(0, 1, 0), 0x10, "element1 only") && ok;
    ok = expectU32(ResultKey(0, 0, 1), 0x1, "element2 only") && ok;
    ok = expectU32(ResultKey(1, 2, 3), (1u << 8) + (2u << 4) + 3u, "1,2,3") && ok;

    // --- Production sample keys (plant + feeds) ---
    // Fruit seeds (1) + fire (1) common + ice (2) extra
    ok = expectU32(ResultKey(1, 1, 2), 0x112, "fruit fire ice") && ok;
    // Herb seeds (2) + none + none (plant-only CalculateResults key)
    ok = expectU32(ResultKey(2, 0, 0), 0x200, "herb none none") && ok;
    // Runtime fixture key from test_garden_result_runtime
    ok = expectU32(ResultKey(2, 1, 3), 0x213, "seed2 e1 e3") && ok;
    ok = expectU32(ResultKey(9, 8, 7), 0x987, "missing-key sample") && ok;

    // --- High nibble fields (element1/element2 fit in low 4 bits of production tables) ---
    ok = expectU32(ResultKey(255, 15, 15), (255u << 8) + (15u << 4) + 15u, "max seed+feeds") && ok;
    ok = expectU32(ResultKey(8, 8, 8), 0x888, "wildgrass light light") && ok;

    // --- Pure helper matches detail dual-wire wrapper and inline formula ---
    const struct
    {
        uint8 seed;
        uint8 element1;
        uint8 element2;
    } cases[] = {
        { 0, 0, 0 },
        { 1, 0, 0 },
        { 1, 2, 3 },
        { 2, 1, 3 },
        { 9, 8, 7 },
        { 255, 15, 15 },
        { 7, 4, 5 },
    };
    for (const auto& c : cases)
    {
        const uint32 pure   = ResultKey(c.seed, c.element1, c.element2);
        const uint32 detail = gardenutils::detail::ResultKey(c.seed, c.element1, c.element2);
        const uint32 want   = inlinePack(c.seed, c.element1, c.element2);
        ok                  = expect(pure == want, "pure matches inline pack") && ok;
        ok                  = expect(detail == pure, "detail dual-wires pure") && ok;
        ok                  = expect(detail == want, "detail matches inline pack") && ok;
    }

    // --- AppendResult dual-wires pure key into ResultMap ---
    {
        gardenutils::detail::ResultMap results;
        gardenutils::detail::AppendResult(results, 2, 1, 3, 100, 1, 2, 10);
        gardenutils::detail::AppendResult(results, 2, 1, 3, 200, 2, 4, 20);
        const uint32 key = ResultKey(2, 1, 3);
        ok               = expect(results.find(key) != results.end(), "append uses pure key") && ok;
        ok               = expect(results[key].size() == 2, "append groups by pure key") && ok;
        ok               = expect(results[key][0].ItemID == 100 && results[key][1].ItemID == 200,
                    "append order preserved") &&
             ok;
        // Independent feed key must not collide
        ok = expect(results.find(ResultKey(1, 0, 0)) == results.end(), "independent key absent") && ok;
    }

    return ok;
}
