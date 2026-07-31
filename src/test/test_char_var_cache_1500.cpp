#include "test_char_var_cache_1500.h"

#include "map/char_var_cache_capacity.h"

#include <iostream>

namespace
{
using charvarcachehelpers::ClearedEntry;
using charvarcachehelpers::IsFresh;
using charvarcachehelpers::MakeEntry;
using charvarcachehelpers::ShouldIncludeRow;
using charvarcachehelpers::ShouldRejectClearPrefix;
using charvarcachehelpers::ShouldUseCacheHit;
using charvarcachehelpers::StartsWithPrefix;

auto Check() -> bool
{
    if (!IsFresh(0, 100) || !IsFresh(101, 100) || IsFresh(100, 100) || IsFresh(99, 100))
    {
        return false;
    }

    if (!ShouldUseCacheHit(true, 0, 50) || !ShouldUseCacheHit(true, 60, 50) ||
        ShouldUseCacheHit(false, 0, 50) || ShouldUseCacheHit(true, 40, 50))
    {
        return false;
    }

    if (!ShouldRejectClearPrefix(4) || ShouldRejectClearPrefix(5) || ShouldRejectClearPrefix(10) ||
        ShouldRejectClearPrefix(-1))
    {
        return false;
    }

    if (!StartsWithPrefix("[ASSIST]foo", "[ASSIST]") || StartsWithPrefix("foo[ASSIST]", "[ASSIST]") ||
        StartsWithPrefix("ASSIST", "[ASSIST]"))
    {
        return false;
    }

    if (ClearedEntry().first != 0 || ClearedEntry().second != 0)
    {
        return false;
    }

    if (!ShouldIncludeRow(0, 10) || !ShouldIncludeRow(11, 10) || ShouldIncludeRow(10, 10))
    {
        return false;
    }

    const auto entry = MakeEntry(42, 99);
    if (entry.first != 42 || entry.second != 99)
    {
        return false;
    }

    return true;
}
} // namespace

auto runCharVarCache1500SelfTests() -> bool
{
    const bool ok = Check();
    if (!ok)
    {
        std::cerr << "char var cache 1500 self-test failed\n";
    }
    return ok;
}
