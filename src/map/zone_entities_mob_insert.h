#pragma once

namespace zoneentities
{

enum class MobInsertionList { None, Ally, Mob };
struct MobInsertionPlan
{
    bool insert{};
    MobInsertionList list{ MobInsertionList::None };
    auto operator==(const MobInsertionPlan&) const -> bool = default;
};
inline auto PlanMobInsertion(bool isMob, bool ally) -> MobInsertionPlan
{
    return isMob ? MobInsertionPlan{ true, ally ? MobInsertionList::Ally : MobInsertionList::Mob } : MobInsertionPlan{};
}
} // namespace zoneentities
