#include "test_c2s_maprect_moghouse_exit.h"
#include "map/packets/c2s/maprect_moghouse_exit.h"
#include <iostream>

namespace
{

auto region(const uint16_t zone) -> REGION_TYPE
{
    if (zone >= 230 && zone <= 233)
        return REGION_TYPE::SANDORIA;
    if (zone == 256 || zone == 257)
        return REGION_TYPE::ADOULIN_ISLANDS;
    return REGION_TYPE::UNKNOWN;
}

auto expect(bool v, const char* n) -> bool
{
    if (!v)
        std::cerr << "MAPRECT Mog House self-test failed: " << n << '\n';
    return v;
}

} // namespace

auto runC2SMapRectMogHouseExitSelfTests() -> bool
{
    bool       ok    = true;
    const auto c     = maprect::ClassifyRectID(0x71726d7a);
    ok               = expect(c.isMogHouseExit && c.isMogHouseEntrance, "zmrq fourcc") && ok;
    ok               = expect(!maprect::ClassifyRectID(0x00726d7a).isMogHouseExit, "fourcc no NUL") && ok;
    auto in          = maprect::MogHouseExitInput{ 230, REGION_TYPE::SANDORIA, true, 1, GP_CLI_COMMAND_MAPRECT_MYROOMEXITBIT::SandOria, GP_CLI_COMMAND_MAPRECT_MYROOMEXITMODE::Option2, region };
    auto d           = maprect::MogHouseExitFor(in);
    ok               = expect(d.action == maprect::MogHouseExitAction::LeaveMogHouse && d.destinationZone == 231 && d.clearSecondFloorTracker, "city quest exit") && ok;
    in.mogHouseFlags = 0;
    d                = maprect::MogHouseExitFor(in);
    ok               = expect(d.action == maprect::MogHouseExitAction::Denied, "locked city exit") && ok;
    in.exitMode      = GP_CLI_COMMAND_MAPRECT_MYROOMEXITMODE::Mog2F;
    in.mogHouseFlags = 0x20;
    d                = maprect::MogHouseExitFor(in);
    ok               = expect(d.action == maprect::MogHouseExitAction::ChangeFloor && d.toggleSecondFloorTracker, "unlocked floor") && ok;
    in.exitMode      = GP_CLI_COMMAND_MAPRECT_MYROOMEXITMODE::Option1;
    in.exitBit       = GP_CLI_COMMAND_MAPRECT_MYROOMEXITBIT::RonfaureFront;
    d                = maprect::MogHouseExitFor(in);
    ok               = expect(d.action == maprect::MogHouseExitAction::Abort, "crafted valid bit abort") && ok;
    in.exitBit       = GP_CLI_COMMAND_MAPRECT_MYROOMEXITBIT::SandOria;
    in.exitMode      = GP_CLI_COMMAND_MAPRECT_MYROOMEXITMODE::AreaEnteredFrom;
    in.inMogHouse    = false;
    d                = maprect::MogHouseExitFor(in);
    ok               = expect(d.action == maprect::MogHouseExitAction::Denied, "area entered outside mog house") && ok;
    in               = { 256, REGION_TYPE::ADOULIN_ISLANDS, true, 0, GP_CLI_COMMAND_MAPRECT_MYROOMEXITBIT::Adoulin, GP_CLI_COMMAND_MAPRECT_MYROOMEXITMODE::Option1, region };
    d                = maprect::MogHouseExitFor(in);
    return expect(d.action == maprect::MogHouseExitAction::LeaveMogHouse && d.destinationZone == 256, "Adoulin always available") && ok;
}
