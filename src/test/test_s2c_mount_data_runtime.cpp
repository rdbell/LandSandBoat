#include "test_s2c_mount_data_runtime.h"

#include <array>
#include <cstdint>
#include <cstring>
#include <iostream>

#include "common/mmo.h"
#include "map/packets/s2c/0x0ae_mount_data.h"

auto runS2CMountDataRuntimeSelfTests() -> bool
{
    auto keys = keyitems_t{};
    std::memset(&keys.tables[6].keyList, 0xA5, sizeof(keys.tables[6].keyList));
    std::memset(&keys.tables[5].keyList, 0x5A, sizeof(keys.tables[5].keyList));

    const auto plan     = mountdatahelpers::PlanFor(keys);
    const auto expected = std::array<uint8_t, 8>{ 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5 };
    const auto copied   = std::memcmp(plan.MountDataTbl, expected.data(), expected.size()) == 0;
    if (!copied)
    {
        std::cerr << "s2c MOUNT_DATA runtime self-test failed: table six selection\n";
    }
    return copied;
}
