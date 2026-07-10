#include "test_zone_catalog_runtime_1231.h"

#include "map/utils/zoneutils.h"

#include <iostream>

namespace
{
auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "zone catalog runtime 1231 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

auto runZoneCatalogRuntime1231SelfTests() -> bool
{
    using zoneutils::detail::DecideZoneReady;
    using zoneutils::detail::ZoneReadyDecision;

    bool ok = true;
    ok = expect(zoneutils::detail::IsInstancedZoneType(ZONE_TYPE::INSTANCED), "instanced type") && ok;
    ok = expect(!zoneutils::detail::IsInstancedZoneType(static_cast<ZONE_TYPE>(0)), "normal type") && ok;
    ok = expect(DecideZoneReady(true, true, true, true) == ZoneReadyDecision::Ready, "loaded ready") && ok;
    ok = expect(DecideZoneReady(false, false, true, true) == ZoneReadyDecision::Ready, "immediate mode ready") && ok;
    ok = expect(DecideZoneReady(false, true, false, true) == ZoneReadyDecision::UnmanagedReady, "unmanaged ready") && ok;
    ok = expect(DecideZoneReady(false, true, true, false) == ZoneReadyDecision::LoadSynchronously, "sync lazy load") && ok;
    ok = expect(DecideZoneReady(false, true, true, true) == ZoneReadyDecision::QueueAsynchronously, "async lazy queue") && ok;
    return ok;
}
