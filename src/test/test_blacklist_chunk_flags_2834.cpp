#include "test_blacklist_chunk_flags_2834.h"

#include "map/utils/blacklist_capacity.h"
#include "map/utils/blacklistutils.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "blacklist chunk flags 2834 self-test failed: " << label << '\n';
    }
    return condition;
}

auto expectFlags(const std::pair<bool, bool> got, const bool wantReset, const bool wantLast, const char* const label) -> bool
{
    return expect(got.first == wantReset && got.second == wantLast, label);
}

} // namespace

auto runBlacklistChunkFlags2834SelfTests() -> bool
{
    using blacklistutilshelpers::FullChunkFlags;

    bool ok = true;

    // Production default chunkSize=12 via capacity helper.
    ok = expectFlags(FullChunkFlags(12, 12), true, true, "exact first chunk finishes") && ok;
    ok = expectFlags(FullChunkFlags(12, 13), true, false, "filtered rows suppress last") && ok;
    ok = expectFlags(FullChunkFlags(13, 13), false, true, "second-chunk boundary not reset") && ok;
    ok = expectFlags(FullChunkFlags(24, 24), false, true, "second full chunk finishes") && ok;
    ok = expectFlags(FullChunkFlags(24, 25), false, false, "second full with more rows") && ok;
    ok = expectFlags(FullChunkFlags(0, 0), true, true, "zero counts") && ok;
    ok = expectFlags(FullChunkFlags(0, 1), true, false, "zero total nonzero rows") && ok;
    ok = expectFlags(FullChunkFlags(1, 0), true, false, "total exceeds zero rows") && ok;

    // Explicit chunkSize override.
    ok = expectFlags(FullChunkFlags(5, 5, 5), true, true, "custom size exact") && ok;
    ok = expectFlags(FullChunkFlags(5, 6, 5), true, false, "custom size not last") && ok;
    ok = expectFlags(FullChunkFlags(6, 6, 5), false, true, "custom size beyond reset") && ok;
    ok = expectFlags(FullChunkFlags(10, 10, 12), true, true, "under default size still reset") && ok;

    // detail:: dual-wire matches capacity default-12 path.
    const auto d12  = blacklistutils::detail::FullChunkFlags(12, 12);
    const auto d13  = blacklistutils::detail::FullChunkFlags(13, 13);
    const auto dFilt = blacklistutils::detail::FullChunkFlags(12, 13);
    ok = expectFlags(d12, true, true, "detail exact 12") && ok;
    ok = expectFlags(d13, false, true, "detail 13") && ok;
    ok = expectFlags(dFilt, true, false, "detail filtered") && ok;

    ok = expect(FullChunkFlags(12, 12) == d12, "capacity == detail (12,12)") && ok;
    ok = expect(FullChunkFlags(13, 13) == d13, "capacity == detail (13,13)") && ok;
    ok = expect(FullChunkFlags(12, 13) == dFilt, "capacity == detail (12,13)") && ok;

    return ok;
}
