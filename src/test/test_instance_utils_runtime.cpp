#include "test_instance_utils_runtime.h"

#include "map/utils/instanceutils.h"

#include <iostream>
#include <unordered_map>

namespace instanceutils
{
extern std::unordered_map<uint16, InstanceData_t> InstanceData;
extern detail::LazyLoadState                      lazyLoad;
} // namespace instanceutils

namespace
{
auto expect(bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "instance utils runtime self-test failed: " << label << '\n';
        return false;
    }
    return true;
}
} // namespace

auto runInstanceUtilsRuntimeSelfTests() -> bool
{
    InstanceData_t defaults;
    bool           ok = true;
    ok = expect(defaults.music_day == 0 && defaults.music_night == 0 && defaults.battlesolo == 0 && defaults.battlemulti == 0,
                "constructor leaves SQL NULL tracks as Maybe(0)") && ok;

    instanceutils::InstanceData.clear();
    instanceutils::lazyLoad.enabled = true;
    instanceutils::lazyLoad.managedInstances = { 12 };
    ok = expect(instanceutils::IsValidInstanceID(12), "managed lazy id is valid before loading") && ok;
    ok = expect(instanceutils::IsValidInstanceID(0x1000C), "validity narrows uint32 id to uint16") && ok;
    ok = expect(!instanceutils::IsValidInstanceID(13), "unmanaged lazy id is invalid") && ok;

    instanceutils::lazyLoad = {};
    instanceutils::InstanceData.clear();
    return ok;
}
