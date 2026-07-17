#include "test_map_session_lookup_2790.h"

#include "map/map_session_container_capacity.h"

#include <iostream>
#include <string>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "map session lookup 2790 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runMapSessionLookup2790SelfTests() -> bool
{
    using mapsessionhelpers::SessionMatchesCharID;
    using mapsessionhelpers::SessionMatchesCharName;
    using mapsessionhelpers::ShouldRejectNullCharLookup;

    bool ok = true;

    // getSessionByChar: reject null PChar before scan.
    ok = expect(ShouldRejectNullCharLookup(true), "null char lookup rejects") && ok;
    ok = expect(!ShouldRejectNullCharLookup(false), "non-null char lookup proceeds") && ok;

    // getSessionByChar loop match: require owned char AND matching id.
    ok = expect(SessionMatchesCharID(true, 42, 42), "char id: has char match") && ok;
    ok = expect(!SessionMatchesCharID(true, 42, 7), "char id: has char mismatch") && ok;
    ok = expect(!SessionMatchesCharID(false, 42, 42), "char id: no char rejects even if ids equal") && ok;
    ok = expect(!SessionMatchesCharID(false, 0, 0), "char id: no char rejects zero ids") && ok;

    // getSessionByCharName loop match: require owned char AND exact name.
    ok = expect(SessionMatchesCharName(true, "Alice", "Alice"), "char name: exact match") && ok;
    ok = expect(!SessionMatchesCharName(true, "Alice", "alice"), "char name: case-sensitive") && ok;
    ok = expect(!SessionMatchesCharName(true, "Alice", "Bob"), "char name: mismatch") && ok;
    ok = expect(!SessionMatchesCharName(false, "Alice", "Alice"), "char name: no char rejects") && ok;
    ok = expect(!SessionMatchesCharName(false, "", ""), "char name: no char rejects empty") && ok;
    ok = expect(SessionMatchesCharName(true, "", ""), "char name: empty strings match when owned") && ok;

    // Null gate is independent of match helpers.
    ok = expect(ShouldRejectNullCharLookup(true) && !SessionMatchesCharID(false, 1, 1),
                "null reject and no-char mismatch both block") &&
        ok;

    return ok;
}
