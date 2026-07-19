#pragma once

namespace trustcontrollercombatmasterengagement
{
struct Plan
{
    bool disengage;
    bool clearTopEnmity;
    bool recordCombatEnd;
};

constexpr auto Resolve(const bool masterEngaged) -> Plan
{
    return masterEngaged ? Plan{} : Plan{ true, true, true };
}
} // namespace trustcontrollercombatmasterengagement
