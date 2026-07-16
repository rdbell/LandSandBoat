#include "test_s2c_scenarioitem_runtime.h"

#include <cstdint>
#include <cstring>
#include <iostream>

#include "map/packets/s2c/0x055_scenarioitem.h"

auto runS2CScenarioItemRuntimeSelfTests() -> bool
{
    auto keys = keyitems_t{};
    std::memset(&keys.tables[3].keyList, 0xA5, sizeof(keys.tables[3].keyList));
    std::memset(&keys.tables[3].seenList, 0x5A, sizeof(keys.tables[3].seenList));

    const auto valid   = scenarioitemhelpers::PlanFor(keys, 3);
    const auto invalid = scenarioitemhelpers::PlanFor(keys, static_cast<uint8>(keys.tables.size()));
    const auto zero    = GP_SERV_COMMAND_SCENARIOITEM::PacketData{};
    const auto copied  = std::memcmp(valid.GetItemFlag, &keys.tables[3].keyList, sizeof(valid.GetItemFlag)) == 0 && std::memcmp(valid.LookItemFlag, &keys.tables[3].seenList, sizeof(valid.LookItemFlag)) == 0 && valid.TableIndex == 3;
    const auto zeroed  = std::memcmp(&invalid, &zero, sizeof(invalid)) == 0;
    if (!copied || !zeroed)
    {
        std::cerr << "s2c SCENARIOITEM runtime self-test failed: table selection\n";
    }
    return copied && zeroed;
}
