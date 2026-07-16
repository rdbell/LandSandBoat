#include "test_conquest_data_2620.h"

#include <iostream>

#include "map/conquest_data.h"

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "conquest data self-test failed: " << label << '\n';
    }
    return condition;
}

auto testControls() -> bool
{
    auto                          data     = ConquestData{ {}, {} };
    std::vector<region_control_t> controls = {
        { NATION_BASTOK, NATION_WINDURST },
        { NATION_SANDORIA, NATION_BASTOK },
        { NATION_BEASTMEN, NATION_SANDORIA },
    };
    data.updateRegionControls(controls);

    bool ok = true;
    ok      = expect(data.getRegionOwner(REGION_TYPE::RONFAURE) == NATION_BASTOK, "regular region owner") && ok;
    ok      = expect(data.getRegionOwner(REGION_TYPE::SANDORIA) == NATION_BEASTMEN, "special region owner") && ok;
    ok      = expect(data.getRegionControlCount(NATION_BASTOK) == 1, "current control count") && ok;
    ok      = expect(data.getPrevRegionControlCount(NATION_BASTOK) == 1, "previous control count") && ok;

    controls[0].current = NATION_WINDURST;
    return expect(data.getRegionOwner(REGION_TYPE::RONFAURE) == NATION_BASTOK, "control update copies input") && ok;
}

auto testInfluence() -> bool
{
    auto                     data       = ConquestData{ {}, {} };
    std::vector<influence_t> influences = {
        { 10, 20, 30, 40 },
        { 1, 2, 3, 4 },
    };
    data.updateInfluencePoints(influences);
    data.addInfluencePoints(-5, NATION_SANDORIA, REGION_TYPE::RONFAURE);
    data.addInfluencePoints(99, NATION_NEUTRAL, REGION_TYPE::RONFAURE);

    bool ok = true;
    ok      = expect(data.getInfluence(REGION_TYPE::RONFAURE, NATION_SANDORIA) == 5, "negative influence delta") && ok;
    ok      = expect(data.getInfluence(REGION_TYPE::RONFAURE, NATION_BASTOK) == 20, "Bastok influence") && ok;
    ok      = expect(data.getInfluence(REGION_TYPE::RONFAURE, NATION_WINDURST) == 30, "Windurst influence") && ok;
    ok      = expect(data.getInfluence(REGION_TYPE::RONFAURE, NATION_BEASTMEN) == 40, "Beastmen influence") && ok;
    ok      = expect(data.getInfluence(REGION_TYPE::RONFAURE, NATION_NEUTRAL) == 0, "unknown nation") && ok;
    ok      = expect(data.getInfluence(REGION_TYPE::SANDORIA, NATION_SANDORIA) == 0, "out-of-range region") && ok;

    influences[0].sandoria_influence = 999;
    return expect(data.getInfluence(REGION_TYPE::RONFAURE, NATION_SANDORIA) == 5, "influence update copies input") && ok;
}

} // namespace

auto runConquestData2620SelfTests() -> bool
{
    return testControls() && testInfluence();
}
