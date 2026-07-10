#include "test_map_session_index.h"

#include "map/map_session.h"
#include "map/map_session_container.h"
#include "map/entities/char_entity.h"

#include <cstdint>
#include <iostream>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "map-session index self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto testConfirmedLookupsAndReplacement() -> bool
{
    MapSessionIndex index;
    MapSession       lower;
    MapSession       upper;
    lower.client_ipp = IPP(0x0100007F, 1000);
    lower.charID     = 42;
    lower.accountID  = 7;
    upper.client_ipp = IPP(0x0200007F, 1000);
    upper.charID     = 42;
    upper.accountID  = 7;

    index.addSession(&upper);
    index.addSession(&lower);

    bool ok = true;
    ok      = expect(index.getSessionByIPP(lower.client_ipp) == &lower, "IPP lookup") && ok;
    ok      = expect(index.getSessionByIPP(lower.client_ipp.getRawIPP()) == &lower, "raw IPP lookup") && ok;
    ok      = expect(index.getSessionByCharId(42) == &lower, "character lookup uses lowest IPP") && ok;
    ok      = expect(index.getSessionByAccountId(7) == &lower, "account lookup uses lowest IPP") && ok;
    ok      = expect(index.confirmedSize() == 2, "confirmed size") && ok;

    upper.charID    = 43;
    upper.accountID = 8;
    ok = expect(index.getSessionByCharId(43) == &upper && index.getSessionByAccountId(8) == &upper,
                "live character and account lookups") && ok;

    MapSession replacement;
    replacement.client_ipp = lower.client_ipp;
    replacement.charID     = 99;
    index.addSession(&replacement);
    ok = expect(index.getSessionByIPP(lower.client_ipp) == &replacement, "same-key replacement") && ok;
    ok = expect(!index.removeSession(&lower), "stale removal rejected") && ok;
    ok = expect(index.removeSession(&replacement), "current removal accepted") && ok;
    ok = expect(index.confirmedSize() == 1, "confirmed size after removal") && ok;
    return ok;
}

auto testPendingLookupsAndRemoval() -> bool
{
    MapSessionIndex index;
    MapSession       pending;
    pending.charID = 77;

    index.addPendingSession(&pending);

    bool ok = true;
    ok      = expect(index.getPendingSessionByCharId(77) == &pending, "pending lookup") && ok;
    ok      = expect(index.pendingSize() == 1, "pending size") && ok;
    ok      = expect(index.removePendingSession(&pending), "pending pointer removal") && ok;
    ok      = expect(index.getPendingSessionByCharId(77) == nullptr, "pending removal") && ok;

    index.addPendingSession(&pending);
    ok = expect(index.removePendingSession(77) == &pending, "pending ID removal") && ok;
    ok = expect(index.removePendingSession(77) == nullptr, "missing pending ID removal") && ok;

    MapSession replacement;
    replacement.charID = 77;
    index.addPendingSession(&replacement);
    ok = expect(!index.removePendingSession(&pending), "stale pending removal rejected") && ok;
    ok = expect(index.removePendingSession(&replacement), "current pending removal accepted") && ok;
    return ok;
}

auto testNilAndMissingInputs() -> bool
{
    MapSessionIndex index;
    bool            ok = true;
    ok                  = expect(index.getSessionByIPP(IPP()) == nullptr, "missing IPP") && ok;
    ok                  = expect(index.getSessionByCharId(0) == nullptr, "missing character") && ok;
    ok                  = expect(index.getSessionByAccountId(0) == nullptr, "missing account") && ok;
    ok                  = expect(index.getPendingSessionByCharId(0) == nullptr, "missing pending") && ok;
    ok                  = expect(!index.removeSession(nullptr), "nil session removal") && ok;
    ok                  = expect(!index.removePendingSession(nullptr), "nil pending removal") && ok;
    index.addSession(nullptr);
    index.addPendingSession(nullptr);
    ok = expect(index.confirmedSize() == 0 && index.pendingSize() == 0, "nil additions ignored") && ok;
    return ok;
}

} // namespace

auto runMapSessionIndexSelfTests() -> bool
{
    bool ok = true;
    ok      = testConfirmedLookupsAndReplacement() && ok;
    ok      = testPendingLookupsAndRemoval() && ok;
    ok      = testNilAndMissingInputs() && ok;
    return ok;
}
