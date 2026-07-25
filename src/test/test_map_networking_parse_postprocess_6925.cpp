#include "test_map_networking_parse_postprocess_6925.h"

#include "map/map_networking_parse_postprocess.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "map networking parse postprocess 6925 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runMapNetworkingParsePostprocess6925SelfTests() -> bool
{
    using mapnetworkingparsepostprocesshelpers::MakePlan;
    using mapnetworkingparsepostprocesshelpers::kEquipmentSlotCount;

    std::array<bool, kEquipmentSlotCount> equipped{};
    equipped[0]  = true;
    equipped[3]  = true;
    equipped[15] = true;

    bool ok = true;
    auto plan = MakePlan(false, equipped);
    ok = expect(plan.latentEquipSlotCount == 0 && !plan.resetRetriggerLatents && plan.flushEquipChanges,
                "ordinary parse only flushes equipment changes") &&
         ok;

    plan = MakePlan(true, equipped);
    ok = expect(plan.latentEquipSlotCount == 3 && plan.latentEquipSlots[0] == 0 && plan.latentEquipSlots[1] == 3 &&
                    plan.latentEquipSlots[2] == 15 && plan.resetRetriggerLatents && plan.flushEquipChanges,
                "retrigger checks occupied equipment slots in order") &&
         ok;

    plan = MakePlan(true, {});
    ok = expect(plan.latentEquipSlotCount == 0 && plan.resetRetriggerLatents && plan.flushEquipChanges,
                "empty equipment still resets and flushes") &&
         ok;

    return ok;
}
