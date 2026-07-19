#pragma once

namespace petcontrollerpetskill
{
struct Plan
{
    bool faceTarget;
    bool emitBeforeUse;
    bool delegate;
};

inline auto Resolve(bool hasOwner) -> Plan
{
    return hasOwner ? Plan{ true, true, true } : Plan{};
}
} // namespace petcontrollerpetskill
