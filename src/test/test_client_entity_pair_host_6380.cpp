#include "test_client_entity_pair_host_6380.h"

#include "lua/helpers/client_entity_pair_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "client entity pair host 6380 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure dual-wire suite for CLuaClientEntityPair helper residuals (slice 6380).
// Go: internal/cliententitypair.
auto runClientEntityPairHost6380SelfTests() -> bool
{
    using cliententitypairhelpers::BlueSpellIDOffset;
    using cliententitypairhelpers::BlueSpellPageSlots;
    using cliententitypairhelpers::ClassifyEntityQuery;
    using cliententitypairhelpers::EntityQueryKind;
    using cliententitypairhelpers::EventEndModeEnd;
    using cliententitypairhelpers::EventEndModeUpdatePending;
    using cliententitypairhelpers::FFIBodyCopyOK;
    using cliententitypairhelpers::InEvent;
    using cliententitypairhelpers::MoveDir;
    using cliententitypairhelpers::NoEventID;
    using cliententitypairhelpers::PacketHeaderSize;

    bool ok = true;

    ok = expect(NoEventID == 65535, "no event sentinel") && ok;
    ok = expect(!InEvent(NoEventID) && InEvent(1), "InEvent") && ok;
    ok = expect(EventEndModeEnd == 0 && EventEndModeUpdatePending == 1, "event modes") && ok;

    ok = expect(ClassifyEntityQuery(true, false, false) == EntityQueryKind::ID, "query id") && ok;
    ok = expect(ClassifyEntityQuery(false, true, false) == EntityQueryKind::Name, "query name") && ok;
    ok = expect(ClassifyEntityQuery(false, false, true) == EntityQueryKind::Entity, "query entity") && ok;
    ok = expect(ClassifyEntityQuery(false, false, false) == EntityQueryKind::Invalid, "query invalid") && ok;

    ok = expect(BlueSpellPageSlots == 20, "blu slots") && ok;
    ok = expect(BlueSpellIDOffset == 0x200, "blu offset") && ok;
    ok = expect(MoveDir(false, 9) == 0 && MoveDir(true, 9) == 9, "move dir") && ok;

    ok = expect(PacketHeaderSize == 4, "header size") && ok;
    ok = expect(FFIBodyCopyOK(12) && !FFIBodyCopyOK(4), "ffi copy range") && ok;

    return ok;
}
