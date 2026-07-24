#include "test_zone_entity_lookup_6264.h"

#include "map/entities/base_entity.h"
#include "map/zone_entity_lookup.h"

#include <iostream>

namespace
{
auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "zone entity lookup 6264 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

// Pins GetEntity's production-wired targid partitions and type filter.
auto runZoneEntityLookup6264SelfTests() -> bool
{
    using namespace zoneentitylookup;

    bool ok = true;
    ok      = expect(ClassifyTargetID(0x000) == TargetRange::Static, "static lower bound") && ok;
    ok      = expect(ClassifyTargetID(0x3FF) == TargetRange::Static, "static upper bound") && ok;
    ok      = expect(ClassifyTargetID(0x400) == TargetRange::Character, "character lower bound") && ok;
    ok      = expect(ClassifyTargetID(0x6FF) == TargetRange::Character, "character upper bound") && ok;
    ok      = expect(ClassifyTargetID(0x700) == TargetRange::Dynamic, "dynamic lower bound") && ok;
    ok      = expect(ClassifyTargetID(0xFFF) == TargetRange::Dynamic, "dynamic upper bound") && ok;
    ok      = expect(ClassifyTargetID(0x1000) == TargetRange::Invalid, "invalid upper bound") && ok;
    ok      = expect(RequestsType(TYPE_MOB | TYPE_NPC, TYPE_NPC), "requested type bit matches") && ok;
    ok      = expect(!RequestsType(TYPE_MOB, TYPE_NPC), "unrequested type bit does not match") && ok;
    return ok;
}
