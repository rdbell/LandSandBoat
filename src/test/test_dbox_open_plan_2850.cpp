#include "test_dbox_open_plan_2850.h"

#include "map/utils/dbox_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "dbox open plan 2850 self-test failed: " << label << '\n';
    }
    return condition;
}

// LSB UCONTAINER_SEND_DELIVERYBOX / UCONTAINER_RECV_DELIVERYBOX pins without
// pulling universal_container.h into the capacity self-test.
constexpr uint8 UContainerSendDeliveryBox = 7;
constexpr uint8 UContainerRecvDeliveryBox = 8;

} // namespace

auto runDboxOpenPlan2850SelfTests() -> bool
{
    using dboxutilshelpers::OpenPlan;
    using dboxutilshelpers::PlanOpenRecvBox;
    using dboxutilshelpers::PlanOpenSendBox;

    bool ok = true;

    // PlanOpenSendBox: always Clean + SetType(send)
    {
        const OpenPlan plan = PlanOpenSendBox(UContainerSendDeliveryBox);
        ok                  = expect(plan.clean, "send clean true") && ok;
        ok                  = expect(plan.setType == UContainerSendDeliveryBox, "send setType 7") && ok;
    }

    // PlanOpenRecvBox: always Clean + SetType(recv)
    {
        const OpenPlan plan = PlanOpenRecvBox(UContainerRecvDeliveryBox);
        ok                  = expect(plan.clean, "recv clean true") && ok;
        ok                  = expect(plan.setType == UContainerRecvDeliveryBox, "recv setType 8") && ok;
    }

    // Injected type is passthrough (host supplies the constant)
    ok = expect(PlanOpenSendBox(0).setType == 0, "send inject 0") && ok;
    ok = expect(PlanOpenSendBox(0).clean, "send inject 0 still cleans") && ok;
    ok = expect(PlanOpenRecvBox(99).setType == 99, "recv inject 99") && ok;
    ok = expect(PlanOpenRecvBox(99).clean, "recv inject 99 still cleans") && ok;

    // Plans are independent: send type != recv type under production pins
    ok = expect(PlanOpenSendBox(UContainerSendDeliveryBox).setType !=
                    PlanOpenRecvBox(UContainerRecvDeliveryBox).setType,
                "send type != recv type") &&
         ok;

    // Default-constructed plan is inert (no clean, type 0)
    {
        const OpenPlan empty{};
        ok = expect(!empty.clean, "default clean false") && ok;
        ok = expect(empty.setType == 0, "default setType 0") && ok;
    }

    return ok;
}
