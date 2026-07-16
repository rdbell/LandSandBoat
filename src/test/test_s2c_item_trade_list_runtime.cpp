#include "test_s2c_item_trade_list_runtime.h"
#include "map/packets/s2c/item_trade_list_runtime.h"
#include <iostream>

auto runS2CItemTradeListRuntimeSelfTests() -> bool
{
    using namespace itemtradelisthelpers;
    auto plain     = PlanFor(2, { .extra = { 9 } });
    auto charged   = PlanFor(2, { .reserve = 1, .charged = true, .charges = 3 });
    auto linkshell = PlanFor(2, { .reserve = 1, .linkshell = true, .lsID = 0x11223344, .lsColor = 0x5566, .lsType = 7, .lsName = { 'x' } });
    if (plain.ItemNo || plain.Attr[0] != 9 || charged.Attr[0] != 1 || charged.Attr[1] != 3 || linkshell.Attr[0] != 0x44 || linkshell.Attr[3] != 0x11 || linkshell.Attr[6] != 0x66 || linkshell.Attr[8] != 7 || linkshell.Attr[9] != 'x')
    {
        std::cerr << "s2c ITEM_TRADE_LIST runtime self-test failed\n";
        return false;
    }
    return true;
}
