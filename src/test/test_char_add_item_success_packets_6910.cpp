#include "test_char_add_item_success_packets_6910.h"
#include "map/char_add_item_success_packets.h"
#include <iostream>
auto runCharAddItemSuccessPackets6910SelfTests() -> bool { const bool ok = additemsuccesspackethelpers::BuildPlan() == std::array{ additemsuccesspackethelpers::Action::Attribute, additemsuccesspackethelpers::Action::Same }; if (!ok) std::cerr << "add item success packets 6910 self-test failed\n"; return ok; }
