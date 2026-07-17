#include "test_login_erase_empty_ip_2973.h"

#include "login/connect_cleanup.h"
#include "login/session_cleanup.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "login ShouldEraseEmptyIPEntry 2973 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline periodicCleanup / clear empty-IP outer erase formula for dual-wire
// cross-check (slice 2973):
//   ipMapEmpty
auto inlineShouldEraseEmptyIPEntry(const bool ipMapEmpty) -> bool
{
    return ipMapEmpty;
}

} // namespace

// Pure dual-wire expansion for loginHelpers::ShouldEraseEmptyIPEntry
// (ipMapEmpty; slice 2973).
auto runLoginEraseEmptyIP2973SelfTests() -> bool
{
    using loginHelpers::ShouldEraseEmptyIPEntry;
    using loginHelpers::ShouldEraseIdleSession;
    using loginHelpers::ShouldEraseIPAfterSessionErase;

    bool ok = true;

    // Residual 1325 pins still hold under dual-wire.
    ok = expect(ShouldEraseEmptyIPEntry(true), "residual empty ip erases") && ok;
    ok = expect(!ShouldEraseEmptyIPEntry(false), "residual nonempty ip keeps") && ok;

    const struct
    {
        bool        ipMapEmpty;
        bool        want;
        const char* label;
    } cases[] = {
        // Empty inner map → erase outer IP entry
        { true, true, "empty ip erases" },

        // Non-empty inner map → keep outer IP entry
        { false, false, "nonempty ip keeps" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldEraseEmptyIPEntry(c.ipMapEmpty);
        const bool inlineF = inlineShouldEraseEmptyIPEntry(c.ipMapEmpty);
        const bool wantPin = c.ipMapEmpty;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldEraseEmptyIPEntry dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldEraseEmptyIPEntry == pin formula") && ok;
    }

    // Pin composition: identity of ipMapEmpty.
    ok = expect(ShouldEraseEmptyIPEntry(true), "empty must erase") && ok;
    ok = expect(!ShouldEraseEmptyIPEntry(false), "nonempty must keep") && ok;

    // Dense compose: full 2^1 boolean space.
    for (const bool ipMapEmpty : { false, true })
    {
        const bool got  = ShouldEraseEmptyIPEntry(ipMapEmpty);
        const bool want = ipMapEmpty;
        ok              = expect(got == want, "compose free == pin formula") && ok;
        ok              = expect(got == inlineShouldEraseEmptyIPEntry(ipMapEmpty), "compose free == inline") && ok;
    }

    // --- Production periodicCleanup / clear path semantics ---
    // Host injects:
    //   ipMapEmpty = ipAddrIterator->second.size() == 0
    // After inner sessionIterator erases complete for this IP:
    //   size()==0 → erase outer IP entry
    //   size()!=0 → keep outer IP entry and advance iterator
    //
    // Host inject poles (size comparisons):
    //   size 0 → empty true  → erase
    //   size 1 → empty false → keep
    //   size N → empty false → keep
    const bool sizeZeroEmpty = true;  // size() == 0
    const bool sizeOneEmpty  = false; // size() == 1
    const bool sizeManyEmpty = false; // size() > 1

    ok = expect(ShouldEraseEmptyIPEntry(sizeZeroEmpty), "host inject size 0 must erase") && ok;
    ok = expect(!ShouldEraseEmptyIPEntry(sizeOneEmpty), "host inject size 1 must keep") && ok;
    ok = expect(!ShouldEraseEmptyIPEntry(sizeManyEmpty), "host inject size N must keep") && ok;
    ok = expect(inlineShouldEraseEmptyIPEntry(sizeZeroEmpty), "host inject size 0 inline must erase") && ok;
    ok = expect(!inlineShouldEraseEmptyIPEntry(sizeOneEmpty), "host inject size 1 inline must keep") && ok;

    // Explicit dual-wire: free == ipMapEmpty for host poles.
    for (const bool ipMapEmpty : { false, true })
    {
        const bool got  = ShouldEraseEmptyIPEntry(ipMapEmpty);
        const bool want = ipMapEmpty;
        ok              = expect(got == want, "host inject free == pin") && ok;
        ok              = expect(got == inlineShouldEraseEmptyIPEntry(ipMapEmpty), "host inject free == inline") && ok;
    }

    // Sibling session-error IP erase (1322) shares polarity but is a separate
    // free function; both empty-true erase / empty-false keep.
    ok = expect(ShouldEraseIPAfterSessionErase(true) && !ShouldEraseIPAfterSessionErase(false),
                "session-error IP erase polarity residual") &&
         ok;
    ok = expect(ShouldEraseEmptyIPEntry(true) && !ShouldEraseEmptyIPEntry(false),
                "empty-IP vs session-error erase polarity residual") &&
         ok;

    // Sibling idle erase remains orthogonal (session-level, not outer-IP).
    // Empty-IP erase runs after idle / clear inner erases complete.
    ok = expect(ShouldEraseIdleSession(false, false, true),
                "idle erase residual (post-inner precondition context)") &&
         ok;
    // Idle keep leaves sessions → outer IP not empty → empty-IP keep.
    ok = expect(!ShouldEraseEmptyIPEntry(false), "nonempty outer after idle keep must keep") && ok;

    return ok;
}
