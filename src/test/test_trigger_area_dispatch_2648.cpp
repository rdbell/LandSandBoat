#include "test_trigger_area_dispatch_2648.h"

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <vector>

#include "map/trigger_area_dispatch.h"

namespace
{

struct Occupancy
{
    uint32_t id;
    bool     inside;
};

struct Transition
{
    uint32_t                      id;
    triggerarea::MembershipAction action;

    auto operator==(const Transition&) const -> bool = default;
};

auto dispatch(bool zoningIn, std::vector<uint32_t>& members, const std::vector<Occupancy>& occupancies) -> std::vector<Transition>
{
    if (zoningIn)
    {
        return {};
    }

    std::vector<Transition> transitions;
    for (const auto& occupancy : occupancies)
    {
        const auto member = std::find(members.begin(), members.end(), occupancy.id) != members.end();
        switch (triggerarea::MembershipActionFor(false, occupancy.inside, member))
        {
            case triggerarea::MembershipAction::Enter:
                members.push_back(occupancy.id);
                transitions.push_back({ occupancy.id, triggerarea::MembershipAction::Enter });
                break;
            case triggerarea::MembershipAction::Leave:
                std::erase(members, occupancy.id);
                transitions.push_back({ occupancy.id, triggerarea::MembershipAction::Leave });
                break;
            case triggerarea::MembershipAction::None:
                break;
        }
    }
    return transitions;
}

auto testActions() -> bool
{
    return triggerarea::MembershipActionFor(true, true, false) == triggerarea::MembershipAction::None &&
           triggerarea::MembershipActionFor(true, false, true) == triggerarea::MembershipAction::None &&
           triggerarea::MembershipActionFor(false, false, false) == triggerarea::MembershipAction::None &&
           triggerarea::MembershipActionFor(false, true, true) == triggerarea::MembershipAction::None &&
           triggerarea::MembershipActionFor(false, true, false) == triggerarea::MembershipAction::Enter &&
           triggerarea::MembershipActionFor(false, false, true) == triggerarea::MembershipAction::Leave;
}

auto testOrderedDispatch() -> bool
{
    std::vector<uint32_t> members;
    const auto            enters = dispatch(false, members, { { 9, true }, { 3, true }, { 7, false } });
    if (enters != std::vector<Transition>{ { 9, triggerarea::MembershipAction::Enter }, { 3, triggerarea::MembershipAction::Enter } } ||
        members != std::vector<uint32_t>{ 9, 3 })
    {
        return false;
    }

    const auto leaves = dispatch(false, members, { { 3, false }, { 9, false } });
    if (leaves != std::vector<Transition>{ { 3, triggerarea::MembershipAction::Leave }, { 9, triggerarea::MembershipAction::Leave } } || !members.empty())
    {
        return false;
    }

    return dispatch(false, members, { { 3, false }, { 9, false } }).empty();
}

auto testZoningInSkipsMutation() -> bool
{
    std::vector<uint32_t> members{ 12 };
    return dispatch(true, members, { { 12, false }, { 8, true } }).empty() && members == std::vector<uint32_t>{ 12 };
}

} // namespace

auto runTriggerAreaDispatch2648SelfTests() -> bool
{
    const auto ok = testActions() && testOrderedDispatch() && testZoningInSkipsMutation();
    if (!ok)
    {
        std::cerr << "trigger area dispatch self-test failed\n";
    }
    return ok;
}
