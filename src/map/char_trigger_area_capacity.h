#pragma once

namespace chartriggerareahelpers
{

template <typename Set, typename ID>
inline bool Contains(const Set& triggerAreas, const ID triggerAreaID)
{
    return triggerAreas.find(triggerAreaID) != triggerAreas.end();
}

template <typename Set, typename ID>
inline void Enter(Set& triggerAreas, const ID triggerAreaID)
{
    triggerAreas.insert(triggerAreaID);
}

template <typename Set, typename ID>
inline void Leave(Set& triggerAreas, const ID triggerAreaID)
{
    triggerAreas.erase(triggerAreaID);
}

template <typename Set>
inline void Clear(Set& triggerAreas)
{
    triggerAreas.clear();
}

} // namespace chartriggerareahelpers
