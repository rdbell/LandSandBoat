#pragma once

// Pure CInstanceLoader constructor admission. Zone lookup and instance
// allocation remain host-owned.
namespace instanceloader
{

struct CreatePlan
{
    bool createInstance{};
    bool reportInvalidZone{};

    auto operator==(const CreatePlan&) const -> bool = default;
};

inline auto PlanCreate(const bool zonePresent, const bool zoneIsInstanced) -> CreatePlan
{
    if (!zonePresent || !zoneIsInstanced)
    {
        return { false, true };
    }
    return { true, false };
}

} // namespace instanceloader
